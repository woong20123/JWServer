#include "Server.h"
#include "Arguments.h"
#include "Logger.h"
#include "LogWorker.h"
#include "LogStream.h"
#include "Network.h"
#include "NetworkHelper.h"
#include "IOWorker.h"
#include "LogConsoleStream.h"
#include "LogFileStream.h"
#include "Port.h"
#include "TimerLauncher.h"
#include <list>
#include <chrono>
#include "Config.h"
#include "DumpMaker.h"

namespace jw
{
    Server::Server() :
        _name{ INVALID_SERVER_NAME },
        _logWorker{ nullptr },
        _workerThreadCount{ 0 },
        _serverEventContainer{ std::make_unique<ServerEventProducerCon>(DEFAULT_SERVER_EVENT_TIME_TICK_MSEC) },
        _state{ ServerState::SERVER_STATE_NONE },
        _timerTickIntervalMSec{ DEFAULT_TIMER_LOGIC_TICK_INTERVAL_MSEC },
        _logWaitMSec{ DEFAULT_LOG_WAIT_TICK_MSEC }
    {
    }

    Server::~Server() = default;

    bool Server::Initialize(const std::wstring& name, int argc, char* argv[])
    {
        GetDumpMaker().SetApplicationName(name);
        GetDumpMaker().Regist();

        _name = name;

        if (_name == INVALID_SERVER_NAME)
        {
            std::cerr << std::format("not call Server::Initialize()\n");
            return false;
        }

        setState(ServerState::SERVER_STATE_INITIALIZING);

        if (!initializeAndRunLog())
        {
            std::cerr << std::format("fail startLog func\n");
            return false;
        }

        registArgument(argc, argv);

        if (!initializeConfig())
        {
            LOG_ERROR(L"fail setConfig, name:{}", _name);
            return false;
        }

        if (!initializeNetwork())
        {
            LOG_ERROR(L"fail initialize Network, name:{}", _name);
            return false;
        }

        if (!initializeTimer())
        {
            LOG_ERROR(L"fail initialize Timer, name:{}", _name);
            return false;
        }

        if (!validateChecker())
        {
            LOG_ERROR(L"fail validate checker, name:{}", _name);
            return false;
        }

        setState(ServerState::SERVER_STATE_INITIALIZED);

        return true;
    }

    void Server::Start(int argc, char* argv[])
    {
        LOG_INFO(L"on start, name:{}", _name);

        startNetwork();

        startTimer();

        setState(ServerState::SERVER_STATE_STARTED_SERVER);

        waitEvent();

        CloseServer();
    }

    void Server::SendServerEvent(std::shared_ptr<ServerEvent>& eventObj)
    {
        if (!_serverEventContainer)
        {
            LOG_ERROR(L"can not send event because serverEventContainer is null");
            return;
        }
        _serverEventContainer->Push(eventObj);
    }

    const wchar_t* Server::ServerStateToStr(ServerState state)
    {
        return ServerStateStr[static_cast<size_t>(state)];
    }

    bool Server::initializeAndRunLog()
    {
        // logger와 logWorker를 container로 연결 
        if (!onInitializingLog())
        {
            std::cerr << std::format("fail onInitializingLog func\n");
            return false;
        }

        // Logger Container를 Logger와 LogWorker에 연결합니다. 
        std::shared_ptr<Logger::PContainer> container = std::make_shared<Logger::PContainer>(100);
        GetLogger().Initialize(container);
        _logWorker = std::make_unique<LogWorker>(container);

        if (!onInitializedLog())
        {
            std::cerr << std::format("fail onInitializedLog func\n");
            return false;
        }

        _logWorker->RunThread();

        LOG_INFO(L"initialize Log Success, name:{}, registLogStreamCount:{}", _name, _logWorker->getRegistedLogStreamCount());
        return true;
    }

    void Server::registConsoleLogStream(const std::span<LogType> logFlags)
    {
        std::wstring flags;
        for (const auto flag : logFlags)
        {
            flags.append(std::format(L"{},", Logger::LogTypeToString(flag)));
        }

        auto consoleStream = std::make_shared<LogConsoleStream>();
        consoleStream->OnLogTypeFlags(logFlags);
        registLogStream(consoleStream);
        LOG_INFO(L"regist consoleStream, onflags:{}", flags.c_str());
    }

    void Server::registFileLogStream(const std::span<LogType> logFlags)
    {

        constexpr const wchar_t* fileStreamPath = L"log\\";
        constexpr const wchar_t* fileStreamName = L"JWServer";

        std::wstring flags;
        for (const auto flag : logFlags)
        {
            flags.append(std::format(L"{},", Logger::LogTypeToString(flag)));
        }

        auto fileStream = std::make_shared<LogFileStream>(fileStreamPath, fileStreamName);
        fileStream->OnLogTypeFlags(logFlags);
        registLogStream(fileStream);
        LOG_INFO(L"regist fileStream, path:{}, name:{}, onflags:{}", fileStreamPath, fileStreamName, flags.c_str());
    }

    void Server::registLogStream(const std::shared_ptr<LogStream>& logStream)
    {
        _logWorker->RegisterLogStream(logStream);
    }

    void Server::setNetworkWorkerThread(const uint16_t workerThreadCount)
    {
        _workerThreadCount = workerThreadCount;
    }

