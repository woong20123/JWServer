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
    class ThreadChecker;

    enum ServerEventID
    {
        SERVER_EVENT_ID_NONE,
        SERVER_EVENT_ID_NOTIFY,
        SERVER_EVENT_ID_MAX,
    };

    enum class ServerState : uint16_t
    {
        SERVER_STATE_NONE = 0,
        SERVER_STATE_INITIALIZING,
        SERVER_STATE_INITIALIZED,
        SERVER_STATE_STARTED_SERVER,
        SERVER_STATE_STOPING,
        SERVER_STATE_STOPED,
        SERVER_STATE_CLOSING,
        SERVER_STATE_CLOSED,
        SERVER_STATE_MAX
    };

    constexpr const wchar_t* ServerStateStr[(size_t)(ServerState::SERVER_STATE_MAX)] = {
        L"NONE",
        L"INITIALIZING",
        L"INITIALIZED",
        L"STARTED_SERVER",
        L"STATE_STOPING",
        L"STATE_STOPED",
        L"CLOSING",
        L"CLOSED"
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

        static constexpr uint32_t DEFAULT_SERVER_EVENT_TIME_TICK_MSEC = 30000;
        static constexpr int64_t   DEFAULT_TIMER_LOGIC_TICK_INTERVAL_MSEC = 100;
        static constexpr int32_t   DEFAULT_LOG_WAIT_TICK_MSEC = 100;
        // 관리되는 최대 타이머 틱
        // 등록하는 타이머의 틱이 해당 값 보다 크다면 라스트 틱에 타이머를 등록한 후 틱을 감소 시킵니다. 
        // 다음의 작업을 반복해서 동작할 수 있도록 구성합니다.         
        static constexpr int32_t   SERVER_CLOSE_WAIT_SECOND = 5;

        using ServerEventProducerCon = ProducerContainer<std::shared_ptr<ServerEvent>>;
        using TimerList = std::list<Timer*>;

        Server();
        virtual ~Server();

        bool Initialize(const std::wstring& name, int argc, char* argv[]);
        void Start(int argc, char* argv[]);

        // 서버에 이벤트를 전송합니다. 
        void SendServerEvent(std::shared_ptr<ServerEvent>& eventObj);

        ServerState GetState() { return _state; }

        std::wstring_view GetName() { return _name; }

        static const wchar_t* ServerStateToStr(ServerState state);

        void CloseServer();

    protected:
        void registConsoleLogStream(const std::span<LogType> logFlags);
        void registFileLogStream(const std::span<LogType> logFlags);
        void registLogStream(const std::shared_ptr<LogStream>& logStream);

        void setNetworkWorkerThread(const uint16_t);
        void setTimerTickIntervalMilliSecond(const int64_t timerTickIntervalMSecond);
        void setLogWaitMilliseconds(const uint32_t waitMSecond);
        void reigstPort(const PortInfo& portInfo);

        void setState(ServerState state);
    private:

        // 서버 초기화시 사용자가 설정할 작업을 등록 합니다.
        virtual bool onInitializing() { return true; }
        virtual bool onInitialized() { return true; }
        virtual bool onStartedServer() { return true; }
        virtual void onClosingServer() { }
        virtual void onClosedServer() { }

        // 서버 구동시 사용자가 설정할 Log 작업을 등록 합니다. 
        // - LogStream 등록
        virtual bool onInitializingLog() { return true; }
        virtual bool onInitializedLog() { return true; }

        // config 설정 작업을 등록합니다. 
        virtual bool onInitializeConfig() { return true; }

        // 서버 구동시 사용자가 설정할 Network 작업을 등록 합니다. 
        virtual bool onInitializingNetwork() = 0;
        virtual bool onInitializedNetwork() = 0;

        virtual bool onInitializingTimer() = 0;
        virtual bool onInitializedTimer() = 0;

        virtual bool onInitializedThreadManager() { return true; };

        virtual bool onValidateChecker() = 0;

        // 런타임에 서버에 전달되는 이벤트 처리 동작을 추가 합니다. 
        virtual bool onHandleEvent(const std::shared_ptr<ServerEvent>& event) = 0;



        // initialize
        bool initializeAndRunLog();
        bool registArgument(int argc, char* argv[]);
        bool initializeConfig();
        bool initializeNetwork();
        bool initializeTimer();
        bool initializeThreadManager();
        bool validateChecker();

        // start
        void startNetwork();
        void startTimer();

        void waitEvent();
        void handleEvent(const std::list<std::shared_ptr<ServerEvent>>& eventObjs);
        void Stop();

        std::wstring                            _name;
        std::unique_ptr<LogWorker>              _logWorker;
        int64_t                                 _timerTickIntervalMSec;
        uint16_t                                _workerThreadCount;
        uint32_t                                _logWaitMSec;
        std::unique_ptr<ServerEventProducerCon> _serverEventContainer;
        std::atomic<ServerState>                _state;
        std::unique_ptr<ThreadChecker>          _threadChecker;
    };
}

#endif // !__JW_SERVER_H__ 

