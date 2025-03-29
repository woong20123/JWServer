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
        void Initialize(const std::shared_ptr<User>& user, const std::string& roomName);
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
        void Initialize(const std::shared_ptr<User>& user);
        void Execute() override;
    private:
        std::shared_ptr<User> _user;
    };

    class CreateRoomNotifyTask : public WorldSerializeObject
    {
    public:
        CreateRoomNotifyTask();
        virtual ~CreateRoomNotifyTask();
        void Initialize(int64_t roomId, std::string& roomName, int64_t hostUserId, std::string& hostName);
        void Execute() override;
    private:
        int64_t _roomId;
        std::string _roomName;
        int64_t _hostUserId;
        std::string _hostUserName;
    };

    class DestoroyRoomNotifyTask : public WorldSerializeObject
    {
    public:
        DestoroyRoomNotifyTask();
        virtual ~DestoroyRoomNotifyTask();
        void Initialize(const int64_t roomId);
        void Execute() override;
    private:
        int64_t _roomId;
    };
}

#endif

