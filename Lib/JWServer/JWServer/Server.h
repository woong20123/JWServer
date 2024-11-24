#pragma once

#ifndef __JW_SERVER_H__
#define __JW_SERVER_H__
#include <string>
#include <memory>
#include <span>
#include "Logger.h"
#include "ProducerContainer.hpp"

namespace jw
{
    class Port;
    struct PortInfo;
    class LogWorker;
    class LogStream;

    enum ServerEventID
    {
        SERVER_EVENT_ID_NONE,
        SERVER_EVENT_ID_NOTIFY,
        SERVER_EVENT_ID_MAX,
    };

    struct ServerEvent
    {
        ServerEvent(uint16_t id) : _id{ id }
        {}

        uint16_t _id;
    };

    struct NotifyServerEvent : public ServerEvent
    {
        NotifyServerEvent() : ServerEvent(SERVER_EVENT_ID_NOTIFY)
        {}
    };

    class Server
    {
    public:
        static constexpr const wchar_t* INVALID_SERVER_NAME = L"INVALID_SERVER_NAME";

        static constexpr uint32_t   NONE_PORT_ID = 0;
        static constexpr uint32_t   CLIENT_PORT_ID = 1;
        static constexpr uint32_t   INTERNAL_PORT_ID = 2;

        using ServerEventProducerCon = ProducerContainer<std::shared_ptr<ServerEvent>>;

        Server();
        virtual ~Server();

        bool Initialize(const std::wstring& name);
        bool Start(int argc, char* argv[]);
        void SendServerEvent(std::shared_ptr<ServerEvent>& eventObj);
        void Stop();

        std::wstring_view GetName() { return _name; }

    protected:

        // 서버 구동시 사용자가 설정할 Log 작업을 등록 합니다. 
        // - LogStream 등록
        virtual bool onStartLog() = 0;

        // 서버 구동시 사용자가 설정할 Network 작업을 등록 합니다. 
        // - Port 등록 
        // - SessionHandler 등록
        // - PacketHandler 등록 
        virtual bool onStartNetwork() = 0;

        // 런타임에 서버에 전달되는 이벤트 처리 동작을 추가 합니다. 
        virtual bool onHandleEvent(const std::shared_ptr<ServerEvent>& event) = 0;

        // 서버 종료시에 진행할 작업을 등록합니다. 
        virtual void onClosedServer() = 0;

        void registConsoleLogStream(const std::span<LogType> logFlags);
        void registFileLogStream(const std::span<LogType> logFlags);
        void registLogStream(const std::shared_ptr<LogStream>& logStream);

        void setNetworkWorkerThread(uint16_t);
        void reigstPort(const PortInfo& portInfo);

    private:
        bool startLog();
        bool setArgument(int argc, char* argv[]);
        bool startNetwork();
        void waitEvent();
        void handleEvent(const std::list<std::shared_ptr<ServerEvent>>& eventObjs);
        void closeServer();

        std::wstring                            _name;
        std::unique_ptr<LogWorker>              _logWorker;
        uint16_t                                _workerThreadCount;
        std::unique_ptr<ServerEventProducerCon> _serverEventContainer;
    };
}

#endif // !__JW_SERVER_H__ 

