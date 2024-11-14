#include "Session.h"
#include "SessionBuffer.h"
#include "Network.h"
#include "NetworkHelper.h"
#include "Logger.h"
#include "TypeDefinition.h"

namespace jw
{
    Session::Session() :
        _id{ INVALID_ID },
        _recvBuffer{ nullptr }
    {
    }

    bool Session::Initialize(int64_t id, SOCKET socket, sockaddr_in* remoteAddr)
    {
        if (INVALID_ID == id || INVALID_SOCKET == socket ||
            nullptr == remoteAddr)
        {
            LOG_ERROR(L"invalid parameter, id:{}, socket:{}, ip:{}", id, socket);
            return false;
        }

        _id = id;
        _socket = socket;
        _ip = remoteAddr->sin_addr.S_un.S_addr;
        _port = remoteAddr->sin_port;
        _recvBuffer = std::make_unique<SessionRecvBuffer>();

        wchar_t ipAddress[16]{ 0, };
        _ipString = InetNtopW(AF_INET, &remoteAddr->sin_addr, ipAddress, sizeof(ipAddress));

        LOG_INFO(L"Session Initialize, id:{}, ip:{}, port:{}", GetId(), _ipString.c_str(), _port);

        return true;
    }

    int64_t Session::GetId() const { return _id; }

    bool Session::HandleEvent(OVERLAPPED* context, paramType bytes)
    {
        auto* asyncContext{ static_cast<AsyncContext*>(context) };
        switch (asyncContext->_id)
        {
        case ASYNC_CONTEXT_ID_RECV:
            if (0 == bytes) {
                LOG_ERROR(L"recv zero, socket has closed, id:{}", GetId());
                return false;
            }

            if (0 == _recvBuffer->UpdateRecvedSize(bytes))
            {
                LOG_ERROR(L"UpdateRecvedSize fail, id:{}", GetId());
                return false;
            }

            // 패킷 핸들
            LOG_DEBUG(L"on async recved, id:{}, bytes:{}", GetId(), bytes);
            if (!Recv())
            {
                LOG_ERROR(L"next async recv error, id:{}, error:{}", GetId(), WSAGetLastError());
                return false;
            }
            break;
        case ASYNC_CONTEXT_ID_SEND:
            break;
        case ASYNC_CONTEXT_ID_CONNECT:
            break;
        default:
            break;
        }

        return true;
    }

    bool Session::OnAccept()
    {
        if (!NetworkHelper::AssociateDeviceWithIOCP(reinterpret_cast<HANDLE>(_socket), NETWORK().GetIOCPHandle(), reinterpret_cast<uint64_t>(this)))
        {
            LOG_FETAL(L"sessionSocket fail AssociateDeviceWithIOCP, id:{}, ip:{}, port:{}", GetId(), _ipString.c_str(), _port);
            return false;
        }
        LOG_INFO(L"OnAccept Session, id:{}, ip:{}, port:{}", GetId(), _ipString.c_str(), _port)
        return true;
    }

    bool Session::Recv()
    {
        {
            std::unique_lock lock{ _mutex };
            asyncRecv();
        }
        return true;
    }

    bool Session::asyncRecv()
    {
        auto recvContext = _recvBuffer->GetContext();
        recvContext->hEvent = NULL;
        recvContext->_session = this;
        recvContext->_wsaBuffer.len = _recvBuffer->GetFreeBufferSize();
        recvContext->_wsaBuffer.buf = _recvBuffer->GetFreeBuffer();

        unsigned long recvedSize{ 0 }, recvedFlag{ 0 };

        if (SOCKET_ERROR == ::WSARecv(_socket, &recvContext->_wsaBuffer, 1, &recvedSize, &recvedFlag, recvContext, NULL))
        {
            const auto errorCode{ ::WSAGetLastError() };
            if (WSA_IO_PENDING != errorCode)
            {
                LOG_FETAL(L"WSARecv() fail, err:{}", errorCode);
                return false;
            }
        }

        return true;
    }

    Session* Session::MakeSession(int64_t& sessionId)
    {
        Session* session{ new Session() };
        if (!session) return nullptr;

        static std::atomic<int64_t> sessionGen{ 1 };
        sessionId = sessionGen;
        sessionGen.fetch_add(1);

        if (INVALID_ID == sessionId)
        {
            LOG_FETAL(L"sessionId is zero, sessionId:{}, sessionGen:{}", sessionId, sessionGen.load());
            return nullptr;
        }
        return session;
    }
}

