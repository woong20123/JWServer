#pragma once
#ifndef __JW_NETWOKR_HELPER_H__
#define __JW_NETWOKR_HELPER_H__
#include <WinSock2.h>
#include <cstdint>
#include <string>

namespace jw
{

    class NetworkHelper
    {
    public:
        static void SetSocketOptNoBuffer(SOCKET& s);

        static HANDLE CreateNewIOCP();
        static bool AssociateDeviceWithIOCP(HANDLE h, HANDLE iocpHandle, uint64_t key);

        static uint16_t GetProcessorCount();

        static const std::string GetAddressStringA(uint32_t address);
        static const std::wstring GetAddressStringW(uint32_t address);
    };
}

#define MakeTCPSocket() ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED)

#endif // !__JW_NETWOKR_HELPER_H__