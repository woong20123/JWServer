#include "Network.h"
#include "Logger.h"
#include <vector>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>

#pragma comment(lib, "ws2_32.lib")

namespace jw
{
    struct Network::Impl
    {
        Impl() {}
        LPFN_ACCEPTEX                    acceptExFunc{ NULL };
        LPFN_GETACCEPTEXSOCKADDRS        getAcceptExSockAddrFunc{ nullptr };
        LPFN_CONNECTEX                   connectExFunc{ nullptr };
        LPFN_DISCONNECTEX                disConnectExFunc{ nullptr };
    };

    Network::Network() :
        _pImpl{ std::make_unique<Impl>() }
    {}

    Network::~Network() {
        WSACleanup();
    }

    bool Network::Initialize()
    {
        WSADATA	wsaData;
        const auto ret = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (0 != ret)
        {
            LOG_FETAL(L"WSAStartup Fail, ret:{}", ret);
            return false;
        }

        if (!initializeWSASocketFunc())
        {
            LOG_FETAL_STRING(L"initializeWSASocketFunc Fail");
            return false;
        }

        return true;
    }

    bool Network::initializeWSASocketFunc()
    {
        ULONG bytes{ 0 };

        SOCKET s = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
        if (INVALID_SOCKET == s) {
            LOG_FETAL(L"WSASocket Error err:{}", ::WSAGetLastError());
            return false;
        }

        std::vector<std::tuple<std::wstring_view, GUID, LPVOID, DWORD>> functionInfos = {
            {L"WSAID_ACCEPTEX",             WSAID_ACCEPTEX,                 _pImpl->acceptExFunc == nullptr ? &_pImpl->acceptExFunc : nullptr,                           
                sizeof(_pImpl->acceptExFunc)},
            {L"WSAID_GETACCEPTEXSOCKADDRS", WSAID_GETACCEPTEXSOCKADDRS,     _pImpl->getAcceptExSockAddrFunc == nullptr ? & _pImpl->getAcceptExSockAddrFunc : nullptr,
                sizeof(_pImpl->getAcceptExSockAddrFunc)},
            {L"WSAID_CONNECTEX",            WSAID_CONNECTEX,                _pImpl->connectExFunc == nullptr ? &_pImpl->connectExFunc : nullptr,
                sizeof(_pImpl->connectExFunc)},
            {L"WSAID_DISCONNECTEX",         WSAID_DISCONNECTEX,             _pImpl->disConnectExFunc == nullptr ? &_pImpl->disConnectExFunc : nullptr,
                sizeof(_pImpl->disConnectExFunc)},
        };

        for (const auto& funcInfo : functionInfos)
        {
            auto guid = std::get<1>(funcInfo);
            LPVOID func = std::get<2>(funcInfo);
            const auto funcSize = std::get<3>(funcInfo);
            if (func)
            {
                if (SOCKET_ERROR == ::WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER,
                    &guid, sizeof(guid), func, funcSize, &bytes, nullptr, nullptr))
                {

                    LOG_FETAL(L"WSAIoctl Error func:{}, errcode:{}", std::get<0>(funcInfo), ::WSAGetLastError());
                    return false;
                }
            }
        }
        return true;
    }
}