    void Server::setTimerTickIntervalMilliSecond(const int64_t timerTickIntervalMSec)
    {
        _timerTickIntervalMSec = timerTickIntervalMSec;
    }

    void Server::setLogWaitMilliseconds(const uint32_t waitMSec)
    {
        _logWaitMSec = waitMSec;
    }

    void Server::reigstPort(const PortInfo& portInfo)
    {
        std::shared_ptr<Port> port = std::make_shared<Port>();
        port->Initialize(portInfo);
        GetNetwork().RegistPort(portInfo._id, port);
    }

    void Server::setState(ServerState state)
    {
        if (_state == state)
        {
            LOG_ERROR(L"equal state, state:{}", Server::ServerStateToStr(state));
            return;
        }

        switch (state)
        {
        case ServerState::SERVER_STATE_INITIALIZING:
            onInitializing();
            break;
        case ServerState::SERVER_STATE_INITIALIZED:
            onInitialized();
            break;
        case ServerState::SERVER_STATE_STARTED_SERVER:
            onStartedServer();
            break;
        case ServerState::SERVER_STATE_CLOSING:
            break;
        case ServerState::SERVER_STATE_CLOSED:
            onClosedServer();
            break;
        default:
            break;
        }

        LOG_INFO(L"sucesss, beforeState:{}, afeterState:{}", Server::ServerStateToStr(_state), Server::ServerStateToStr(state));
        _state = state;
    }

    bool Server::registArgument(int argc, char* argv[])
    {
        GetArguments().Initialize(argc, argv);
        GetArguments().HandleArgument();
        return true;
    }

    bool Server::initializeConfig()
    {
        // TODO : config를 설정합니다. 
        if (!onInitializeConfig())
            return false;

        return true;
    }

    bool Server::initializeNetwork()
    {
        if (!onInitializingNetwork())
        {
            LOG_ERROR(L"fail onStartedNetwork, name:{}, workerThreadCount:{}", _name, _workerThreadCount);
            return false;
        }

        if (!GetNetwork().Initialize())
        {
            LOG_ERROR(L"fail startNetwork, name:{}, workerThreadCount:{}", _name, _workerThreadCount);
            return false;
        }

        if (!onInitializedNetwork())
        {
            LOG_ERROR(L"fail onStartedNetwork, name:{}, workerThreadCount:{}", _name, _workerThreadCount);
            return false;
        }

        LOG_INFO(L"initialize Network Success, name:{}, workerThreadCount:{}", _name, _workerThreadCount);

        return true;
    }

    bool Server::initializeTimer()
    {
        if (!onInitializingTimer())
        {
            LOG_ERROR(L"fail onStartingTimer, name:{}, tickIntervalMilliSecond:{}", _name, _timerTickIntervalMSec);
            return false;
        }

        GetTimerLauncher().Initialize(_timerTickIntervalMSec);

        if (!onInitializedTimer())
        {
            LOG_ERROR(L"fail onStartedTimer, name:{}, tickIntervalMilliSecond:{}", _name, _timerTickIntervalMSec);
            return false;
        }

        LOG_INFO(L"initialize startTimer Success, name:{}, tickIntervalMilliSecond:{}", _name, _timerTickIntervalMSec);

        return true;
    }

    bool Server::validateChecker()
    {
        if (!onValidateChecker())
        {
            LOG_ERROR(L"fail onValidateChecker, name:{}", _name);
            return false;
        }

        return true;
    }

    void Server::startNetwork()
    {
        GetNetwork().Start(_workerThreadCount);

        LOG_INFO(L"start Network Success, name:{}, workerThreadCount:{}", _name, _workerThreadCount);
    }
    void Server::startTimer()
    {
        GetTimerLauncher().Run();
        LOG_INFO(L"start Timer Success, name:{}", _name);
    }

    void Server::waitEvent()
    {
        while (true)
        {
            if (_serverEventContainer->IsStop())
            {
                LOG_INFO(L"StopSignal send to Server, name:{}", _name);
                break;
            }

            std::list<std::shared_ptr<ServerEvent>> objects;
            _serverEventContainer->Wait(objects);
            if (!objects.empty())
            {
                handleEvent(objects);
            }

            LOG_DEBUG(L"Server is running, name:{}", _name);
        }

        LOG_INFO(L"Server is closed, name:{}", _name);
    }

    void Server::handleEvent(const std::list<std::shared_ptr<ServerEvent>>& eventObjs)
    {
        for (const auto& eventObj : eventObjs)
        {
            onHandleEvent(eventObj);
        }
    }

    void Server::CloseServer()
    {
        setState(ServerState::SERVER_STATE_CLOSING);
        GetNetwork().Stop();
        GetLogger().Stop();
        GetTimerLauncher().Stop();

        // 서버의 정리작업을 기다립니다. 
        std::this_thread::sleep_for(std::chrono::seconds(SERVER_CLOSE_WAIT_SECOND));

        setState(ServerState::SERVER_STATE_CLOSED);
    }

    void Server::Stop()
    {
        if (_state == ServerState::SERVER_STATE_CLOSING ||
            _state == ServerState::SERVER_STATE_CLOSED)
            return;

        _serverEventContainer->SetStopSignal();
        std::shared_ptr<ServerEvent> evt = std::make_shared<NotifyServerEvent>();
        SendServerEvent(evt);
    }
}
