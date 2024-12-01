#pragma once
#ifndef __JW_SESSION_BUFFER_H__
#define __JW_SESSION_BUFFER_H__
#include <cstdint>
#include <memory>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <queue>
#include <shared_mutex>

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
        size_t  _sentSize;
        WSABUF  _wsaBuffer;
        char    _buffer[BUFFER_MAX_SIZE];
    };

    enum AddReason
    {

    };

    class SessionSendBuffer
    {
    public:
        using bufferType = char;
        static constexpr size_t BUFFER_LIST_SIZE = 32;
        SessionSendBuffer();
        ~SessionSendBuffer();

        // 전달 받은 바이트를 SendBufferList로 변환하여 buffer 등록합니다. 
        // 등록된 이전 buffer가 없다면 asyncSend를 호출해서 비동기 연산을 시작합니다. 
        // 이미 pending 중이라면 buffer만 등록합니다. 진행 중인 asyncSend의 콜백에 의해서 sending됩니다.
        // returns : 첫번째 bool은 Add 성공 여부, 두번째 bool은 asyncSend 호출 여부
        std::pair<bool, bool> Add(const void* byteStream, const size_t byteCount);

        // 전송된 sentSize를 업데이트합니다. 
        // pending중인 sendbuffer을 모두 보냈다면 다음 sendbuffer가 있는지 확인 후 등록합니다. 
        // returns : 다음 sendBuffer가 있는지 여부
        std::pair<size_t, bool> UpdateSentSizeAndSetNextSendBuffer(uint32_t sentSize);

        AsyncSendContext* GetContext() const;
    private:

        std::queue<std::shared_ptr<SendBufferList>> _bufferListQueue;
        std::shared_ptr<SendBufferList>         _curSendingBufferList;
        int16_t                                 _allocateBufferListSendingIdx;
        std::unique_ptr<AsyncSendContext>       _context;
        std::shared_mutex                       _bufferListMutex;
    };
}
#endif

