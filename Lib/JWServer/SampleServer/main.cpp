#include "SampleServer.h"
#include <memory.h>
#include <iostream>

int main(int argc, char* argv[])
{
	SAMPLE_SERVER().Initialize(L"SampleServer");
	if (!SAMPLE_SERVER().Start(argc, argv))
	{
		// Logger�� �ʱ�ȭ ���� ���� �� �ֱ⿡ wcerr�� ����մϴ�. 
		std::wcerr << L"Server Start Error" << std::endl;
		return 0;
	}

	int a;
	std::cin >> a;

	return 0;
}