#include "SampleServer.h"
#include <memory.h>
#include <iostream>
#include "proto\code\Test.pb.h"
#include "TypeDefinition.h"

int main(int argc, char* argv[])
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	/*jw::ping pingMessage;
	pingMessage.set_version(1123);
	char* buffer = new char[pingMessage.ByteSizeLong()];

	pingMessage.SerializeToArray(buffer, pingMessage.ByteSizeLong());

	jw::ping pingMessage2;
	pingMessage2.ParsePartialFromArray(buffer, pingMessage.ByteSizeLong());*/


	SAMPLE_SERVER().Initialize(L"SampleServer");
	if (!SAMPLE_SERVER().Start(argc, argv))
	{
		// Logger가 초기화 되지 않을 수 있기에 wcerr로 출력합니다. 
		std::wcerr << L"Server Start Error" << std::endl;
		return 0;
	}

	return 0;
}