#include "SampleServer.h"
#include "Logger.h"

#include "Port.h"
#include "Network.h"

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
        PortInfo baseClientPort;
        baseClientPort.id = CLIENT_PORT_ID;
        baseClientPort.portNumber = 13211;
        baseClientPort.iocpHandle = NETWORK().GetIOCPHandle();
        baseClientPort.sesionMaxCount = 5000;
        reigstPort(baseClientPort);

        return true;
    }
}