#include "Room.h"
#include "SampleServer.h"
#include "SerializerManager.h"
#include "WorldSerializeObject.h"
#include "TimerLauncher.h"
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
        _serializer = std::make_shared<Serializer>(serializerKey, GetTimerLauncher().GetTickIntervalMilliSecond());
        SERIALIZER_MANAGER().RegistSerializer(serializerKey, _serializer);
    }

    SerializerKey Room::GetSerializerKey() const
    {
        return *_serializerKey;
    }

    bool Room::AddUser(const userID userId, const std::string& name)
    {
        if (_userList.contains(userId)) return false;

        _userList[userId] = RoomUserInfo{ ._userId = userId, ._name = name };
        return true;
    }

    bool Room::RemoveUser(const userID userId)
    {
        return _userList.erase(userId) == 1 ? true : false;
    }

    bool Room::IsEmpty() const
    {
        return _userList.empty();
    }

    std::vector<Room::userID> Room::GetMemberIds() const
    {
        std::vector<userID> memberIds;
        for (auto& [userKey, roomUserInfo] : std::as_const(_userList))
        {
            memberIds.emplace_back(userKey);
        }
        return memberIds;
    }

    std::vector<RoomUserInfo> Room::GetMemberInfoList() const
    {
        std::vector<RoomUserInfo> memberInfoList;
        for (auto& [userKey, roomUserInfo] : std::as_const(_userList))
        {
            memberInfoList.emplace_back(roomUserInfo);
        }
        return memberInfoList;
    }

    void Room::OnCreate()
    {
        std::shared_ptr<CreateRoomNotifyTask> createRoomNotifyTask = std::make_shared<CreateRoomNotifyTask>();
        createRoomNotifyTask->Initialize(_id, _name, _hostUserId, _hostUserName);

        auto worldSerializerKey = SAMPLE_SERVER().GetWorld()->GetSerializerKey();
        if (!SERIALIZER_MANAGER().Post(worldSerializerKey, createRoomNotifyTask))
        {
            LOG_ERROR(L"Fail DestoroyRoomTask Post");
        }
    }
    void Room::OnDestoroy()
    {
        std::shared_ptr<DestoroyRoomNotifyTask> destoroyRoomTask = std::make_shared<DestoroyRoomNotifyTask>();
        destoroyRoomTask->Initialize(_id);

        auto worldSerializerKey = SAMPLE_SERVER().GetWorld()->GetSerializerKey();
        if (!SERIALIZER_MANAGER().Post(worldSerializerKey, destoroyRoomTask))
        {
            LOG_ERROR(L"Fail DestoroyRoomTask Post");
        }
    }
}