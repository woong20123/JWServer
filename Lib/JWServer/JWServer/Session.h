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
    class Session;

    struct SessionContext : public AsyncContext
    {
        SessionContext(uint32_t asyncId) : AsyncContext(asyncId),
            _session{ nullptr }
        {}
        Session* _session;
    };

    struct AsyncRecvContext : public SessionContext
    {

        AsyncRecvContext() : SessionContext(ASYNC_CONTEXT_ID_RECV)
        {}
        WSABUF      _wsaBuffer;
    };

    struct AsyncSendContext : public SessionContext
    {
        AsyncSendContext() : SessionContext(ASYNC_CONTEXT_ID_SEND)
        {}
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

    class Session : public AsyncIOObject
    {
    public:
        Session();

        bool Initialize(SessionID sessionId, SOCKET socket, sockaddr_in* remoteAddr);

        int64_t GetId() const;
        int16_t GetPortId() const;
        int32_t GetIndex() const;

        uint32_t GetAsyncObjectId() const override { return ASYNC_IO_OBJ_SESSION; }
        bool HandleEvent(AsyncContext* context, paramType bytes) override;

        bool OnAccept();
        bool Recv();

        static SessionID MakeSessionID(uint32_t index, uint16_t portId);

    private:
        bool asyncRecv();
        bool onRecvEvent(AsyncContext* context, paramType bytes) {}

        SessionID                               _id;
        SOCKET                                  _socket;
        long                                    _ip;
        std::wstring                            _ipString;
        uint16_t                                _port;
        std::unique_ptr<SessionRecvBuffer>      _recvBuffer;

        std::shared_mutex                       _mutex;
    };
}

#endif // !__JW_SESSION_H__