#include "Listener.h"
#include "Logger.h"
#include "NetworkHelper.h"
#include "Session.h"

namespace jw
{
	struct Listener::Impl
	{
		Impl() : listenSocket{ INVALID_SOCKET }, port{ 0 }
		{}

		LPFN_ACCEPTEX	acceptexFunc;
		LPFN_GETACCEPTEXSOCKADDRS        getAcceptExSockAddrFunc;
		uint16_t		port;
		SOCKET			listenSocket;
		HANDLE			iocpHandle;
		bool			nagle{ false };
	};

	Listener::Listener() : _pImpl{ std::make_unique<Impl>() }
	{}
	Listener::~Listener()
	{}

	void Listener::Initialize(const LPFN_ACCEPTEX acceptexFunc, const LPFN_GETACCEPTEXSOCKADDRS acceptexSockAddrFunc, uint16_t port, HANDLE iocpHandle)
	{
		if (!acceptexFunc)
		{
			LOG_FETAL(L"acceptexFunc is nullptr");
		}

		_pImpl->acceptexFunc = acceptexFunc;
		_pImpl->getAcceptExSockAddrFunc = acceptexSockAddrFunc;
		_pImpl->port = port;
		_pImpl->iocpHandle = iocpHandle;

		SOCKADDR_IN addr;
		SOCKET s = MakeTCPSocket();
		if (INVALID_SOCKET == s)
		{
			LOG_FETAL(L"initialize socket");
			return;
		}

		::memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.S_un.S_addr = INADDR_ANY;
		addr.sin_port = ::htons(_pImpl->port);

		if (SOCKET_ERROR == ::bind(s, reinterpret_cast<SOCKADDR*>(&addr), sizeof(addr)))
		{
			LOG_FETAL(L"fail socket bind");
			::closesocket(s);
			return;
		}

		// nagle 옵션이 꺼져 있다면 딜레이 없이 동작하도록 설정 합니다. 
		if (!_pImpl->nagle)
		{
			int optval = 1;
			if (SOCKET_ERROR == ::setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (const char*)&optval, sizeof(optval)))
			{
				LOG_FETAL(L"setsocket, TCP_NODELAY set fail");
				::closesocket(s);
				return;
			}
		}

		_pImpl->listenSocket = s;

		// listenSocket을 IOCP에 연결하는 작업 수행 필요

		if (SOCKET_ERROR == ::listen(_pImpl->listenSocket, SOMAXCONN))
		{
			LOG_FETAL(L"listen fail");
			::closesocket(_pImpl->listenSocket);
			_pImpl->listenSocket = INVALID_SOCKET;
			return;
		}

		if (!NetworkHelper::AssociateDeviceWithIOCP(reinterpret_cast<HANDLE>(_pImpl->listenSocket), _pImpl->iocpHandle, reinterpret_cast<uint64_t>(this)))
		{
			LOG_DEBUG(L"AssociateDeviceWithIOCP fail");
			return;
		}

		for (int i = 0; i < CONTEXT_COUNT; ++i)
		{
			auto& context{ _context[i] };

			if (!asyncAccept(i))
			{
				LOG_FETAL(L"asyncAccept Fail, index:{}", i);
				::closesocket(_pImpl->listenSocket);
				_pImpl->listenSocket = INVALID_SOCKET;
				break;
			}
		}

	}

	bool Listener::HandleEvent(OVERLAPPED* context, unsigned long bytes)
	{
		const auto acceptContext = static_cast<AcceptContext*>(context);

		if (!acceptContext || !onAccept(*acceptContext))
		{
			::closesocket(acceptContext->_socket);
			acceptContext->_socket = INVALID_SOCKET;
			return false;
		}

		asyncAccept(acceptContext->_index);

		LOG_INFO(L"accept success, index:{}", acceptContext->_index);
		return true;
	}

	void Listener::Clear()
	{
		SOCKET s = ::InterlockedExchange64(reinterpret_cast<int64_t volatile*>(_pImpl->listenSocket), INVALID_SOCKET);
		if (INVALID_SOCKET != s)
			::closesocket(s);
	}

	bool Listener::asyncAccept(int index)
	{
		auto& context{ _context[index] };
		if (INVALID_SOCKET == context._socket)
		{
			context._socket = MakeTCPSocket();
			if (INVALID_SOCKET == context._socket)
			{
				LOG_FETAL(L"initialize socket");
				return false;
			}
		}

		int addrSize{ sizeof(SOCKADDR) + 16 };
		unsigned long recvedSize{ 0 };

		if (_pImpl->acceptexFunc(_pImpl->listenSocket, context._socket, context._buffer, 0,
			addrSize, addrSize, &recvedSize, &(context)))
		{
			if (const auto errCode = ::WSAGetLastError();
				WSA_IO_PENDING != errCode)
			{
				LOG_FETAL(L"acceptex fail errCode:{}", errCode);
				return false;
			}
		}
		return true;
	}

	bool Listener::onAccept(AcceptContext& context)
	{
		ASSERT_WITH_MSG(0 <= context._index && context._index < CONTEXT_COUNT, std::format(L"INVALID AcceptContext index {}", context._index).c_str());
		int addrSize{ sizeof(SOCKADDR) + 16 };
		uint32_t recvedSize{ 0 };

		if (SOCKET_ERROR == ::setsockopt(context._socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&context._socket, sizeof(context._socket)))
		{
			LOG_FETAL(L"setsockopt fail SO_UPDATE_ACCEPT_CONTEXT");
			return false;
		}

		NetworkHelper::SetSocketOptNoBuffer(context._socket);

		int localAddrSize;
		int remoteAddrSize;
		sockaddr_in* localAddr;
		sockaddr_in* remoteAddr;

		_pImpl->getAcceptExSockAddrFunc(&context._buffer, context._recvdSize, context._localAddrSize, context._localAddrSize,
			reinterpret_cast<LPSOCKADDR*>(&localAddr), &localAddrSize,
			reinterpret_cast<LPSOCKADDR*>(&remoteAddr), &remoteAddrSize);

		// 세션 생성
		int64_t sessionId{ INVALID_ID };
		Session* session = Session::MakeSession(sessionId);
		if (!session)
		{
			LOG_ERROR(L"can not make session, sessionId:{}, remoteAddress:{}, remotePort:{}", sessionId, remoteAddr->sin_addr.S_un.S_addr, ::ntohs(remoteAddr->sin_port));
			return false;
		}

		if (!session->OnAccept(sessionId, context._socket, remoteAddr->sin_addr.S_un.S_addr, ::ntohs(remoteAddr->sin_port)))
		{
			LOG_ERROR(L"session onAccept fail, sessionId:{}, remoteAddress:{}, remotePort:{}", sessionId, remoteAddr->sin_addr.S_un.S_addr, ::ntohs(remoteAddr->sin_port));
			return false;
		}

		if (!session->Recv())
		{
			LOG_ERROR(L"async recv error, id:{}, error:{}", sessionId, WSAGetLastError());
		}

		return true;
	}
}
