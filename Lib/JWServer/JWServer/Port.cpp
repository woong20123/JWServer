#include "Port.h"
#include "Network.h"
#include "Listener.h"
#include "Logger.h"
#include "Session.h"
#include "SessionBuffer.h"

namespace jw
{
    Port::Port() :
        _listerner{ nullptr },
        _portNumber{ 0 },
        _ipcpHandle{ INVALID_HANDLE_VALUE },
        _sessionList{ nullptr },
        _sessionMaxCount{ 0 }

    {
    }

    Port::~Port()
    {
        if (_sessionList)
        {
            delete[] _sessionList;
            _sessionList = nullptr;
        }
    }

    bool Port::Initialize(const PortInfo& data)
    {
        _listerner = std::make_unique<Listener>();
        _sessionList = new std::shared_ptr<Session>[data.sesionMaxCount];

        initializeSessionIndex(data.sesionMaxCount);

        if (data.id < 0 || data.portNumber <= 0 || data.iocpHandle == INVALID_HANDLE_VALUE || data.sesionMaxCount <= 0)
        {
            LOG_FETAL(L"Initialize Fail invalid parameter, id:{}, portNumber:{}, ipcpHandle:{}, sesionMaxCount:{}", data.id, data.portNumber, data.iocpHandle, data.sesionMaxCount);
            return false;
        }

        _id = data.id;
        _portNumber = data.portNumber;
        _ipcpHandle = data.iocpHandle;

        if (!_listerner->Initialize(_id, _portNumber, _ipcpHandle, NETWORK().GetAcceptExFunc(), NETWORK().GetAcceptExSockAddrFunc()))
        {
            LOG_FETAL(L"lister initialize fail, id:{}, portNumber:{}", data.id, data.portNumber);
            return false;
        }

        LOG_INFO(L"Port Initialize Success, id:{}, portNumber:{}, sesionMaxCount:{}", data.id, data.portNumber, data.sesionMaxCount);

        return true;
    }

    uint32_t Port::GetId() const
    {
        return _id;
    }

    std::pair<Session*, uint16_t> Port::MakeSession()
    {
        if (getAvailableSessionCount() == 0)
        {
            return { nullptr, 0 };
        }

        std::shared_ptr<Session> session = std::make_shared<Session>();
        if (!session) return { nullptr, 0 };

        uint16_t sessionIndex{ static_cast<uint16_t>(_sessionMaxCount) };
        {
            WRITE_LOCK(_sessionMutex);
            if (!_availableSessionIndexCon.empty())
            {
                sessionIndex = _availableSessionIndexCon.front();
                _availableSessionIndexCon.pop();
            }
        }

        if (sessionIndex < 0 || _sessionMaxCount <= sessionIndex)
        {
            LOG_FETAL(L"invalid sessionIndex, index:{}, sessionMaxCount:{}", sessionIndex, _sessionMaxCount);
            return { nullptr, 0 };
        }

        _sessionList[sessionIndex] = session;


        return { session.get(), sessionIndex };
    }

    size_t Port::getAvailableSessionCount()
    {
        READ_LOCK(_sessionMutex);
        return _availableSessionIndexCon.size();
    }

    size_t Port::getUsedSessionCount()
    {
        READ_LOCK(_sessionMutex);
        return _sessionMaxCount - _availableSessionIndexCon.size();
    }

    void Port::initializeSessionIndex(const size_t maxSessionCount)
    {
        _sessionMaxCount = maxSessionCount;

        for (uint16_t i = 0; i < maxSessionCount; ++i)
        {
            _availableSessionIndexCon.push(i);
        }
    }
}
