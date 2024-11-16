#pragma once
#ifndef __JW_ASYNCIO_OBJECT_H__
#define __JW_ASYNCIO_OBJECT_H__
#include <ws2tcpip.h>
#include <mswsock.h>
#include <cstdint>

namespace jw
{
    enum
    {
        ASYNC_CONTEXT_ID_NONE,
        ASYNC_CONTEXT_ID_ACCEPT,
        ASYNC_CONTEXT_ID_RECV,
        ASYNC_CONTEXT_ID_SEND,
        ASYNC_CONTEXT_ID_CONNECT,
    };

    struct AsyncContext : public OVERLAPPED
    {
        AsyncContext(uint32_t id) : _id{ id }
        {
            hEvent = nullptr;
            Internal = 0;
            InternalHigh = 0;
            Offset = 0;
            OffsetHigh = 0;
        }
        uint32_t _id;
    };

    class AsyncIOObject
    {
    public:
        using paramType = unsigned long;

        enum
        {
            ASYNC_IO_OBJ_NONE = 0,
            ASYNC_IO_OBJ_LISTENER,
            ASYNC_IO_OBJ_SESSION,
            ASYNC_IO_OBJ_MAX
        };

        virtual uint32_t GetAsyncObjectId() const = 0;
        virtual bool HandleEvent(AsyncContext* context, paramType param) = 0;
        virtual void HandleFailedEvent(AsyncContext* context, paramType param) { };
    };
}

#endif // !__JW_ASYNCIO_OBJECT_H__git