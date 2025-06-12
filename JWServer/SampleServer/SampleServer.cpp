#include "SampleServer.h"
#include "Logger.h"
#include "Port.h"
#include "Arguments.h"
#include "SampleServerConfig.h"
#include "Network.h"
#include "Serializer.h"
#include "SerializerManager.h"
#include "TimerLauncher.h"
#include "GameSessionHandler.h"
#include "GamePacketHandler.h"
#include "GamePacketHandleFuncList.h"
#include "PacketBufferHandler.h"
#include "RoomManager.h"
#include <format>

namespace jw
{
    SampleServer::SampleServer()
    {
        _world = std::make_unique<World>();
        _roomManager = std::make_unique<RoomManager>();
        _config = std::make_unique<SampleServerConfig>();
    }
    SampleServer::~SampleServer() = default;

    bool SampleServer::onInitializingLog()
    {
        setLogWaitMilliseconds(static_cast<uint32_t>(_config->GetLoggerTickIntervalMilliSecond()));
        return true;
    }

    bool SampleServer::onInitializedLog()
    {
#ifdef _DEBUG
        std::vector<jw::LogType> consoleLogFlags = { jw::LogType::LOG_FATAL, jw::LogType::LOG_ERROR, jw::LogType::LOG_WARN, jw::LogType::LOG_INFO, jw::LogType::LOG_DEBUG };
        std::vector<jw::LogType> fileLogFlags = { jw::LogType::LOG_FATAL, jw::LogType::LOG_ERROR, jw::LogType::LOG_WARN, jw::LogType::LOG_INFO, jw::LogType::LOG_DEBUG };
#else
        std::vector<jw::LogType> consoleLogFlags = { jw::LogType::LOG_FATAL, jw::LogType::LOG_ERROR, jw::LogType::LOG_WARN, jw::LogType::LOG_INFO };
        std::vector<jw::LogType> fileLogFlags = { jw::LogType::LOG_FATAL, jw::LogType::LOG_ERROR, jw::LogType::LOG_WARN, jw::LogType::LOG_INFO };
#endif // DEBUG

        registConsoleLogStream(consoleLogFlags);
        registFileLogStream(fileLogFlags);

        return true;
    }

    bool SampleServer::onInitializeConfig()
    {
        // config를 설정합니다. 
        // config를 설정하는 방법은 Config.h, Config.cpp를 참고하세요.         
        std::locale::global(std::locale("ko_KR.UTF-8"));

        _config->Initialize(ConfigParser::CreateParser(ConfigParser::ParserType::JSON));
        const auto& configPath = std::format(L"./{}_config.json", GetArguments().getProcessName());

        if (!_config->Load(configPath))
        {
            //_config->Dump(configPath);
            LOG_ERROR(L"failed to load config, path:{}", configPath.c_str());
            return false;
        }

        return true;
    }


    bool SampleServer::onInitializingNetwork()
    {
        // 워커 스레드 설정
        setNetworkWorkerThread(_config->GetWorkerThreadCount());
        return true;
    }

    bool SampleServer::onInitializedNetwork()
    {
        // GameSessionHandler 및 PacketHandler 설정 
        std::shared_ptr<SessionHandler> gameSessionHandler = std::make_shared<GameSessionHandler>();;
        std::shared_ptr<PacketHandler> gamePacketHandler = std::make_shared<GamePacketHandler>();
        GAME_PACKET_HANDLE_FUNC_LIST().GetInstance().Initialize(gamePacketHandler);
        gameSessionHandler->Initialize(gamePacketHandler);

        // 클라이언트와 연동하는 포트 정보 입력
        PortInfo clientPort;
        clientPort._id = CLIENT_PORT_ID;
        clientPort._portNumber = _config->GetServerPort();
        clientPort._iocpHandle = GetNetwork().GetIOCPHandle();
        clientPort._sesionMaxCount = _config->GetMaxClientSessionCount();
        clientPort._sessionHandler = gameSessionHandler;
        clientPort._packetBufferHandler = std::make_shared<TrustedPacketBufferHandler>();
        clientPort._recvCheckTimeSecond = _config->GetSessionRecvCheckTimeSecond();

        reigstPort(clientPort);

        // BadIPBlock 설정
        Network::BadIpBlockOption badIpBlockOption;
        badIpBlockOption._isRun = _config->GetBadIpBlockEnable();
        badIpBlockOption._sanctionsTimeSecond = _config->GetBadIpBlockSanctionTimeSecond();
        badIpBlockOption._triggeringSessionCount = _config->GetBadIpBlockTriggeringSessionCount();
        badIpBlockOption._thresholdCheckedCount = _config->GetBadIpBlockThresholdCheckedCount();
        GetNetwork().SetBadIpBlockOption(badIpBlockOption);

        return true;
    }

    bool SampleServer::onInitializedTimer()
    {
        return true;
    }

    bool SampleServer::onInitializingTimer()
    {
        setTimerTickIntervalMilliSecond(_config->GetTimerTickIntervalMilliSecond());

        return true;
    }

    bool SampleServer::onValidateChecker()
    {
        if (!_config->IsValidate())
        {
            return false;
        }

        return true;
    }

    bool SampleServer::onInitializing()
    {
        LOG_INFO(L"initializing server, name:{}", GetName().data());
        return true;
    }

    bool SampleServer::onInitialized()
    {
        LOG_INFO(L"initialized server, name:{}", GetName().data());
        return true;
    }

    bool SampleServer::onStartedServer()
    {
        _world->Initialize(World::USER_LIST_MAX_SIZE);

        LOG_INFO(L"started server, name:{}", GetName().data());
        return true;
    }

    void SampleServer::onClosingServer()
    {
        LOG_INFO(L"closing server, name:{}", GetName().data());
    }

    void SampleServer::onClosedServer()
    {
        LOG_INFO(L"closed server, name:{}", GetName().data());
    }

    bool SampleServer::onHandleEvent(const std::shared_ptr<ServerEvent>& eventObj)
    {
        LOG_INFO(L"recved server event, id:{}", eventObj->_id);
        return true;
    }

    World* SampleServer::GetWorld()
    {
        return _world.get();
    }

    RoomManager* SampleServer::GetRoomManager()
    {
        return _roomManager.get();
    }

    SampleServerConfig* SampleServer::GetConfig()
    {
        return _config.get();
    }
}