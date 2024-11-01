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
	logWorker.RegisterLogStream(std::make_shared<jw::LogConsoleStream>());
	logWorker.RegisterLogStream(std::make_shared<jw::LogFileStream>(L"log\\", L"JWServer"));
	logWorker.RunThread();

	LOG_FETAL_STRING(L"테스트 기록용");

	ARGUMENT().Initialize(argc, argv);
	ARGUMENT().HandleArgument();

	NETWORK().Initialize();


	int a;
	std::cin >> a;

	return 0;
}	