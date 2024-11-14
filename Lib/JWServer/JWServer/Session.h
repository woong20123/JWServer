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

    class Session : public AsyncIOObject
    {
    public:
        Session();

        bool Initialize(int64_t id, SOCKET socket, sockaddr_in* remoteAddr);

        int64_t GetId() const;

        uint32_t GetAsyncObjectId() const override { return ASYNC_IO_OBJ_SESSION; }
        bool HandleEvent(OVERLAPPED* context, paramType bytes) override;

        bool OnAccept();
        bool Recv();

        static Session* MakeSession(int64_t& sessionId);

    private:
        bool asyncRecv();
        bool onRecvEvent(AsyncContext* context, paramType bytes) {}

        int64_t                                 _id;
        SOCKET                                  _socket;
        long                                    _ip;
        std::wstring                            _ipString;
        uint16_t                                _port;
        std::unique_ptr<SessionRecvBuffer>      _recvBuffer;

        std::shared_mutex                       _mutex;
    };
}

#endif // !__JW_SESSION_H__