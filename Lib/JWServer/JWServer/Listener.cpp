#include "Listener.h"
#include "Logger.h"

namespace jw
{
	struct Listener::Impl
	{
		LPFN_ACCEPTEX	acceptexFunc;
		uint16_t		port;
		SOCKET			listenSocket;
	};
	
	Listener::Listener() : _pImpl{std::make_unique<Impl>()}
	{}
	Listener::~Listener()
	{}

	void Listener::Initialize(const LPFN_ACCEPTEX acceptexFunc, uint16_t port)
	{
		if(!acceptexFunc)
		{
			LOG_FETAL_STRING(L"acceptexFunc is nullptr");
		}

		_pImpl->acceptexFunc = acceptexFunc;
		_pImpl->port = port;
	}
}
