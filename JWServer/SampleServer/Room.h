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

        void Initialize(RoomID id, const std::string& name, const int64_t hostUserId, const std::string& hostUserName);
        const std::string_view GetRoomName() const { return _name; }
        const std::string_view getHostUserName() const { return _hostUserName; }
        const int64_t GetHostUserId() const { return _hostUserId; }
    private:
        RoomID _id;
        std::string _name;
        int64_t _hostUserId;
        std::string _hostUserName;
        std::unordered_map<userID, RoomUserInfo> _userList;
    };
}

#endif // !__JW_ROOM_H__

