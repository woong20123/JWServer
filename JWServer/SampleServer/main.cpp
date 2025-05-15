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
		// Logger�� �ʱ�ȭ ���� ���� �� �ֱ⿡ wcerr�� ����մϴ�. 
		std::cerr << std::format("Server Initialize Error\n");
	}
	

	return 0;
}