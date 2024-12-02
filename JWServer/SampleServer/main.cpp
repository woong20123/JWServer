#include "SampleServer.h"
#include <memory.h>
#include <iostream>
#include "proto\code\Test.pb.h"

int main(int argc, char* argv[])
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	SAMPLE_SERVER().Initialize(L"SampleServer");
	if (!SAMPLE_SERVER().Start(argc, argv))
	{
		// Logger가 초기화 되지 않을 수 있기에 wcerr로 출력합니다. 
		std::wcerr << L"Server Start Error" << std::endl;
		return 0;
	}

	return 0;
}