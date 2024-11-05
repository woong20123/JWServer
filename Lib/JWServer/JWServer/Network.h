#pragma once
#ifndef __JW_NETWORK_H__
#define __JW_NETWORK_H__
#include "Singleton.hpp"
#include <memory>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>

namespace jw
{
    class Network : public Singleton<Network>
    {
    public:
        bool Initialize();

        LPFN_ACCEPTEX                   GetAcceptExFunc();
        LPFN_GETACCEPTEXSOCKADDRS       GetAcceptExSockAddrFunc();
        LPFN_CONNECTEX                  GetConnectExFunc();
        LPFN_DISCONNECTEX               GetisConnectExFunc();
    protected:
        Network();
        ~Network();
        Network(const Network&) = delete;
        Network& operator=(const Network&) = delete;
    private:
        bool initializeWSASocketFunc();

        LPFN_ACCEPTEX                    _acceptExFunc{ nullptr };
        LPFN_GETACCEPTEXSOCKADDRS        _getAcceptExSockAddrFunc{ nullptr };
        LPFN_CONNECTEX                   _connectExFunc{ nullptr };
        LPFN_DISCONNECTEX                _disConnectExFunc{ nullptr };
        HANDLE  _iocpHandle;
        friend class Singleton<Network>;
    };
}

#define NETWORK jw::Network::GetInstance

#endif // !__JW_NETWORK_H__

