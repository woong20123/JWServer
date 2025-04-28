#include "NetworkHelper.h"
#include "StringConverter.h"
#include <thread>

namespace jw
{
    void NetworkHelper::SetSocketOptNoBuffer(SOCKET& s)
    {
        int bufferSize{ 0 };
        ::setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char*)&bufferSize, sizeof(bufferSize));
        ::setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char*)&bufferSize, sizeof(bufferSize));
    }

    HANDLE NetworkHelper::CreateNewIOCP()
    {
        return ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
    }

    bool NetworkHelper::AssociateDeviceWithIOCP(HANDLE registerHandle, HANDLE iocpHandle, uint64_t key)
    {
        HANDLE h = ::CreateIoCompletionPort(registerHandle, iocpHandle, key, 0);
        return (h == iocpHandle);
    }

    uint16_t NetworkHelper::GetProcessorCount()
    {
#if _HAS_CXX17 

        return static_cast<uint16_t>(std::thread::hardware_concurrency());

#else 
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        return static_cast<uint16_t>(sysInfo.dwNumberOfProcessors);
#endif
    }

    const std::string NetworkHelper::GetAddressStringA(uint32_t address)
    {
        return std::format("{}.{}.{}.{}", address & 0xFF, address >> 8 & 0xFF, address >> 16 & 0xFF, address >> 24 & 0xFF);
    }

    const std::wstring NetworkHelper::GetAddressStringW(uint32_t address)
    {

        return StringConverter::StrA2WUseUTF8(GetAddressStringA(address)).value();
    }
}

