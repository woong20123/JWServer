#include "Network.h"
#include "NetworkHelper.h"
#include "Logger.h"
#include "IOWorker.h"
#include "Port.h"
#include "Session.h"
#include "SessionInspector.h"
#include "TimeUtil.h"
#include "BadIpBlock.h"
#include <vector>

#pragma comment(lib, "ws2_32.lib")

namespace jw
{
    Network::Network()
    {
    }

    Network::~Network() {
        WSACleanup();
    }

    bool Network::Initialize()
    {
        if (!initializeWinSock())
        {
            LOG_FETAL(L"initializeWinSock Fail");
            return false;
        }

        _iocpHandle = NetworkHelper::CreateNewIOCP();
        if (NULL == _iocpHandle || INVALID_HANDLE_VALUE == _iocpHandle)
        {
            LOG_FETAL(L"CreateIOCPHandle Fail, _iocpHandle:{}", _iocpHandle);
            return false;
        }

        if (!initializeWSASocketFunc())
        {
            LOG_FETAL(L"initializeWSASocketFunc Fail");
            return false;
        }

        _sessionInspector = std::make_unique<SessionInspector>();
        _sessionInspector->Initialize(true, TimeUtil::SECOND_TO_MILLISECOND * 10);

        _badIpBlock = std::make_shared<BadIpBlock>();
        _badIpBlock->Initalize();

        return true;
    }

    void Network::SetBadIpBlockOption(const BadIpBlockOption& option)
    {
        _badIpBlock->SetOption({ option._sanctionsTimeSecond, option._triggeringSessionCount, option._thresholdCheckedCount });
    }

    bool Network::Start(uint16_t& workerThreadCount)
    {

        if (Network::DEFAULT_WORKER_THREAD_COUNT == workerThreadCount)
        {
            workerThreadCount = NetworkHelper::GetProcessorCount() * 2;
        }

        _workerThreadCount = workerThreadCount;

        for (int i = 0; i < workerThreadCount; ++i)
        {
            auto ioWorker = std::make_unique<IOWorker>();
            ioWorker->Initialize(GetIOCPHandle());
            ioWorker->SetId(i);
            ioWorker->RunThread();
            _ioWorkers.push_back(std::move(ioWorker));
        }

        _sessionInspector->RunThread();

        LOG_INFO(L"Network start, workerThreadCount:{}", _workerThreadCount);
        return true;
    }

    void Network::Stop()
    {
        for (const auto [key, port] : _portContainer)
        {
            port->Stop();
            LOG_INFO(L"Port({}) stopped", key);
        }

        if (_sessionInspector)
            _sessionInspector->Stop();

        for (const auto& worker : _ioWorkers)
        {
            worker->Stop();
        }

    }

    LPFN_ACCEPTEX Network::GetAcceptExFunc() const
    {
        return _acceptExFunc;
    }

    LPFN_GETACCEPTEXSOCKADDRS Network::GetAcceptExSockAddrFunc() const
    {
        return _getAcceptExSockAddrFunc;
    }

    LPFN_CONNECTEX Network::GetConnectExFunc() const
    {
        return _connectExFunc;
    }

    LPFN_DISCONNECTEX Network::GetisConnectExFunc() const
    {
        return _disConnectExFunc;
    }

    HANDLE Network::GetIOCPHandle() const
    {
        return _iocpHandle;
    }

    bool Network::RegistPort(const Network::PortId_t portId, std::shared_ptr<Port>& port)
    {
        if (portId == INVALID_PORT_ID_TYPE || port == nullptr)
        {
            LOG_FETAL(L"invalid parameter, portId:{}", portId);
            return false;
        }

        if (_portContainer.contains(portId))
        {
            LOG_FETAL(L"already registed port, portId:{}", portId);
            return false;
        }

        _portContainer[portId] = port;

        return true;
    }

    std::shared_ptr<Session> Network::CreateSession(const PortId_t portId)
    {
        if (const auto port = getPort(portId);
            nullptr != port)
        {
            return port->CreateSession();
        }

        LOG_ERROR(L"not find port, portId:{}", portId);
        return nullptr;
    }

