#pragma once

#ifndef __JW_SERVER_H__
#define __JW_SERVER_H__
#include "Singleton.hpp"
#include <string>
#include <memory>

namespace jw
{
    class Port;
    class LogWorker;
    class Server : public Singleton<Server>
    {
    public:
        static constexpr const wchar_t * INVALID_SERVER_NAME =  L"INVALID_SERVER_NAME";

        static constexpr uint32_t   NONE_PORT_ID = 0;
        static constexpr uint32_t   CLIENT_PORT_ID = 1;


        bool Initialize(const std::wstring& name);
        bool Start(int argc, char* argv[]);
    protected:
        virtual bool initializeLog();
        virtual bool initializeNetwork();

        Server();
        virtual ~Server();

    private:
        std::wstring                    _name;
        std::unique_ptr<LogWorker>      _logWorker;

        friend class Singleton<Server>;
    };
}

#endif // !__JW_SERVER_H__ 

