#pragma once
#ifndef __JW_SAMPLE_SERVER_H__
#define __JW_SAMPLE_SERVER_H__
#include "Server.h"
#include "Singleton.hpp"

namespace jw
{
    class SampleServer : public Server, public Singleton<SampleServer>
    {
    protected:
        SampleServer();
        virtual ~SampleServer();

        bool onStartLog() override;
        bool onStartNetwork() override;
    private:
        friend class Singleton<SampleServer>;
    };
}

#define SAMPLE_SERVER jw::SampleServer::GetInstance
#endif // !__JW_SAMPLE_SERVER_H__
