#pragma once
#ifndef __JW_NETWORK_H__
#define __JW_NETWORK_H__
#include "Singleton.hpp"
#include <memory>
#include <unordered_map>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>

namespace jw
{
    class IOWorker;
    class Port;
    class Session;
    class SessionInspector;
    class SessionInspectorInfoTable;
    class Network : public Singleton<Network>
    {
    public:
        using PortId_t = uint16_t;
        using PortContainer = std::unordered_map<uint16_t, std::shared_ptr<Port>>;

        static constexpr PortId_t INVALID_PORT_ID_TYPE = 0;
        // 해당 값으로 설정시 (프로세서 * 2)개의 스레드를 생성합니다 .
        static constexpr uint16_t DEFAULT_WORKER_THREAD_COUNT = 0;

        bool Initialize();

        bool Start(uint16_t& workerThreadCount);
        void Stop();

        LPFN_ACCEPTEX                   GetAcceptExFunc() const;
        LPFN_GETACCEPTEXSOCKADDRS       GetAcceptExSockAddrFunc() const;
        LPFN_CONNECTEX                  GetConnectExFunc() const;
        LPFN_DISCONNECTEX               GetisConnectExFunc() const;
        HANDLE                          GetIOCPHandle() const;

        bool                            RegistPort(const PortId_t portId, std::shared_ptr<Port>& port);
        Session* CreateSession(const PortId_t portId);
        bool                            DestroySession(const PortId_t portId, Session* session);
        std::shared_ptr<Session>        GetSession(uint64_t sessionId);
        std::shared_ptr<Session>        GetSession(const PortId_t portId, const int32_t sessionIndex);
        bool                            RegisterSessionInspectorInfoTable(const PortId_t portId, std::shared_ptr<SessionInspectorInfoTable>& table);


    protected:
        Network();
        ~Network();
        Network(const Network&) = delete;
        Network& operator=(const Network&) = delete;
    private:
        bool initializeWinSock();
        bool initializeWSASocketFunc();
        bool initializeIOWorkers();

        std::shared_ptr<Port>& getPort(const PortId_t portId);

        LPFN_ACCEPTEX                       _acceptExFunc{ nullptr };
        LPFN_GETACCEPTEXSOCKADDRS           _getAcceptExSockAddrFunc{ nullptr };
        LPFN_CONNECTEX                      _connectExFunc{ nullptr };
        LPFN_DISCONNECTEX                   _disConnectExFunc{ nullptr };
        HANDLE                              _iocpHandle;
        uint16_t                            _workerThreadCount;
        std::unique_ptr<IOWorker>           _ioWorker;
        PortContainer                       _portContainer;
        std::unique_ptr<SessionInspector>   _sessionInspector;

        static std::shared_ptr<Port>               _NullPort;
        friend class Singleton<Network>;
    };
}

#define NETWORK jw::Network::GetInstance

#endif // !__JW_NETWORK_H__

