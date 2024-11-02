#include "Arguments.h"
#include "Logger.h"
#include "LogWorker.h"
#include "LogStream.h"
#include "LogBuffer.h"
#include "LogConsoleStream.h"
#include "LogFileStream.h"
#include "Network.h"
#include "StopWatch.h"
#include <memory>
#include <list>

int main(int argc, char* argv[])
{
	std::shared_ptr<jw::Logger::ProducerObj> producer = std::make_shared<jw::Logger::ProducerObj>();
	LOGGER().Initialize(producer);
	jw::LogWorker logWorker{ producer };

#ifdef DEBUG
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

	logWorker.RegisterLogStream(consoleStream);
	logWorker.RegisterLogStream(fileStream);
	logWorker.RunThread();

	LOG_FETAL_STRING(L"테스트 기록용");

	ARGUMENT().Initialize(argc, argv);
	ARGUMENT().HandleArgument();

	NETWORK().Initialize();


	int a;
	std::cin >> a;

	return 0;
}	