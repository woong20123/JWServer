#include "Server.h"
#include <iostream>

int main(int argc, char* argv[])
{
	jw::Server::GetInstance().Initialize(L"SampleServer");
	if (!jw::Server::GetInstance().Start(argc, argv))
	{
		// Logger가 초기화 되지 않을 수 있기에 wcerr로 출력합니다. 
		std::wcerr << L"Server Start Error" << std::endl;
		return 0;
	}

	int a;
	std::cin >> a;

	return 0;
}