#pragma once

#ifndef __JW_SERVER_H__
#define __JW_SERVER_H__
#include <string>
#include <memory>
#include <span>
#include "Logger.h"

namespace jw
{
    class Port;
    struct PortInfo;
    class LogWorker;
    class LogStream;
    class Server
    {
    public:
        static constexpr const wchar_t* INVALID_SERVER_NAME = L"INVALID_SERVER_NAME";

        static constexpr uint32_t   NONE_PORT_ID = 0;
        static constexpr uint32_t   CLIENT_PORT_ID = 1;

        Server();
        virtual ~Server();

        bool Initialize(const std::wstring& name);
        bool Start(int argc, char* argv[]);
    protected:
        bool startLog();
        bool setArgument(int argc, char* argv[]);
        bool startNetwork();

        virtual bool onStartLog() = 0;
        virtual bool onStartNetwork() = 0;

        void registConsoleLogStream(const std::span<LogType> logFlags);
        void registFileLogStream(const std::span<LogType> logFlags);
        void registLogStream(const std::shared_ptr<LogStream>& logStream);

        void setNetworkWorkerThread(uint16_t);
        void reigstPort(const PortInfo& portInfo);

    private:
        std::wstring                    _name;
        std::unique_ptr<LogWorker>      _logWorker;
        uint16_t                        _workerThreadCount;
    };
}

#endif // !__JW_SERVER_H__ 

