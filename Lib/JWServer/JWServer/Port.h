#pragma once
#ifndef __JW_PORT_H__
#define __JW_PORT_H__

#include <memory>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <array>
#include <queue>
#include <shared_mutex>

namespace jw
{
    class Listener;
    class Session;

    struct PortInfo
    {
        uint32_t id{ 0 };
        uint16_t portNumber{ 0 };
        HANDLE iocpHandle{ nullptr };
        size_t sesionMaxCount{ 0 };
    };

    class Port
    {
    public:
        static constexpr size_t MAX_SESSION_COUNT = 5000;
        using SessionIndexCon = std::queue<uint16_t>;


        Port();
        ~Port();

        bool Initialize(const PortInfo& data);
        uint32_t GetId() const;
        std::pair<Session*, uint16_t> MakeSession();

    private:

        size_t getAvailableSessionCount();
        size_t getUsedSessionCount();
        void initializeSessionIndex(const size_t maxSessionCount);

        uint32_t                        _id;
        uint16_t                        _portNumber;
        HANDLE                          _ipcpHandle;
        std::unique_ptr<Listener>       _listerner;
        std::shared_ptr<Session>* _sessionList;
        SessionIndexCon                 _availableSessionIndexCon;     // 사용가능한 세션 인덱스
        size_t                          _sessionMaxCount;
        std::shared_mutex               _sessionMutex;
    };

}
#endif // !__JW_PORT_H__
