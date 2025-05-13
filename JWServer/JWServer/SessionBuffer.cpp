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

    SessionRecvBuffer::bufferType* SessionRecvBuffer::GetBuffer()
    {
        return _buffer;
    }

    SessionRecvBuffer::bufferType* SessionRecvBuffer::GetFreeBuffer()
    {
        return _buffer + _recvedSize;
    }

    uint32_t SessionRecvBuffer::GetFreeBufferSize() const
    {
        return BUFFER_MAX_SIZE - _recvedSize;
    }

    uint32_t SessionRecvBuffer::UpdateRecvedSize(const uint32_t recvedSize)
    {
        if (0 == recvedSize || BUFFER_MAX_SIZE < recvedSize + _recvedSize)
            return 0;

        _recvedSize += recvedSize;
        return _recvedSize;
    }

    bool SessionRecvBuffer::EraseHandledData(const uint32_t handledSize)
    {
        if (_recvedSize < handledSize)
            return false;
        const auto remainSize = _recvedSize - handledSize;
        ::memmove(_buffer, _buffer + handledSize, handledSize);
        _recvedSize = remainSize;
        return true;
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
    }

    SessionSendBuffer::~SessionSendBuffer() = default;

    std::pair<bool, bool> SessionSendBuffer::Add(const void* byteStream, const size_t byteCount)
    {
        bool isAdd{ false }, isAsyncSend{ false };

        if (SendBufferList::BUFFER_MAX_SIZE < byteCount)
        {
            LOG_FETAL(L"overflow send byteStream Count, byteCount:{}", byteCount);
            return { isAdd, isAsyncSend };
        }

        auto allocateBuffer = std::make_shared<SendBufferList>();
        ::memcpy(allocateBuffer->_buffer, byteStream, byteCount);
        allocateBuffer->_wsaBuffer.buf = (char*)allocateBuffer->_buffer;
        allocateBuffer->_wsaBuffer.len = (unsigned long)byteCount;
        allocateBuffer->_sentSize = 0;

        {
            WRITE_LOCK(_bufferListMutex);
            _bufferListQueue.push(allocateBuffer);
            isAdd = true;

            if (_curSendingBufferList || 1 < _bufferListQueue.size())
                return { isAdd, isAsyncSend };

            _context->_sendBuffer = _curSendingBufferList = allocateBuffer;
        }
        isAsyncSend = true;

        return { isAdd, isAsyncSend };
    }

    std::pair<size_t, bool> SessionSendBuffer::UpdateSentSizeAndSetNextSendBuffer(uint32_t sentSize)
    {
        size_t resultSendSize{ 0 };
        if (sentSize == 0)
            return { resultSendSize, false };

        WRITE_LOCK(_bufferListMutex);
        _curSendingBufferList->_sentSize += sentSize;
        resultSendSize = _curSendingBufferList->_sentSize;

        if (_context->_sendBuffer->_wsaBuffer.len == sentSize)
        {
            _context->_sendBuffer = _curSendingBufferList = nullptr;
            _bufferListQueue.pop();
            if (!_bufferListQueue.empty())
            {
                _context->_sendBuffer = _curSendingBufferList = _bufferListQueue.front();
                return { resultSendSize, true };
            }
        }

        return { resultSendSize, false };
    }

    AsyncSendContext* SessionSendBuffer::GetContext() const
    {
        return _context.get();
    }
}