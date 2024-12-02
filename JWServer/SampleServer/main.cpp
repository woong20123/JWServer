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
		// Logger�� �ʱ�ȭ ���� ���� �� �ֱ⿡ wcerr�� ����մϴ�. 
		std::wcerr << L"Server Start Error" << std::endl;
		return 0;
	}

	return 0;
}