#include "SessionBuffer.h"
#include "Session.h"
#include <memory.h>

namespace jw
{
    SessionRecvBuffer::SessionRecvBuffer() :
        _recvedSize{ 0 },
        _context{ std::make_unique<AsyncRecvContext>() }
    {
        ::memset(_buffer, 0x00, sizeof(_buffer));
    }
    SessionRecvBuffer::~SessionRecvBuffer()
    {}

    SessionRecvBuffer::bufferType* SessionRecvBuffer::GetFreeBuffer()
    {
        return _buffer + _recvedSize;
    }

    uint32_t SessionRecvBuffer::GetFreeBufferSize() const
    {
        return BUFFER_MAX_SIZE - _recvedSize;
    }

    uint32_t SessionRecvBuffer::UpdateRecvedSize(uint32_t recvedSize)
    {
        if (0 == recvedSize || BUFFER_MAX_SIZE < recvedSize + _recvedSize)
            return 0;

        _recvedSize += recvedSize;
        return _recvedSize;
    }

    AsyncRecvContext* SessionRecvBuffer::GetContext() const
    {
        return _context.get();
    }
}