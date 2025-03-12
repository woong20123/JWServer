#pragma once
#ifndef __JW_ROOM_H__
#define __JW_ROOM_H__
#include <cstdint>
#include <unordered_map>
#include <string>

namespace jw
{
    struct RoomUserInfo
    {
        std::string _name;
    };

    class Room
    {
    public:
        using userID = int64_t;
        using RoomID = int64_t;

        Room();
        ~Room();

        void Initialize(RoomID id, const std::string& name);
        const std::string& GetRoomName() const { return _name; }

    private:
        RoomID _id;
        std::string _name;
        std::unordered_map<userID, RoomUserInfo> _userList;
    };
}

#endif // !__JW_ROOM_H__

