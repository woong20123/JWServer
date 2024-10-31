#pragma once
#ifndef __JW_LISTENER_H__
#define __JW_LISTENER_H__
#include <memory>
#include <ws2tcpip.h>
#include <mswsock.h>

namespace jw
{
	class Listener
	{
	public:
		Listener();
		~Listener();
		void Initialize(const LPFN_ACCEPTEX acceptexFunc, uint16_t port);
	private:
		struct Impl;
		std::unique_ptr<Impl> _pImpl;
	};
}

#endif // ! __JW_LISTENER_H__


