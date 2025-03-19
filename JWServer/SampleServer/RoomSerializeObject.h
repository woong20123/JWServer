#pragma once
#ifndef __JW_ROOM_SERIALIZE_OBJECT_H__
#define __JW_ROOM_SERIALIZE_OBJECT_H__
#include "SerializeObject.h"
#include <string>

namespace jw
{
    class User;
    class RoomSerializeObject : public SerializeObject
    {
    public:
        RoomSerializeObject(int32_t id);
    protected:
        virtual void Execute() = 0;
    };

    class RoomChatTask : public RoomSerializeObject
    {
    public:
        RoomChatTask();
        virtual ~RoomChatTask();
        void Initialize(const int64_t roomId, const std::shared_ptr<User>& user, const std::string& msg);
        void Execute() override;
    private:

        int64_t _roomId;
        std::shared_ptr<User> _user;
        std::string _message;
    };
}
#endif // !__JW_ROOM_SERIALIZE_OBJECT_H__

