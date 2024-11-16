#include "Network.h"
#include "NetworkHelper.h"
#include "Logger.h"
#include "IOWorker.h"
#include "Port.h"
#include <vector>

#pragma comment(lib, "ws2_32.lib")

namespace jw
{

    Network::Network() :
        _ioWorker{ nullptr },
        _NullPort{ nullptr }
    {}

    Network::~Network() {
        WSACleanup();
    }

    bool Network::Initialize(const uint16_t workerThreadCount)
    {
        _ioWorker = std::make_unique<IOWorker>();

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

        initializeIOWorkers(workerThreadCount);

        return true;
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

    std::pair<Session*, uint16_t> Network::MakeSession(const PortId_t portId)
    {
        if (auto& port = getPort(portId);
            nullptr != port)
        {
            return port->MakeSession();
        }

        return { nullptr, 0 };
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

    bool Network::initializeIOWorkers(const uint16_t workerThreadCount)
    {
        _ioWorker->Initialize(GetIOCPHandle());
        _ioWorker->RunThreads(workerThreadCount);

        return true;
    }

    std::shared_ptr<Port>& Network::getPort(const PortId_t portId)
    {
        if (_portContainer.contains(portId))
        {
            return _portContainer[portId];
        }

        LOG_ERROR(L"not find portNumber, portId:{}", portId);
        return _NullPort;
    }
}
