#include "Listener.h"
#include "Logger.h"
#include "Network.h"
#include "NetworkHelper.h"
#include "Session.h"

namespace jw
{
	Listener::Listener() :
		_nagle{ false },
		_portNumber{ 0 },
		_listenSocket{ INVALID_SOCKET },
		_iocpHandle{ INVALID_HANDLE_VALUE }
	{}

	bool Listener::Initialize(uint32_t portId, uint16_t portNumber, HANDLE iocpHandle, const LPFN_ACCEPTEX acceptexFunc, const LPFN_GETACCEPTEXSOCKADDRS acceptexSockAddrFunc)
	{
		if (portId < 0 || portNumber < 0 || iocpHandle == INVALID_HANDLE_VALUE ||
			!acceptexFunc || !acceptexSockAddrFunc)
		{
			LOG_FETAL(L"intialize failed parameter invalid, portId:{}, portNumber:{}, iocpHandle:{}, acceptexFunc:{}, acceptexSockAddrFunc:{}"
				, portId, portNumber, iocpHandle, (void*)acceptexFunc, (void*)acceptexSockAddrFunc);
			return false;
		}

		_portId = portId;
		_portNumber = portNumber;
		_iocpHandle = iocpHandle;
		_acceptexFunc = acceptexFunc;
		_getAcceptExSockAddrFunc = acceptexSockAddrFunc;


		SOCKADDR_IN addr;
		SOCKET s = MakeTCPSocket();
		if (INVALID_SOCKET == s)
		{
			LOG_FETAL(L"initialize failed make listen socket, portId:{}, portNumber:{}", _portId, _portNumber);
			return false;
		}

		::memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.S_un.S_addr = INADDR_ANY;
		addr.sin_port = ::htons(_portNumber);

		if (SOCKET_ERROR == ::bind(s, reinterpret_cast<SOCKADDR*>(&addr), sizeof(addr)))
		{
			LOG_FETAL(L"fail socket bind");
			::closesocket(s);
			return false;
		}

		// nagle 옵션이 꺼져 있다면 딜레이 없이 동작하도록 설정 합니다. 
		if (!_nagle)
		{
			int optval = 1;
			if (SOCKET_ERROR == ::setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (const char*)&optval, sizeof(optval)))
			{
				LOG_FETAL(L"initialize failed TCP_NODELAY set fail, portId:{}, portNumber:{}", _portId, _portNumber);
				::closesocket(s);
				return false;
			}
		}

		_listenSocket = s;

		// listenSocket을 IOCP에 연결하는 작업 수행 필요

		if (SOCKET_ERROR == ::listen(_listenSocket, SOMAXCONN))
		{
			LOG_FETAL(L"initialize failed listen fail, portId:{}, portNumber:{}", _portId, _portNumber);
			::closesocket(_listenSocket);
			_listenSocket = INVALID_SOCKET;
			return false;
		}

		if (!NetworkHelper::AssociateDeviceWithIOCP(reinterpret_cast<HANDLE>(_listenSocket), _iocpHandle, reinterpret_cast<uint64_t>(this)))
		{
			LOG_FETAL(L"listenSocket fail AssociateDeviceWithIOCP, portId:{}, portNumber:{}", _portId, _portNumber);
			return false;
		}

		for (int i = 0; i < CONTEXT_COUNT; ++i)
		{
			auto& context{ _context[i] };

			if (!asyncAccept(i))
			{
				LOG_FETAL(L"asyncAccept Fail, index:{}, portId:{}, portNumber:{}", i, _portId, _portNumber);
				::closesocket(_listenSocket);
				_listenSocket = INVALID_SOCKET;
				return false;
			}
		}

		LOG_INFO(L"Listener Initialize OK, portId:{}, portNumber:{}", _portId, _portNumber);
		return true;
	}

	bool Listener::HandleEvent(AsyncContext* context, unsigned long bytes)
	{
		const auto acceptContext = static_cast<AcceptContext*>(context);

		if (!acceptContext || !onAccept(*acceptContext))
		{
			::closesocket(acceptContext->_socket);
			acceptContext->_socket = INVALID_SOCKET;
			return false;
		}

		asyncAccept(acceptContext->_index);

		LOG_INFO(L"accept success, index:{}, portId:{}, portNumber:{}", acceptContext->_index, _portId, _portNumber);
		return true;
	}

	void Listener::Clear()
	{
		SOCKET s = ::InterlockedExchange64(reinterpret_cast<int64_t volatile*>(_listenSocket), INVALID_SOCKET);
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
				LOG_FETAL(L"initialize socket, portId:{}, portNumber:{}", _portId, _portNumber);
				return false;
			}
		}

		int addrSize{ sizeof(SOCKADDR) + 16 };
		unsigned long recvedSize{ 0 };

		if (_acceptexFunc(_listenSocket, context._socket, context._buffer, 0,
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
		ASSERT_WITH_MSG(0 <= context._index && context._index < CONTEXT_COUNT, std::format("INVALID AcceptContext index {}", context._index).c_str());
		int addrSize{ sizeof(SOCKADDR) + 16 };
		uint32_t recvedSize{ 0 };

		if (SOCKET_ERROR == ::setsockopt(context._socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&context._socket, sizeof(context._socket)))
		{
			LOG_FETAL(L"setsockopt fail SO_UPDATE_ACCEPT_CONTEXT portId:{}, portNumber:{}", _portId, _portNumber);
			return false;
		}

		NetworkHelper::SetSocketOptNoBuffer(context._socket);

		int localAddrSize;
		int remoteAddrSize;
		sockaddr_in* localAddr;
		sockaddr_in* remoteAddr;

		_getAcceptExSockAddrFunc(&context._buffer, context._recvdSize, context._localAddrSize, context._localAddrSize,
			reinterpret_cast<LPSOCKADDR*>(&localAddr), &localAddrSize,
			reinterpret_cast<LPSOCKADDR*>(&remoteAddr), &remoteAddrSize);


		NETWORK().IsBadIp(remoteAddr->sin_addr.S_un.S_addr, static_cast<int32_t>(NETWORK().GetSessionCount(_portId)));

		// 세션 생성
		const auto session = NETWORK().CreateSession(_portId);

		if (!session || !session->SetSocketInfo(context._socket, remoteAddr))
		{
			LOG_ERROR(L"can not make session, portNumber:{}, remoteAddress:{}, remotePort:{}", _portNumber, remoteAddr->sin_addr.S_un.S_addr, ::ntohs(remoteAddr->sin_port));
			return false;
		}

		if (!session->OnAccept())
		{
			LOG_ERROR(L"session onAccept fail, sessionId:{}, portNumber:{}, remoteAddress:{}, remotePort:{}", session->GetId(), _portNumber, remoteAddr->sin_addr.S_un.S_addr, ::ntohs(remoteAddr->sin_port));
			return false;
		}

		if (!session->Recv())
		{
			LOG_ERROR(L"async recv error, id:{}, error:{}, _portNumber:{}", session->GetId(), WSAGetLastError(), _portNumber);
		}

		return true;
	}
}
