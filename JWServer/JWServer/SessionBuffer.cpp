#include "SessionBuffer.h"
#include "Session.h"
#include "Logger.h"
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


    SessionSendBuffer::SessionSendBuffer() :
        _curSendingBufferList{ nullptr },
        _allocateBufferListSendingIdx{ 0 },
        _context{ std::make_unique<AsyncSendContext>() }
    {
        for (int i = 0; i < BUFFER_LIST_SIZE; ++i)
        {
            _bufferList[i]._index = static_cast<uint16_t>(i);
            _bufferList[i]._sendSize = 0;
            _bufferList[i]._wsaBuffer.buf = nullptr;
            _bufferList[i]._wsaBuffer.len = 0;
            ::memset(_bufferList[i]._buffer, 0, sizeof(_bufferList[i]._buffer));
        }

    }

    SessionSendBuffer::~SessionSendBuffer()
    {

    }

    bool SessionSendBuffer::Add(const void* byteStream, const size_t byteCount)
    {
        const auto nextAllocateIndex = ++_allocateBufferListSendingIdx % BUFFER_LIST_SIZE;

        if (_curSendingBufferList && nextAllocateIndex == _curSendingBufferList->_index)
        {
            LOG_ERROR(L"overflow send byteStream Count, byteCount:{}", byteCount);
            return false;
        }

        if (SendBufferList::BUFFER_MAX_SIZE < byteCount)
        {
            LOG_FETAL(L"overflow send byteStream Count, byteCount:{}", byteCount);
            return false;
        }

        auto& allocateBuffer = _bufferList[_allocateBufferListSendingIdx];

        ::memcpy(allocateBuffer._buffer, byteStream, byteCount);
        allocateBuffer._wsaBuffer.buf = (char*)byteStream;
        allocateBuffer._wsaBuffer.len = (unsigned long)byteCount;

        _allocateBufferListSendingIdx = static_cast<int16_t>(nextAllocateIndex);

        if (_curSendingBufferList)
            return false;

        _context->_sendBuffer = _curSendingBufferList = &allocateBuffer;

        return true;
    }

    AsyncSendContext* SessionSendBuffer::GetContext() const
    {
        return _context.get();
    }
}