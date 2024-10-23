#include "Arguments.h"
#include "Logger.h"
#include "LogWorker.h"
#include <memory>

int main(int argc, char* argv[])
{
	std::shared_ptr<jw::Logger::ProducerObj> producer = std::make_shared<jw::Logger::ProducerObj>();
	jw::Logger::GetInstance().Initialize(producer);
	jw::LogWorker logWorker{ producer };
	logWorker.RunThread();

	jw::ARGUMENT().Initialize(argc, argv);
	jw::ARGUMENT().HandleArgument();

	int a;
	std::cin >> a;

	return 0;
}	