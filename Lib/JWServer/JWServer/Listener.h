#pragma once
#ifndef __JW_LISTENER_H__
#define __JW_LISTENER_H__
#include "AsyncIOObject.h"
#include <memory>
#include <array>
#include <ws2tcpip.h>
#include <mswsock.h>

namespace jw
{
	struct AcceptContext : public AsyncContext
	{
		AcceptContext() : AsyncContext(ASYNC_CONTEXT_ID_ACCEPT),
			_index{ 0 },
			_socket{ INVALID_SOCKET },
			_recvdSize{ 0 },
			_localAddrSize{ sizeof(SOCKADDR) + 16 },
			_remoteAddrSize{ sizeof(SOCKADDR) + 16 }
		{
			::memset(_buffer, 0x00, sizeof(_buffer));
		}

		static constexpr size_t CONTEXT_BUFFER_COUNT = 128;

		uint16_t	_index;
		SOCKET		_socket;
		uint32_t	_recvdSize;
		uint32_t	_localAddrSize;
		uint32_t	_remoteAddrSize;
		uint8_t		_buffer[CONTEXT_BUFFER_COUNT];
	};

	class Listener : public AsyncIOObject
	{
	public:
		Listener();
		~Listener();
		bool Initialize(uint32_t portId, uint16_t portNumber, HANDLE iocpHandle, const LPFN_ACCEPTEX acceptexFunc, const LPFN_GETACCEPTEXSOCKADDRS acceptexSockAddrFunc);
		uint32_t GetAsyncObjectId() const override { return ASYNC_IO_OBJ_LISTENER; }
		bool HandleEvent(AsyncContext* context, unsigned long bytes) override;


		void Clear();
	private:
		static constexpr size_t CONTEXT_COUNT = 128;

		bool asyncAccept(int index);
		bool onAccept(AcceptContext& context);

		LPFN_ACCEPTEX								_acceptexFunc;
		LPFN_GETACCEPTEXSOCKADDRS					_getAcceptExSockAddrFunc;
		uint32_t									_portId;
		uint16_t									_portNumber;
		SOCKET										_listenSocket;
		HANDLE										_iocpHandle;
		bool										_nagle{ false };
		std::array<AcceptContext, CONTEXT_COUNT>	_context;
	};
}

#endif // ! __JW_LISTENER_H__


