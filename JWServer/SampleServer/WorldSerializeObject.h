#pragma once
#ifndef __JW_WORLD_SERIALIZE_OBJECT_H__
#define __JW_WORLD_SERIALIZE_OBJECT_H__
#include "SerializeObject.h"
#include <string>

namespace jw
{
    class User;
    class WorldSerializeObject : public SerializeObject
    {
    public:
        WorldSerializeObject(int32_t id);
        virtual ~WorldSerializeObject() = default;
    protected:
        virtual void Execute() = 0;
    };

    class WorldChatTask : public WorldSerializeObject
    {
    public:
        WorldChatTask();
        virtual ~WorldChatTask();
        void Initialize(const std::string& name, const std::string& msg);

        void Execute() override;
    private:
        std::string _name;
        std::string _message;
    };

    class CreateRoomTask : public WorldSerializeObject
    {
    public:
        CreateRoomTask();
        virtual ~CreateRoomTask();
        void Initialize(std::shared_ptr<User> user, const std::string& roomName);
        void Execute() override;
    private:
        void sendFail(int32_t errCode);
        std::shared_ptr<User> _user;
        std::string _roomName;
    };

    class GameRoomListTask : public WorldSerializeObject
    {
    public:
        GameRoomListTask();
        virtual ~GameRoomListTask();
        void Execute() override;
    };
}

#endif

