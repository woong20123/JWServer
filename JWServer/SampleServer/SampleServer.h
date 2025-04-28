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
    class RoomManager;
    class SampleServerConfig;
    class SampleServer : public Server, public Singleton<SampleServer>
    {
    public:
        World* GetWorld();
        RoomManager* GetRoomManager();
        SampleServerConfig* GetConfig();
    protected:
        SampleServer();
        virtual ~SampleServer();

    private:
        bool onStartingLog() override;
        bool onStartedLog() override;
        bool onStartConfig() override;
        bool onStartingNetwork() override;
        bool onStartedNetwork() override;
        bool onStartedTimer() override;
        bool onStartingTimer() override;
        bool onInitialize() override;
        bool onHandleEvent(const std::shared_ptr<ServerEvent>& eventObj) override;
        void onClosedServer() override;

        std::unique_ptr<World>              _world;
        std::unique_ptr<RoomManager>        _roomManager;
        std::unique_ptr<SampleServerConfig> _config;
        friend class Singleton<SampleServer>;
    };
}

#define SAMPLE_SERVER jw::SampleServer::GetInstance
#endif // !__JW_SAMPLE_SERVER_H__
