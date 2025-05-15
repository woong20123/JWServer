#include "SampleServer.h"
#include <memory.h>
#include <iostream>
#include "TypeDefinition.h"

int main(int argc, char* argv[])
{
	if (SAMPLE_SERVER().Initialize(L"SampleServer", argc, argv))
	{
		SAMPLE_SERVER().Start(argc, argv);
	}
	else
	{
		SAMPLE_SERVER().CloseServer();
		// Logger가 초기화 되지 않을 수 있기에 wcerr로 출력합니다. 
		std::cerr << std::format("Server Initialize Error\n");
	}
	

	return 0;
}