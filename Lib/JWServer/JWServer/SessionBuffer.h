#pragma once
#ifndef __JW_SESSION_BUFFER_H__
#define __JW_SESSION_BUFFER_H__
#include <cstdint>
#include <memory>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>

namespace jw
{
    struct AsyncRecvContext;
    struct AsyncSendContext;

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

    struct SendBufferList
    {
        static constexpr size_t BUFFER_MAX_SIZE = 4000;
        uint16_t _index;
        size_t  _sendSize;
        WSABUF  _wsaBuffer;
        char    _buffer[BUFFER_MAX_SIZE];
    };

    class SessionSendBuffer
    {
    public:
        using bufferType = char;
        static constexpr size_t BUFFER_LIST_SIZE = 32;
        SessionSendBuffer();
        ~SessionSendBuffer();

        bool Add(const void* byteStream, const size_t byteCount);
        AsyncSendContext* GetContext() const;
    private:
        uint16_t    getAllocateIndex();


        SendBufferList                          _bufferList[BUFFER_LIST_SIZE];
        SendBufferList* _curSendingBufferList;
        int16_t                                 _allocateBufferListSendingIdx;
        std::unique_ptr<AsyncSendContext>       _context;
    };
}
#endif

