#include "Room.h"
#include "SampleServer.h"
#include "SerializerManager.h"
#include "User.h"

namespace jw
{
    Room::Room()
    {
        _userList.clear();
    }

    Room::~Room()
    {
    }

    void Room::Initialize(const RoomID id, const std::string& name, const int64_t hostUserId, const std::string& hostUserName)
    {
        _id = id;
        _name = name;
        _hostUserId = hostUserId;
        _hostUserName = hostUserName;

        // WorldSerializer µî·Ï 
        SerializerKey serializerKey{ SERIALIZER_TYPE_ROOM, SERIALIZER_MANAGER().MakeSerializeId(SERIALIZER_TYPE_ROOM) };
        _serializerKey = std::make_unique<SerializerKey>(serializerKey);
        _serializer = std::make_shared<Serializer>(serializerKey);
        SERIALIZER_MANAGER().RegistSerializer(serializerKey, _serializer);
    }

    SerializerKey Room::GetSerializerKey() const
    {
        return *_serializerKey;
    }

    bool Room::AddUser(const userID userId, const std::string& name)
    {
        if (_userList.contains(userId)) return false;

        RoomUserInfo userInfo;
        userInfo._userId = userId;
        userInfo._name = name;
        _userList[userId] = userInfo;
        return true;
    }

    bool Room::RemoveUser(const userID userId)
    {
        return _userList.erase(userId) == 1 ? true : false;
    }

    std::vector<Room::userID> Room::GetMemberIds() const
    {
        std::vector<userID> memberIds;
        for (const auto& user : _userList)
        {
            memberIds.push_back(user.first);
        }
        return memberIds;
    }

    std::vector<RoomUserInfo> Room::GetMemberInfoList() const
    {
        std::vector<RoomUserInfo> memberInfoList;
        for (const auto& user : _userList)
        {
            memberInfoList.push_back(user.second);
        }
        return memberInfoList;
    }
}