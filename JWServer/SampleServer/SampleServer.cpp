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
    SampleServer::~SampleServer()
    {}

    bool SampleServer::onStartingLog()
    {
        setLogWaitMilliseconds(static_cast<uint32_t>(_config->GetLoggerTickIntervalMilliSecond()));
        return true;
    }

    bool SampleServer::onStartedLog()
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

    bool SampleServer::onStartConfig()
    {
        // config�� �����մϴ�. 
        // config�� �����ϴ� ����� Config.h, Config.cpp�� �����ϼ���.         
        std::locale::global(std::locale("ko_KR.UTF-8"));

        _config->Initialize(std::make_shared<JsonConfigParser>());
        const auto& configPath = std::format(L"./{}_config.json", ARGUMENT().getProcessName());

        // config�� ������ ���� �մϴ�. 
        _config->RegisterConfigDefinition(SampleServerConfig::SERVER_PORT, L"13211");
        _config->RegisterConfigDefinition(SampleServerConfig::WORKER_THREAD, L"0");
        _config->RegisterConfigDefinition(SampleServerConfig::MAX_CLIENT_SESSION_COUNT, L"5000");
        _config->RegisterConfigDefinition(SampleServerConfig::TIMER_TICK_INTERVAL_MSEC, L"100");
        _config->RegisterConfigDefinition(SampleServerConfig::LOGGER_TICK_INTERVAL_MSEC, L"100");
        _config->RegisterConfigDefinition(SampleServerConfig::SESSION_RECV_CHECK_TIME_SECOND, L"300");
        _config->RegisterConfigDefinition(SampleServerConfig::BAD_IP_BLOCK_ENABLE, Config::BOOL_FALSE);
        _config->RegisterConfigDefinition(SampleServerConfig::BAD_IP_BLOCK_SANCTION_TIME_SECOND, L"3600");
        _config->RegisterConfigDefinition(SampleServerConfig::BAD_IP_BLOCK_TRIGGERING_SESSION_COUNT, L"5000");
        _config->RegisterConfigDefinition(SampleServerConfig::BAD_IP_BLOCK_THRESHOLD_CHECKED_COUNT, L"1");


        if (!_config->Load(configPath))
        {
            //_config->Dump(configPath);
            LOG_ERROR(L"failed to load config, path:{}", configPath.c_str());
            return false;
        }

        return true;
    }


    bool SampleServer::onStartingNetwork()
    {
        // ��Ŀ ������ ����
        setNetworkWorkerThread(_config->GetWorkerThreadCount());
        return true;
    }

    bool SampleServer::onStartedNetwork()
    {
        // GameSessionHandler �� PacketHandler ���� 
        std::shared_ptr<SessionHandler> gameSessionHandler = std::make_shared<GameSessionHandler>();;
        std::shared_ptr<PacketHandler> gamePacketHandler = std::make_shared<GamePacketHandler>();
        GAME_PACKET_HANDLE_FUNC_LIST().GetInstance().Initialize(gamePacketHandler);
        gameSessionHandler->Initialize(gamePacketHandler);

        // Ŭ���̾�Ʈ�� �����ϴ� ��Ʈ ���� �Է�
        PortInfo clientPort;
        clientPort._id = CLIENT_PORT_ID;
        clientPort._portNumber = _config->GetServerPort();
        clientPort._iocpHandle = NETWORK().GetIOCPHandle();
        clientPort._sesionMaxCount = _config->GetMaxClientSessionCount();
        clientPort._sessionHandler = gameSessionHandler;
        clientPort._packetBufferHandler = std::make_shared<TrustedPacketBufferHandler>();
        clientPort._recvCheckTimeSecond = _config->GetSessionRecvCheckTimeSecond();

        reigstPort(clientPort);

        Network::BadIpBlockOption badIpBlockOption;
        badIpBlockOption._isRun = _config->GetBadIpBlockEnable();
        badIpBlockOption._sanctionsTimeSecond = _config->GetBadIpBlockSanctionTimeSecond();
        badIpBlockOption._triggeringSessionCount = _config->GetBadIpBlockTriggeringSessionCount();
        badIpBlockOption._thresholdCheckedCount = _config->GetBadIpBlockThresholdCheckedCount();
        NETWORK().SetBadIpBlockOption(badIpBlockOption);

        return true;
    }

    bool SampleServer::onStartingTimer()
    {
        setTimerTickIntervalMilliSecond(_config->GetTimerTickIntervalMilliSecond());

        return true;
    }

    bool SampleServer::onStartedTimer()
    {
        return true;
    }

    bool SampleServer::onInitialize()
    {
        _world->Initialize(World::USER_LIST_MAX_SIZE);

        LOG_INFO(L"initialize server, name:{}", GetName().data());
        return true;
    }

    bool SampleServer::onHandleEvent(const std::shared_ptr<ServerEvent>& eventObj)
    {
        LOG_INFO(L"recved server event, id:{}", eventObj->_id);
        return true;
    }

    void SampleServer::onClosedServer()
    {
        LOG_INFO(L"closed server, name:{}", GetName().data());
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