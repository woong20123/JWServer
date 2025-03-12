#include "SampleServer.h"
#include "Logger.h"
#include "Port.h"
#include "Network.h"
#include "Serializer.h"
#include "SerializerManager.h"
#include "TimerLauncher.h"
#include "GameSessionHandler.h"
#include "GamePacketHandler.h"
#include "GamePacketHandleFuncList.h"
#include "PacketBufferHandler.h"
#include "RoomManager.h"

namespace jw
{
    SampleServer::SampleServer()
    {
        _world = std::make_unique<World>();
        _roomManager = std::make_unique<RoomManager>();
    }
    SampleServer::~SampleServer()
    {}

    bool SampleServer::onStartLog()
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

    bool SampleServer::onStartNetwork()
    {
        // 워커 스레드 설정
        setNetworkWorkerThread(Network::DEFAULT_WORKER_THREAD_COUNT);

        // GameSessionHandler 및 PacketHandler 설정 
        std::shared_ptr<SessionHandler> gameSessionHandler = std::make_shared<GameSessionHandler>();;
        std::shared_ptr<PacketHandler> gamePacketHandler = std::make_shared<GamePacketHandler>();
        GAME_PACKET_HANDLE_FUNC_LIST().GetInstance().Initialize(gamePacketHandler);
        gameSessionHandler->Initialize(gamePacketHandler);

        // 클라이언트와 연동하는 포트 정보 입력
        PortInfo clientPort;
        clientPort._id = CLIENT_PORT_ID;
        clientPort._portNumber = 13211;
        clientPort._iocpHandle = NETWORK().GetIOCPHandle();
        clientPort._sesionMaxCount = 5000;
        clientPort._sessionHandler = gameSessionHandler;
        clientPort._packetBufferHandler = std::make_shared<TrustedPacketBufferHandler>();

        reigstPort(clientPort);

        return true;
    }

    bool SampleServer::onStartTimer()
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
}