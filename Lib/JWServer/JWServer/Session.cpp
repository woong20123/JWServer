#include "Session.h"
#include "SessionBuffer.h"
#include "SessionHandler.h"
#include "Network.h"
#include "NetworkHelper.h"
#include "Logger.h"
#include "TypeDefinition.h"

namespace jw
{
    Session::Session() :
        _id{ INVALID_VALUE },
        _port{ INVALID_VALUE },
        _socket{ INVALID_SOCKET },
        _recvBuffer{ nullptr },
        _sessionHandler{ nullptr },
        _state{ SessionState::SESSION_STATE_NONE }
    {
    }

    bool Session::Initialize(SessionID sessionId, std::shared_ptr<SessionHandler>& sessionHandler)
    {
        if (INVALID_VALUE == sessionId.id || !sessionHandler)
        {
            LOG_ERROR(L"invalid parameter, id:{}, sessionHandler:{}", sessionId.id, reinterpret_cast<void*>(sessionHandler.get()));
            return false;
        }

        _id = sessionId;
        _sessionHandler = sessionHandler;
        _recvBuffer = std::make_unique<SessionRecvBuffer>();

        setState(SessionState::SESSION_STATE_CREATE);

        LOG_INFO(L"Session Initialize, id:{}, ip:{}, port:{}", GetId(), _ipString.c_str(), _port);

        return true;
    }
    bool Session::SetSocketInfo(SOCKET socket, sockaddr_in* remoteAddr)
    {
        if (INVALID_SOCKET == socket || nullptr == remoteAddr)
        {
            LOG_ERROR(L"invalid parameter, id:{}, socket:{}, ip:{}", GetId(), socket);
            return false;
        }

        _socket = socket;
        _ip = remoteAddr->sin_addr.S_un.S_addr;
        _port = remoteAddr->sin_port;

        wchar_t ipAddress[32]{ 0, };
        _ipString = InetNtopW(AF_INET, &remoteAddr->sin_addr, ipAddress, 16);

        LOG_INFO(L"Session Socket Info, id:{}, ip:{}, port:{}", GetId(), _ipString.c_str(), _port);

        return true;
    }

    int64_t Session::GetId() const { return _id.id; }
    int16_t Session::GetPortId() const { return _id.portId; }
    int32_t Session::GetIndex() const { return _id.index; }

    bool Session::HandleEvent(AsyncContext* context, paramType bytes)
    {
        auto* asyncContext{ static_cast<AsyncContext*>(context) };
        switch (asyncContext->_id)
        {
        case ASYNC_CONTEXT_ID_RECV:
            if (0 == bytes) {
                LOG_ERROR(L"recv zero, socket has closed, id:{}", GetId());
                Close(CloseReason::CLOSE_REASON_CLIENT_DISCONNECTED);
                return false;
            }

            if (0 == _recvBuffer->UpdateRecvedSize(bytes))
            {
                LOG_ERROR(L"UpdateRecvedSize fail, id:{}", GetId());
                return false;
            }

            // 패킷 핸들
            LOG_DEBUG(L"on async recved, id:{}, bytes:{}", GetId(), bytes);
            if (!Recv())
            {
                LOG_ERROR(L"next async recv error, id:{}, error:{}", GetId(), WSAGetLastError());
                return false;
            }
            break;
        case ASYNC_CONTEXT_ID_SEND:
            break;
        case ASYNC_CONTEXT_ID_CONNECT:
            break;
        default:
            break;
        }

        return true;
    }

    void Session::HandleFailedEvent(AsyncContext* context, paramType param)
    {
        if (!IsClosed())
            Close(CloseReason::CLOSE_REASON_UNKNOWN);
    }

    bool Session::OnAccept()
    {
        if (!NetworkHelper::AssociateDeviceWithIOCP(reinterpret_cast<HANDLE>(_socket), NETWORK().GetIOCPHandle(), reinterpret_cast<uint64_t>(this)))
        {
            LOG_FETAL(L"sessionSocket fail AssociateDeviceWithIOCP, id:{}, ip:{}, port:{}", GetId(), _ipString.c_str(), _port);
            return false;
        }

        if (!_sessionHandler->OnAccepted(this))
        {
            Close(CloseReason::CLOSE_REASON_ACCEPT_FAIL);
            return false;
        }

        setState(SessionState::SESSION_STATE_CONNECTED);

        LOG_INFO(L"OnAccept Session, id:{}, ip:{}, port:{}", GetId(), _ipString.c_str(), _port);
        return true;
    }

    bool Session::Recv()
    {
        WRITE_LOCK(_mutex);
        if (!asyncRecv())
        {
            Close(CloseReason::CLOSE_REASON_RECV_FAIL);
            return false;
        }
        return true;
    }

    bool Session::Close(CloseReason reason)
    {
        uint32_t iReason = static_cast<uint32_t>(reason);
        if (INVALID_SOCKET == _socket)
        {
            LOG_ERROR(L"socket is invalid, id:{}, ip:{}, port:{}, reason:{}", GetId(), _ipString.c_str(), _port, iReason);
            return false;
        }

        ::closesocket(_socket);
        _socket = INVALID_SOCKET;
        setState(SessionState::SESSION_STATE_CLOSED);

        _sessionHandler->OnClosed(this);

        /*if (!NETWORK().DestroySession(GetPortId(), this))
        {
            LOG_ERROR(L"DestorySession Fail, id:{}, ip:{}, port:{}, reason:{}", GetId(), _ipString.c_str(), _port, iReason);
            return false;
        }*/

        LOG_INFO(L"Session is Close, id:{}, ip:{}, port:{}, reason:{}", GetId(), _ipString.c_str(), _port, iReason);
        return true;
    }

    bool Session::IsConnected() const
    {
        return _state == SessionState::SESSION_STATE_CONNECTED;
    }
    bool Session::IsClosed() const
    {
        return _state == SessionState::SESSION_STATE_CLOSED;
    }

    bool Session::asyncRecv()
    {
        auto recvContext = _recvBuffer->GetContext();
        recvContext->hEvent = NULL;
        recvContext->_session = this;
        recvContext->_wsaBuffer.len = _recvBuffer->GetFreeBufferSize();
        recvContext->_wsaBuffer.buf = _recvBuffer->GetFreeBuffer();

        unsigned long recvedSize{ 0 }, recvedFlag{ 0 };

        if (SOCKET_ERROR == ::WSARecv(_socket, &recvContext->_wsaBuffer, 1, &recvedSize, &recvedFlag, recvContext, NULL))
        {
            const auto errorCode{ ::WSAGetLastError() };
            if (WSA_IO_PENDING != errorCode)
            {
                LOG_FETAL(L"WSARecv() fail, err:{}", errorCode);
                return false;
            }
        }

        return true;
    }

    void Session::setState(SessionState state)
    {
        const auto oldState = static_cast<uint16_t>(_state);
        const auto newState = static_cast<uint16_t>(state);
        _state = state;
        LOG_DEBUG(L"Session set state, id:{}, oldState:{}, newState:{}", GetId(), oldState, newState);
    }

    SessionID Session::MakeSessionID(uint32_t index, uint16_t portId)
    {
        SessionID sessionId;
        sessionId.padding = 0;
        sessionId.index = index;
        sessionId.portId = portId;
        return sessionId;
    }
}

