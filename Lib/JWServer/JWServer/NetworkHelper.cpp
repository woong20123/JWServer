#include "NetworkHelper.h"

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
}
