#include "Session.h"
#include "SessionHandler.h"
#include "Network.h"
#include "NetworkHelper.h"
#include "Logger.h"
#include "TypeDefinition.h"
#include "PacketBufferHandler.h"
#include "Packet.h"

namespace jw
{
    Session::Session() :
        _id{ INVALID_VALUE },
        _ip{ 0 },
        _port{ INVALID_VALUE },
        _socket{ INVALID_SOCKET },
        _recvBuffer{ nullptr },
        _sessionHandler{ nullptr },
        _state{ SessionState::SESSION_STATE_NONE },
        _packetBufferHandler{ nullptr },
        _channelKey{ INVALID_VALUE }
    {
    }

    bool Session::Initialize(SessionID sessionId, std::shared_ptr<SessionHandler>& sessionHandler, std::shared_ptr<PacketBufferHandler>& packetBufferHandelr)
    {
        if (INVALID_VALUE == sessionId.id || !sessionHandler)
        {
            LOG_ERROR(L"invalid parameter, id:{}, sessionHandler:{}", sessionId.id, reinterpret_cast<void*>(sessionHandler.get()));
            return false;
        }

        _id = sessionId;
        _sessionHandler = sessionHandler;
        _recvBuffer = std::make_unique<SessionRecvBuffer>();
        _sendBuffer = std::make_unique<SessionSendBuffer>();
        _packetBufferHandler = std::move(packetBufferHandelr);

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
        {
            if (0 == bytes) {
                LOG_ERROR(L"recv zero, socket has closed, id:{}", GetId());
                Close(CloseReason::CLOSE_REASON_CLIENT_DISCONNECTED);
                return false;
            }

            if (!updateRecvedSize(bytes))
            {
                LOG_ERROR(L"UpdateRecvedSize fail, id:{}", GetId());
                return false;
            }

            if (!handlePacket())
            {
                LOG_ERROR(L"handlePacket fail, id:{}", GetId());
                return false;
            }

            // 패킷 핸들
            LOG_DEBUG(L"on async recved, id:{}, bytes:{}", GetId(), bytes);
            if (!Recv())
            {
                LOG_ERROR(L"next async recv error, id:{}, error:{}", GetId(), WSAGetLastError());
                return false;
            }
        }
        break;
        case ASYNC_CONTEXT_ID_SEND:
        {
            if (0 == bytes) {
                LOG_ERROR(L"recv zero, socket has closed, id:{}", GetId());
                Close(CloseReason::CLOSE_REASON_CLIENT_DISCONNECTED);
                return false;
            }

            updateSentSize(bytes);
        }
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

        // 세션을 반납합니다. 
        Dispose();
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

    bool Session::Send(Packet& packet)
    {
        return Send(packet.GetHeader(), packet.GetTotalSize());
    }

    bool Session::Send(const void* byteStream, const size_t byteCount)
    {
        if (!asyncSend(byteStream, byteCount))
        {
            Close(CloseReason::CLOSE_REASON_SEND_FAIL);
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

        // 이곳까지 왔다면 session은 반납 상태입니다. 
        LOG_INFO(L"Session is Close, id:{}, ip:{}, port:{}, reason:{}", GetId(), _ipString.c_str(), _port, iReason);
        return true;
    }

    void Session::Dispose()
    {
        const auto id = GetId();
        const auto ipString = _ipString;
        const auto port = _port;

        if (!NETWORK().DestroySession(GetPortId(), this))
        {
            LOG_ERROR(L"DestorySession Fail, id:{}, ip:{}, port:{}", id, ipString.c_str(), port);
        }
    }

    bool Session::IsConnected() const
    {
        return _state == SessionState::SESSION_STATE_CONNECTED;
    }
    bool Session::IsClosed() const
    {
        return _state == SessionState::SESSION_STATE_CLOSED;
    }

    const wchar_t* Session::GetStateToStr(SessionState state)
    {
        return SessionStateStr[static_cast<size_t>(state)];
    }

    void Session::SetChannelKey(const int64_t channelKey)
    {
        _channelKey = channelKey;
    }

    int64_t Session::GetChannelKey() const
    {
        return _channelKey;
    }

    bool Session::asyncRecv()
    {
        auto recvContext = _recvBuffer->GetContext();
        recvContext->hEvent = NULL;
        recvContext->_session = this;
        recvContext->_wsaBuffer.len = _recvBuffer->GetFreeBufferSize();
        recvContext->_wsaBuffer.buf = _recvBuffer->GetFreeBuffer();

        unsigned long recvedSize{ 0 }, recvedFlag{ 0 };

        if (SOCKET_ERROR == ::WSARecv(_socket, &recvContext->_wsaBuffer, 1,
            &recvedSize, &recvedFlag, recvContext, NULL))
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

    bool Session::asyncSend(const void* byteStream, const size_t byteCount)
    {
        if (nullptr == byteStream || 0 == byteCount)
            return false;

        auto [isAdded, isAsyncSend] = _sendBuffer->Add(byteStream, byteCount);
        if (!isAdded)
            return false;

        if (isAsyncSend)
        {
            auto sendContext = _sendBuffer->GetContext();
            asyncSend(sendContext);
        }

        return true;
    }

    bool Session::asyncSend(AsyncSendContext* sendContext)
    {
        unsigned long sendBytes{ 0 }, sentFlag{ 0 };
        if (SOCKET_ERROR == ::WSASend(_socket, &sendContext->_sendBuffer->_wsaBuffer, 1,
            &sendBytes, sentFlag, sendContext, NULL))
        {
            const auto errorCode{ ::WSAGetLastError() };
            if (WSA_IO_PENDING != errorCode)
            {
                LOG_FETAL(L"WSASend() fail, err:{}", errorCode);
                return false;
            }
        }

        return true;
    }

    bool Session::updateRecvedSize(paramType bytes)
    {
        if (0 == _recvBuffer->UpdateRecvedSize(bytes))
        {
            return false;
        }
        return true;
    }

    bool Session::handlePacket()
    {
        Packet packet;
        uint32_t handledSize{ 0 };
        uint32_t bufferSize{ _recvBuffer->GetRecvedSize() };

        const auto packetHeaderSize = _packetBufferHandler->PacketHaederSize();

        while (packetHeaderSize <= bufferSize)
        {
            PacketBufferInfo packetInfo = _packetBufferHandler->EnableToPacket(_recvBuffer->GetBuffer() + handledSize, bufferSize);

            if (packetInfo._isError)
                return false;

            if (!packetInfo._packetBuffer)
                break;

            packet.SetBuffer(packetInfo._packetBuffer);

            _sessionHandler->OnPacket(this, packet);
            handledSize += packetInfo._packetSize;
            bufferSize -= packetInfo._packetSize;
        }

        if (0 < handledSize)
        {
            if (!_recvBuffer->EraseHandledData(handledSize))
            {
                LOG_FETAL(L"Fail to erase the amount handled from the buffer, id:{} handledSize:{}", GetId(), handledSize);
                return false;
            }
        }

        return true;
    }

    bool Session::updateSentSize(paramType bytes)
    {
        auto [sentSize, isSetNextSendBuffer] = _sendBuffer->UpdateSentSizeAndSetNextSendBuffer(bytes);
        if (0 == sentSize)
        {
            LOG_ERROR(L"UpdateSentSize fail, id:{}", GetId());
            return false;
        }

        // 연속된 sendbuffer가 있다면 asyncSend를 호출 합니다.
        if (isSetNextSendBuffer)
        {
            asyncSend(_sendBuffer->GetContext());
        }
        return true;
    }

    void Session::setState(SessionState state)
    {
        const auto oldState = _state;
        const auto newState = state;
        _state = state;
        LOG_DEBUG(L"Session set state, id:{}, oldState:{}, newState:{}", GetId(), Session::GetStateToStr(oldState), Session::GetStateToStr(newState));
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

