#include "NetworkHelper.h"

namespace jw
{
    void NetworkHelper::SetSocketOptNoBuffer(SOCKET& s)
    {
        int bufferSize{ 0 };
        ::setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char*)&bufferSize, sizeof(bufferSize));
        ::setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char*)&bufferSize, sizeof(bufferSize));
    }

    HANDLE NetworkHelper::CreateIOCPHandle()
    {
        return ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
    }

    bool NetworkHelper::RegistIOCP(HANDLE registerHandle, HANDLE iocpHandle, uint64_t key)
    {
        return ::CreateIoCompletionPort(registerHandle, iocpHandle, key, 0);
    }
}
