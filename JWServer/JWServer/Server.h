#pragma once

#ifndef __JW_SERVER_H__
#define __JW_SERVER_H__
#include <string>
#include <span>
#include <memory>
#include "Logger.h"
#include "ProducerContainer.hpp"

namespace jw
{
    class Port;
    struct PortInfo;
    class LogWorker;
    class LogStream;
    class Timer;

    enum ServerEventID
    {
        SERVER_EVENT_ID_NONE,
        SERVER_EVENT_ID_NOTIFY,
        SERVER_EVENT_ID_MAX,
    };

    enum class ServerState : uint16_t
    {
        SERVER_STATE_NONE = 0,
        SERVER_STATE_INIT,
        SERVER_STATE_STARTING,
        SERVER_STATE_ON_SERVER,
        SERVER_STATE_CLOSING,
        SERVER_STATE_CLOSED,
        SERVER_STATE_MAX
    };

    constexpr const wchar_t* ServerStateStr[(size_t)ServerState::SERVER_STATE_MAX] = {
        L"NONE",
        L"INIT",
        L"STATING",
        L"ON SERVER",
        L"CLOSING",
        L"CLOSED",
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

        static constexpr int32_t   NONE_PORT_ID = 0;
        static constexpr int32_t   CLIENT_PORT_ID = 1;
        static constexpr int32_t   INTERNAL_PORT_ID = 2;

        static constexpr int32_t   DEFAULT_TIMER_LOGIC_TICK_INTERVAL_MILLISECOND = 100;
        // 관리되는 최대 타이머 틱
        // 등록하는 타이머의 틱이 해당 값 보다 크다면 라스트 틱에 타이머를 등록한 후 틱을 감소 시킵니다. 
        // 다음의 작업을 반복해서 동작할 수 있도록 구성합니다.         
        static constexpr int32_t   SERVER_CLOSE_WAIT_SECOND = 10;

        using ServerEventProducerCon = ProducerContainer<std::shared_ptr<ServerEvent>>;
        using TimerList = std::list<Timer*>;

        Server();
        virtual ~Server();

        bool Initialize(const std::wstring& name);
        bool Start(int argc, char* argv[]);
        void SendServerEvent(std::shared_ptr<ServerEvent>& eventObj);

        // 종료 시그널을 보냅니다.
        void Stop();

        ServerState GetState() { return _state; }

        std::wstring_view GetName() { return _name; }

        static const wchar_t* ServerStateToStr(ServerState state);

    protected:

        // 서버 구동시 사용자가 설정할 Log 작업을 등록 합니다. 
        // - LogStream 등록
        virtual bool onStartLog() = 0;

        virtual bool onStartConfig() = 0;

        // 서버 구동시 사용자가 설정할 Network 작업을 등록 합니다. 
        // - Port 등록 
        // - SessionHandler 등록
        // - PacketHandler 등록 
        virtual bool onStartingNetwork() = 0;
        virtual bool onStartedNetwork() = 0;

        virtual bool onStartingTimer() = 0;

        virtual bool onInitialize() = 0;

        // 런타임에 서버에 전달되는 이벤트 처리 동작을 추가 합니다. 
        virtual bool onHandleEvent(const std::shared_ptr<ServerEvent>& event) = 0;

        // 서버 종료시에 진행할 작업을 등록합니다. 
        virtual void onClosedServer() = 0;

        void registConsoleLogStream(const std::span<LogType> logFlags);
        void registFileLogStream(const std::span<LogType> logFlags);
        void registLogStream(const std::shared_ptr<LogStream>& logStream);

        void setNetworkWorkerThread(const uint16_t);
        void setTimerTickIntervalMilliSecond(const int32_t intervalMilliSecond);
        void reigstPort(const PortInfo& portInfo);

        void setState(ServerState state);
    private:
        bool startLog();
        bool setArgument(int argc, char* argv[]);
        bool setConfig();
        bool startNetwork();
        void startTimer();
        void waitEvent();
        void handleEvent(const std::list<std::shared_ptr<ServerEvent>>& eventObjs);
        void closeServer();

        std::wstring                            _name;
        std::unique_ptr<LogWorker>              _logWorker;
        int32_t                                 _tickIntervalMilliSecond;
        uint16_t                                _workerThreadCount;
        std::unique_ptr<ServerEventProducerCon> _serverEventContainer;
        std::atomic<ServerState>                _state;
    };
}

#endif // !__JW_SERVER_H__ 

