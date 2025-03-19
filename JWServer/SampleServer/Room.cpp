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

    void Room::Initialize(RoomID id, const std::string& name, const int64_t hostUserId, const std::string& hostUserName)
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

    void Room::AddUser(userID userId, const std::string& name)
    {
        RoomUserInfo userInfo;
        userInfo._name = name;
        _userList[userId] = userInfo;
    }

    const std::vector<Room::userID> Room::GetMemberIds() const
    {
        std::vector<userID> memberIds;
        for (const auto& user : _userList)
        {
            memberIds.push_back(user.first);
        }
        return memberIds;
    }
}