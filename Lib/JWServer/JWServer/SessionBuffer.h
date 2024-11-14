#pragma once
#ifndef __JW_SESSION_BUFFER_H__
#define __JW_SESSION_BUFFER_H__
#include <cstdint>
#include <memory>

namespace jw
{
    struct AsyncRecvContext;
    class SessionRecvBuffer
    {
    public:
        using bufferType = char;
        static constexpr size_t BUFFER_MAX_SIZE = 4000;
        SessionRecvBuffer();
        ~SessionRecvBuffer();

        bufferType* GetFreeBuffer();
        uint32_t        GetFreeBufferSize() const;

        uint32_t        UpdateRecvedSize(uint32_t recvedSize);
        AsyncRecvContext* GetContext() const;
    private:

        std::unique_ptr<AsyncRecvContext> _context;
        uint32_t    _recvedSize;
        bufferType  _buffer[BUFFER_MAX_SIZE];
    };
}
#endif