    bool Network::DestroySession(const PortId_t portId, Session* session)
    {
        if (nullptr == session)
        {
            LOG_ERROR(L"session is nullptr");
            return false;
        }

        if (const auto port = getPort(portId);
            nullptr != port)
        {
            return port->DestroySession(session);
        }

        LOG_ERROR(L"not find port, portId:{}", portId);
        return false;
    }

    std::shared_ptr<Session> Network::GetSession(uint64_t sessionId)
    {
        SessionID structSessionId{ sessionId };
        return GetSession(structSessionId.portId, structSessionId.index);
    }

    std::shared_ptr<Session> Network::GetSession(const PortId_t portId, const int32_t sessionIndex)
    {
        if (const auto port = getPort(portId);
            nullptr != port)
        {
            return port->GetSession(sessionIndex);
        }

        return nullptr;
    }

    bool Network::RegisterSessionInspectorInfoTable(const Network::PortId_t portId, std::shared_ptr<SessionInspectorInfoTable>& table)
    {
        if (_sessionInspector)
        {
            _sessionInspector->RegisterTable(portId, table);
            return true;
        }
        return false;
    }

    size_t Network::GetSessionCount(const PortId_t portId)
    {
        if (const auto port = getPort(portId);
            nullptr != port)
        {
            return port->GetSessionCount();
        }
        return 0L;
    }

    void Network::RegisterBadIp(const uint32_t address)
    {
        if (_badIpBlock)
            _badIpBlock->RegisterBadIp(address);
    }

    const bool Network::IsBadIp(const uint32_t address, int32_t sessionCount)
    {
        if (_badIpBlock)
            return _badIpBlock->IsBadIp(address, sessionCount);
        return false;
    }

    bool Network::initializeWinSock()
    {
        WSADATA	wsaData;
        const auto ret = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (0 != ret)
        {
            LOG_FETAL(L"WSAStartup Fail, ret:{}", ret);
            return false;
        }
        return true;
    }

    bool Network::initializeWSASocketFunc()
    {
        ULONG bytes{ 0 };

        SOCKET s = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
        if (INVALID_SOCKET == s) {
            LOG_FETAL(L"WSASocket Error err:{}", ::WSAGetLastError());
            return false;
        }

        std::vector<std::tuple<std::wstring_view, GUID, LPVOID, DWORD>> functionInfos = {
            {L"WSAID_ACCEPTEX",             WSAID_ACCEPTEX,                 _acceptExFunc == nullptr ? &_acceptExFunc : nullptr,
                sizeof(_acceptExFunc)},
            {L"WSAID_GETACCEPTEXSOCKADDRS", WSAID_GETACCEPTEXSOCKADDRS,     _getAcceptExSockAddrFunc == nullptr ? &_getAcceptExSockAddrFunc : nullptr,
                sizeof(_getAcceptExSockAddrFunc)},
            {L"WSAID_CONNECTEX",            WSAID_CONNECTEX,                _connectExFunc == nullptr ? &_connectExFunc : nullptr,
                sizeof(_connectExFunc)},
            {L"WSAID_DISCONNECTEX",         WSAID_DISCONNECTEX,             _disConnectExFunc == nullptr ? &_disConnectExFunc : nullptr,
                sizeof(_disConnectExFunc)},
        };

        for (const auto& funcInfo : functionInfos)
        {
            auto guid = std::get<1>(funcInfo);
            LPVOID func = std::get<2>(funcInfo);
            const auto funcSize = std::get<3>(funcInfo);
            if (func)
            {
                if (SOCKET_ERROR == ::WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER,
                    &guid, sizeof(guid), func, funcSize, &bytes, nullptr, nullptr))
                {

                    LOG_FETAL(L"WSAIoctl Error func:{}, errcode:{}", std::get<0>(funcInfo), ::WSAGetLastError());
                    return false;
                }
            }
        }
        return true;
    }

    std::shared_ptr<Port> Network::getPort(const PortId_t portId)
    {
        if (_portContainer.contains(portId))
        {
            return _portContainer[portId];
        }

        LOG_ERROR(L"not find portNumber, portId:{}", portId);
        return nullptr;
    }

    Network& GetNetwork() { return Network::GetInstance(); }
}
