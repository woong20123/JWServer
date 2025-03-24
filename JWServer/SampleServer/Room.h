#pragma once
#ifndef __JW_ROOM_H__
#define __JW_ROOM_H__
#include <cstdint>
#include <unordered_map>
#include <string>
#include "Serializer.h"
#include "RoomCommon.h"

namespace jw
{
    class Room : AttachedSerializerObject
    {
    public:
        using userID = int64_t;
        using RoomID = int64_t;

        Room();
        ~Room();

        void Initialize(const RoomID id, const std::string& name, const int64_t hostUserId, const std::string& hostUserName);
        bool AddUser(const userID userId, const std::string& name);
        bool RemoveUser(const userID userId);
        bool IsExistUser(const userID userId) const { return _userList.contains(userId); }
        SerializerKey GetSerializerKey() const override;
        std::string_view GetRoomName() const { return _name; }
        std::string_view getHostUserName() const { return _hostUserName; }
        int64_t GetHostUserId() const { return _hostUserId; }
        std::vector<userID> GetMemberIds() const;
        bool HasMembers() const { return !_userList.empty(); }
        std::vector<RoomUserInfo> GetMemberInfoList() const;
        RoomInfo GetRoomInfo() const { return { _id, _name, _hostUserName, _hostUserId }; }
    private:
        RoomID _id;
        std::string _name;
        int64_t _hostUserId;
        std::string _hostUserName;
        std::unordered_map<userID, RoomUserInfo> _userList;
        std::shared_ptr<Serializer> _serializer;
        std::unique_ptr<SerializerKey> _serializerKey;
    };
}

#endif // !__JW_ROOM_H__

