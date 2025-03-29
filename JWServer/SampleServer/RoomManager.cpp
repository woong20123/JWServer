#include "RoomManager.h"
#include "Logger.h"
#include "Room.h"
#include "StringConverter.h"
#include "TypeDefinition.h"
#include <ranges>

namespace jw
{
    RoomManager::RoomManager() : _roomIdIssuer{ 1 }
    {
    }

    RoomManager::~RoomManager()
    {
    }

    CreatRoomResult RoomManager::CreateRoom(const std::string& roomName, const HostInfo& hostInfo)
    {
        auto roomId = _roomIdIssuer.fetch_add(1);

        std::shared_ptr<Room> room = std::make_shared<Room>();
        room->Initialize(roomId, roomName, hostInfo._userId, hostInfo._name);
        room->AddUser(hostInfo._userId, hostInfo._name);
        {
            WRITE_LOCK(_roomListMutex);
            if (!_roomList.insert({ roomId, room }).second)
            {
                LOG_ERROR(L"CreateRoom failed, roomId:{}, roomName:{}", roomId, StringConverter::StrA2WUseUTF8(roomName)->c_str());
                return CreatRoomResult{ false, -1 };
            }
        }
        room->OnCreate();
        LOG_DEBUG(L"CreateRoom, roomId:{}, roomName:{}", roomId, StringConverter::StrA2WUseUTF8(roomName)->c_str());
        return CreatRoomResult{ true, roomId };
    }

    std::shared_ptr<Room> RoomManager::findRoom(const RoomID roomId) const
    {
        READ_LOCK(_roomListMutex);
        auto roomIter = _roomList.find(roomId);
        if (_roomList.end() == roomIter)
        {
            return std::shared_ptr<Room>{};
        }
        return roomIter->second;
    }

    bool RoomManager::IsExistRoom(const RoomID roomId) const
    {
        return findRoom(roomId) != nullptr;
    }

    SerializerKey RoomManager::GetRoomSerializerKey(const RoomID roomId) const
    {
        const auto& room = findRoom(roomId);
        if (!room) return SerializerKey::INVALID_KEY();
        return room->GetSerializerKey();
    }

    std::vector<RoomInfo> RoomManager::GetRoomList() const
    {
        std::vector<RoomInfo> roomList;
        {
            READ_LOCK(_roomListMutex);
            for (const auto& roomInfo : _roomList | std::views::take(10))
            {
                const auto& room = roomInfo.second;
                roomList.push_back({ roomInfo.first, room->GetRoomName().data(), room->getHostUserName().data(), room->GetHostUserId() });
            }
        }

        return roomList;
    }

    bool RoomManager::HasMembers(const RoomID roomId) const
    {
        const auto& room = findRoom(roomId);
        if (!room) return false;
        return room->HasMembers();
    }

    std::vector<RoomManager::RoomID> RoomManager::GetRoomMemberIds(RoomID roomId) const
    {
        const auto& room = findRoom(roomId);
        if (!room) return {};
        return room->GetMemberIds();
    }

    std::vector<RoomUserInfo> RoomManager::GetRoomMemberInfoList(RoomID roomId) const
    {
        const auto& room = findRoom(roomId);
        if (!room) return {};
        return room->GetMemberInfoList();
    }

    RoomInfo RoomManager::GetRoomInfo(const RoomID roomId) const
    {
        const auto& room = findRoom(roomId);
        if (!room) return {};
        return { room->GetRoomInfo() };
    }

    RoomResult RoomManager::EnterRoom(const RoomID roomId, const int64_t userKey, const std::string& userName)
    {
        const auto room = findRoom(roomId);
        if (!room)
        {
            return ROOM_RESULT_NOT_FIND_ROOM;
        }

        if (room->IsExistUser(userKey))
        {
            return ROOM_RESULT_EXIST_USER;
        }

        room->AddUser(userKey, userName);
        return ROOM_RESULT_SUCCESS;
    }

    RoomResult RoomManager::LeaveRoom(const RoomID roomId, const int64_t userKey)
    {
        const auto room = findRoom(roomId);
        if (!room)
        {
            return ROOM_RESULT_NOT_FIND_ROOM;
        }

        if (!room->IsExistUser(userKey))
        {
            return ROOM_RESULT_EXIST_USER;
        }

        room->RemoveUser(userKey);

        if (room->IsEmpty())
        {
            destoryRoom(roomId);
        }

        return ROOM_RESULT_SUCCESS;
    }

    bool RoomManager::destoryRoom(const int64_t roomId)
    {
        bool result{ false };
        const auto& room = findRoom(roomId);
        if (!room) return false;

        {
            WRITE_LOCK(_roomListMutex);
            result = _roomList.erase(roomId);
        }
        room->OnDestoroy();

        LOG_DEBUG(L"DestoryRoom, roomId:{}", roomId);

        return result;
    }
}