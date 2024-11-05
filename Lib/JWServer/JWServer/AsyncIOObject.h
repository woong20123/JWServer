#pragma once
#ifndef __JW_ASYNCIO_OBJECT_H__
#define __JW_ASYNCIO_OBJECT_H__
#include <ws2tcpip.h>
#include <mswsock.h>
#include <cstdint>

namespace jw
{
    class AsyncIOObject
    {
    public:
        virtual uint32_t GetEventID() const = 0;
        virtual bool HandleEvent(OVERLAPPED* context, unsigned long param) = 0;
    };
}

#endif // !__JW_ASYNCIO_OBJECT_H__