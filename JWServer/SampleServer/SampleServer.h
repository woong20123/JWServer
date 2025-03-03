#pragma once
#ifndef __JW_SAMPLE_SERVER_H__
#define __JW_SAMPLE_SERVER_H__
#include "Server.h"
#include "World.h"
#include "Singleton.hpp"
#include <memory>

namespace jw
{
    class World;
    class SampleServer : public Server, public Singleton<SampleServer>
    {
    public:
        World* GetWorld();
    protected:
        SampleServer();
        virtual ~SampleServer();

        bool onStartLog() override;
        bool onStartNetwork() override;
        bool onStartTimer() override;
        bool onInitialize() override;
        bool onHandleEvent(const std::shared_ptr<ServerEvent>& eventObj) override;
        void onClosedServer() override;

    private:

        std::unique_ptr<World> _world;
        friend class Singleton<SampleServer>;
    };
}

#define SAMPLE_SERVER jw::SampleServer::GetInstance
#endif // !__JW_SAMPLE_SERVER_H__
