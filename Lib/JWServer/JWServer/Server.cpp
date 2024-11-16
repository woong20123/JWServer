#include "Server.h"
#include "Arguments.h"
#include "Logger.h"
#include "LogWorker.h"
#include "LogStream.h"
#include "LogBuffer.h"
#include "LogConsoleStream.h"
#include "LogFileStream.h"
#include "Network.h"
#include "IOWorker.h"
#include "Port.h"
#include <list>

namespace jw
{
    Server::Server() :
        _name{ INVALID_SERVER_NAME },
        _logWorker{ nullptr }
    {}
    Server::~Server() {}

    bool Server::Initialize(const std::wstring& name)
    {
        _name = name;
        _logWorker = std::make_unique<LogWorker>();

        return true;
    }

    bool Server::Start(int argc, char* argv[])
    {
        if (_name == INVALID_SERVER_NAME || _logWorker == nullptr)
        {
            std::wcerr << L"not call Server::Initialize()" << std::endl;
            return false;
        }

        initializeLog();

        ARGUMENT().Initialize(argc, argv);
        ARGUMENT().HandleArgument();

        initializeNetwork();

        LOG_INFO(L"on start, name:{}", _name);

        return true;
    }

    bool Server::initializeLog()
    {
        std::shared_ptr<jw::Logger::PContainer> container = std::make_shared<jw::Logger::PContainer>();
        LOGGER().Initialize(container);
        _logWorker->SetProducerCon(container);

#ifdef _DEBUG
        std::vector<jw::LogType> consoleLogFlags = { jw::LogType::LOG_FATAL, jw::LogType::LOG_ERROR, jw::LogType::LOG_WARN, jw::LogType::LOG_INFO, jw::LogType::LOG_DEBUG };
        std::vector<jw::LogType> fileLogFlags = { jw::LogType::LOG_FATAL, jw::LogType::LOG_ERROR, jw::LogType::LOG_WARN, jw::LogType::LOG_INFO, jw::LogType::LOG_DEBUG };
#else
        std::vector<jw::LogType> consoleLogFlags = { jw::LogType::LOG_FATAL, jw::LogType::LOG_ERROR, jw::LogType::LOG_WARN, jw::LogType::LOG_INFO };
        std::vector<jw::LogType> fileLogFlags = { jw::LogType::LOG_FATAL, jw::LogType::LOG_ERROR, jw::LogType::LOG_WARN, jw::LogType::LOG_INFO };
#endif // DEBUG

        auto consoleStream = std::make_shared<jw::LogConsoleStream>();
        consoleStream->OnLogTypeFlags(consoleLogFlags);

        auto fileStream = std::make_shared<jw::LogFileStream>(L"log\\", L"JWServer");
        fileStream->OnLogTypeFlags(fileLogFlags);

        _logWorker->RegisterLogStream(consoleStream);
        _logWorker->RegisterLogStream(fileStream);
        _logWorker->RunThread();

        LOG_INFO(L"initialize Log Success, name:{}", _name);
        return true;
    }

    bool Server::initializeNetwork()
    {
        uint16_t workerThreadCount{ 4 };
        NETWORK().Initialize(workerThreadCount);

        Port::InitData data;
        data.id = CLIENT_PORT_ID;
        data.portNumber = 13211;
        data.iocpHandle = NETWORK().GetIOCPHandle();
        data.sesionMaxCount = 5000;

        std::shared_ptr<Port> basePort = std::make_shared<Port>();
        basePort->Initialize(data);
        NETWORK().RegistPort(CLIENT_PORT_ID, basePort);

        LOG_INFO(L"initialize Network Success, name:{}, workerThreadCount:{}", _name, workerThreadCount);

        return true;
    }
}
