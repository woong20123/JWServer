#include "SampleServer.h"
#include "Logger.h"
#include "Port.h"
#include "Network.h"
#include "GameSessionHandler.h"

namespace jw
{
    SampleServer::SampleServer()
    {}
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

        // 클라이언트와 연동하는 포트
        PortInfo clientPort;
        clientPort._id = CLIENT_PORT_ID;
        clientPort._portNumber = 13211;
        clientPort._iocpHandle = NETWORK().GetIOCPHandle();
        clientPort._sesionMaxCount = 5000;
        clientPort._sessionHandler = std::make_shared<GameSessionHandler>();
        reigstPort(clientPort);

        // 내부서버와 연동하는 포트
        PortInfo internalPort;
        internalPort._id = INTERNAL_PORT_ID;
        internalPort._portNumber = 23211;
        internalPort._iocpHandle = NETWORK().GetIOCPHandle();
        internalPort._sesionMaxCount = 1000;
        internalPort._sessionHandler = std::make_shared<GameSessionHandler>();
        std::shared_ptr<SessionHandler> sessionHandler = std::make_shared<GameSessionHandler>();
        reigstPort(internalPort);

        return true;
    }

    bool SampleServer::onHandleEvent(const std::shared_ptr<ServerEvent>& eventObj)
    {
        LOG_INFO(L"recved server event, id:{}", eventObj->_id);
        return true;
    }

    void SampleServer::onClosedServer()
    {
        LOG_INFO(L"closed server");
    }
}