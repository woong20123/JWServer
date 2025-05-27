#include "Port.h"
#include "Network.h"
#include "Listener.h"
#include "Logger.h"
#include "Session.h"
#include "SessionBuffer.h"
#include "SessionHandler.h"
#include "SessionInspector.h"

namespace jw
{
    Port::Port() :
        _id{ INVALID_VALUE },
        _listerner{ nullptr },
        _portNumber{ INVALID_VALUE },
        _ipcpHandle{ INVALID_HANDLE_VALUE },
        _sessionList{ nullptr },
        _sessionMaxCount{ INVALID_VALUE },
        _sessionHandler{ nullptr },
        _packetBufferHander{ nullptr }
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

    bool Port::Initialize(const PortInfo& info)
    {
        if (info._id < INVALID_VALUE || info._portNumber <= INVALID_VALUE || info._iocpHandle == INVALID_HANDLE_VALUE || info._sesionMaxCount <= INVALID_VALUE
            || !info._sessionHandler || !info._packetBufferHandler)
        {

            LOG_FETAL(L"Initialize Fail invalid parameter, id:{}, portNumber:{}, ipcpHandle:{}, sesionMaxCount:{}, sessionHandler:{}",
                info._id, info._portNumber, info._iocpHandle, info._sesionMaxCount, reinterpret_cast<void*>(info._sessionHandler.get()));
            return false;
        }

        _listerner = std::make_unique<Listener>();
        _sessionList = new std::shared_ptr<Session>[info._sesionMaxCount];
        for (int i = 0; i < info._sesionMaxCount; ++i)
        {
            _sessionList[i] = nullptr;
        }

        _sessionHandler = info._sessionHandler;

        _packetBufferHander = info._packetBufferHandler;

        initializeSessionIndex(info._sesionMaxCount);
        _id = info._id;
        _portNumber = info._portNumber;
        _ipcpHandle = info._iocpHandle;

        if (!_listerner->Initialize(_id, _portNumber, _ipcpHandle, GetNetwork().GetAcceptExFunc(), GetNetwork().GetAcceptExSockAddrFunc()))
        {
            LOG_FETAL(L"lister initialize fail, id:{}, portNumber:{}", info._id, info._portNumber);
            return false;
        }

        _sessionInspectorStatusTable = std::make_shared<SessionInspectorInfoTable>();
        _sessionInspectorStatusTable->Initialize(_id, info._sesionMaxCount, info._recvCheckTimeSecond);

        GetNetwork().RegisterSessionInspectorInfoTable(_id, _sessionInspectorStatusTable);

        LOG_INFO(L"Port Initialize Success, id:{}, portNumber:{}, sesionMaxCount:{}", info._id, info._portNumber, info._sesionMaxCount);

        return true;
    }

    uint32_t Port::GetId() const
    {
        return _id;
    }

    Session* Port::CreateSession()
    {
        if (getAvailableSessionCount() == 0)
        {
            return nullptr;
        }

        std::shared_ptr<Session> session = std::make_shared<Session>();
        if (!session) return nullptr;

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
            LOG_FETAL(L"invalid sessionIndex, id:{}, port:{}, sessionIndex:{}, sessionMaxCount:{}", _id, _portNumber, sessionIndex, _sessionMaxCount);
            return nullptr;
        }

        SessionID sessionId = Session::MakeSessionID(sessionIndex, _id);
        if (!session->Initialize(sessionId, _sessionHandler, _packetBufferHander))
        {
            LOG_FETAL(L"session Initialize Fail, id:{}, port:{}, sessionIndex:{}, sessionMaxCount:{}", _id, _portNumber, sessionIndex, _sessionMaxCount);
            return nullptr;
        }

        session->RegisterOnClose([this, sessionIndex](uint32_t reason) {
            _sessionInspectorStatusTable->SetStateReleased(sessionIndex);
            });

        _sessionList[sessionIndex] = session;
        _sessionHandler->OnCreated(session.get());

        _sessionInspectorStatusTable->SetStateAllocated(sessionIndex);

        LOG_INFO(L"Session Create Success, id:{}, port:{}, sessionId:{}, sessionIndex:{}, sessionMaxCount:{}", _id, _portNumber, sessionId.id, sessionIndex, _sessionMaxCount);

        return session.get();
    }

    bool Port::DestroySession(Session* session)
    {
        if (!session)
            return false;

        const auto sessionIndex = session->GetIndex();
        if (sessionIndex < 0 || _sessionMaxCount <= sessionIndex)
        {
            LOG_ERROR(L"Session Index is Invalid, id:{}, sessionId:{}, sessionIndex:{}", _id, session->GetId(), sessionIndex);
            return false;
        }

        const auto sessionId = session->GetId();
        _sessionHandler->OnDestroyed(session);

        // Session 객체가 할당 해제되기 때문에 session을 사용하면 안됩니다. 
        {
            WRITE_LOCK(_sessionMutex);
            _availableSessionIndexCon.push(sessionIndex);
            _sessionList[sessionIndex] = nullptr;
        }


        LOG_INFO(L"Session Destroy Success, id:{}, port:{}, sessionId:{}, sessionIndex:{}, sessionMaxCount:{}", _id, _portNumber, sessionId, sessionIndex, _sessionMaxCount);

        return true;
    }

    std::shared_ptr<Session> Port::GetSession(const int32_t sessionIndex)
    {
        if (sessionIndex < 0 || _sessionMaxCount <= sessionIndex)
        {
            LOG_ERROR(L"Session Index is Invalid, id:{}, sessionIndex:{}", _id, sessionIndex);
            return nullptr;
        }

        READ_LOCK(_sessionMutex);
        return _sessionList[sessionIndex];
    }

    const size_t Port::GetSessionCount() const
    {
        return getUsedSessionCount();
    }

    size_t Port::getAvailableSessionCount() const
    {
        READ_LOCK(_sessionMutex);
        return _availableSessionIndexCon.size();
    }

    size_t Port::getUsedSessionCount() const
    {
        READ_LOCK(_sessionMutex);
        return _sessionMaxCount - _availableSessionIndexCon.size();
    }

    void Port::initializeSessionIndex(const size_t maxSessionCount)
    {
        _sessionMaxCount = maxSessionCount;

        for (uint16_t i = 1; i <= maxSessionCount; ++i)
        {
            _availableSessionIndexCon.push(i);
        }
    }
}
