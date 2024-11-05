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
    };
    struct AsyncContext : public OVERLAPPED
    {
        AsyncContext(uint32_t id) : id{id}
        {}
        uint32_t id;
    };

    class AsyncIOObject
    {
    public:
        enum
        {
            ASYNC_IO_OBJ_NONE = 0,
            ASYNC_IO_OBJ_LISTENER,
            ASYNC_IO_OBJ_MAX
        };

        virtual uint32_t GetID() const = 0;
        virtual bool HandleEvent(OVERLAPPED* context, unsigned long param) = 0;
    };
}

#endif // !__JW_ASYNCIO_OBJECT_H__