#include "SampleServer.h"
#include <memory.h>
#include <iostream>
#include <csignal>
#include <windows.h>
#include "TypeDefinition.h"

BOOL WINAPI ConsoleHandler(DWORD signal)
{
	if (signal == CTRL_C_EVENT || signal == CTRL_BREAK_EVENT || signal == CTRL_CLOSE_EVENT)
	{
		LOG_INFO(L"Console signal {} received, shutting down server...", signal);
		SAMPLE_SERVER().SendStopSignal();
		return TRUE;
	}
	return FALSE;
}

int main(int argc, char* argv[])
{
	if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE))
	{
		std::cerr << "Failed to set console control handler." << std::endl;
		return 1;
	}

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