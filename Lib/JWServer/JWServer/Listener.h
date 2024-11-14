#pragma once
#ifndef __JW_LISTENER_H__
#define __JW_LISTENER_H__
#include "AsyncIOObject.h"
#include <memory>
#include <array>

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
		void Initialize(const LPFN_ACCEPTEX acceptexFunc, const LPFN_GETACCEPTEXSOCKADDRS acceptexSockAddrFunc, uint16_t port, HANDLE iocpHandle);
		uint32_t GetAsyncObjectId() const override { return ASYNC_IO_OBJ_LISTENER; }
		bool HandleEvent(OVERLAPPED* context, unsigned long bytes) override;


		void Clear();
	private:
		static constexpr size_t CONTEXT_COUNT = 128;

		bool asyncAccept(int index);
		bool onAccept(AcceptContext& context);

		struct Impl;
		std::unique_ptr<Impl> _pImpl;

		std::array<AcceptContext, CONTEXT_COUNT> _context;
	};
}

#endif // ! __JW_LISTENER_H__


