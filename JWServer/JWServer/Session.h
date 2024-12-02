#pragma once
class Session
{
};

#pragma once
#ifndef __JW_SESSION_H__
#define __JW_SESSION_H__
#include "AsyncIOObject.h"
#include <shared_mutex>

namespace jw
{
    class SessionRecvBuffer;
    class SessionSendBuffer;
    struct SendBufferList;
    class Session;
    class SessionHandler;
    class PacketBufferHandler;

    struct SessionContext : public AsyncContext
    {
        SessionContext(uint32_t asyncId) : AsyncContext(asyncId),
            _session{ nullptr }
        {}
        Session* _session;
    };

    struct AsyncRecvContext : public SessionContext
    {

        AsyncRecvContext() : SessionContext(ASYNC_CONTEXT_ID_RECV), _wsaBuffer{ 0, nullptr }
        {}
        WSABUF      _wsaBuffer;
    };

    struct AsyncSendContext : public SessionContext
    {
        AsyncSendContext() : SessionContext(ASYNC_CONTEXT_ID_SEND), _sendBuffer{ nullptr }
        {}
        std::shared_ptr<SendBufferList> _sendBuffer;
    };

    struct AsyncConnectContext : public SessionContext
    {
        AsyncConnectContext() : SessionContext(ASYNC_CONTEXT_ID_CONNECT)
        {}
    };

    union SessionID
    {
        uint64_t id;
        struct
        {
            uint32_t index;
            uint16_t portId;
            uint16_t padding;
        };
    };

    enum class SessionState : uint16_t
    {
        SESSION_STATE_NONE,
        SESSION_STATE_CREATE,
        SESSION_STATE_CONNECTING,
        SESSION_STATE_CONNECTED,
        SESSION_STATE_CLOSING,
        SESSION_STATE_CLOSED,
        SESSION_STATE_MAX,
    };

    constexpr const wchar_t* SessionStateStr[static_cast<size_t>(SessionState::SESSION_STATE_MAX)] =
    {
        L"NONE",
        L"CREATE",
        L"CONNECTING",
        L"CONNECTED",
        L"CLOSING",
        L"CLOSED",
    };

    enum class CloseReason : uint32_t
    {
        CLOSE_REASON_NONE = 0,
        CLOSE_REASON_UNKNOWN,
        CLOSE_REASON_ACCEPT_FAIL,
        CLOSE_REASON_RECV_FAIL,
        CLOSE_REASON_SEND_FAIL,
        CLOSE_REASON_CLIENT_DISCONNECTED,
    };

    class Session : public AsyncIOObject
    {
    public:
        Session();

        bool Initialize(SessionID sessionId, std::shared_ptr<SessionHandler>& sessionHandler, std::shared_ptr<PacketBufferHandler>& packetBufferHandelr);
        bool SetSocketInfo(SOCKET socket, sockaddr_in* remoteAddr);

        int64_t GetId() const;
        int16_t GetPortId() const;
        int32_t GetIndex() const;

        uint32_t GetAsyncObjectId() const override { return ASYNC_IO_OBJ_SESSION; }
        bool HandleEvent(AsyncContext* context, paramType bytes) override;
        void HandleFailedEvent(AsyncContext* context, paramType param) override;


        bool OnAccept();
        bool Recv();
        bool Send(const void* byteStream, const size_t byteCount);
        bool Close(CloseReason reason = CloseReason::CLOSE_REASON_UNKNOWN);
        void Dispose();

        static SessionID MakeSessionID(uint32_t index, uint16_t portId);

        SessionState GetState() const { return _state; }
        bool IsConnected() const;
        bool IsClosed() const;

        static const wchar_t* GetStateToStr(SessionState state);

    private:
        bool asyncRecv();
        bool asyncSend(const void* byteStream, const size_t byteCount);
        bool asyncSend(AsyncSendContext* sendContext);
        bool onRecvEvent(AsyncContext* context, paramType bytes) {}

        bool updateRecvedSize(paramType bytes);
        bool handlePacket();

        void setState(SessionState state);

        SessionID                               _id;
        SOCKET                                  _socket;
        long                                    _ip;
        std::wstring                            _ipString;
        uint16_t                                _port;
        std::unique_ptr<SessionRecvBuffer>      _recvBuffer;
        std::unique_ptr<SessionSendBuffer>      _sendBuffer;
        std::shared_mutex                       _mutex;
        std::shared_ptr<SessionHandler>         _sessionHandler;
        SessionState                            _state;
        std::shared_ptr<PacketBufferHandler>    _packetBufferHandler;
    };
}

#endif // !__JW_SESSION_H__