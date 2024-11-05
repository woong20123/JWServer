#pragma once
#ifndef __JW_LISTENER_H__
#define __JW_LISTENER_H__
#include "AsyncIOObject.h"
#include <memory>
#include <array>

namespace jw
{
	class Listener : public AsyncIOObject
	{
	public:
		Listener();
		~Listener();
		void Initialize(const LPFN_ACCEPTEX acceptexFunc, const LPFN_GETACCEPTEXSOCKADDRS acceptexSockAddrFunc, uint16_t port);
		uint32_t GetEventID() const override { return 1; }
		bool HandleEvent(OVERLAPPED* context, unsigned long bytes) override;


		void Clear();
	private:
		static constexpr size_t CONTEXT_COUNT = 128;
		static constexpr size_t CONTEXT_BUFFER = 256;

		struct AcceptContext : public OVERLAPPED
		{
			uint16_t	_index;
			SOCKET		_socket;
			uint32_t	_recvdSize;
			uint32_t	_localAddrSize;
			uint32_t	_remoteAddrSize;
			uint8_t		_buffer[CONTEXT_BUFFER];
		};

		bool asyncAccept(int index);
		bool accepted(AcceptContext& context);

		struct Impl;
		std::unique_ptr<Impl> _pImpl;

		std::array<AcceptContext, CONTEXT_COUNT> _context;
	};
}

#endif // ! __JW_LISTENER_H__


