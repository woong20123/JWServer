#include "RoomManager.h"
#include "Logger.h"
#include "Room.h"
#include "StringConverter.h"
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

        if (!_roomList.insert({ roomId, room }).second)
        {
            LOG_ERROR(L"CreateRoom failed, roomId:{}, roomName:{}", roomId, StringConverter::StrA2WUseUTF8(roomName)->c_str());
            return CreatRoomResult{ false, -1 };
        }
        LOG_DEBUG(L"CreateRoom, roomId:{}, roomName:{}", roomId, StringConverter::StrA2WUseUTF8(roomName)->c_str());
        return CreatRoomResult{ true, roomId };
    }

    std::shared_ptr<Room> RoomManager::FindRoom(const RoomID roomId) const
    {
        auto roomIter = _roomList.find(roomId);
        if (_roomList.end() == roomIter)
        {
            return std::shared_ptr<Room>{};
        }
        return roomIter->second;
    }

    std::vector<RoomInfo> RoomManager::GetRoomList() const
    {
        std::vector<RoomInfo> roomList;

        for (const auto& roomInfo : _roomList | std::views::take(10))
        {
            const auto& room = roomInfo.second;
            roomList.push_back({ roomInfo.first, room->GetRoomName().data(), room->getHostUserName().data(), room->GetHostUserId() });
        }
        return roomList;
    }

    std::vector<RoomManager::RoomID> RoomManager::GetRoomMemberIds(RoomID roomId) const
    {
        auto roomIter = _roomList.find(roomId);
        if (_roomList.end() == roomIter)
        {
            return {};
        }
        return roomIter->second->GetMemberIds();
    }

    std::vector<RoomUserInfo> RoomManager::GetRoomMemberInfoList(RoomID roomId) const
    {
        auto roomIter = _roomList.find(roomId);
        if (_roomList.end() == roomIter)
        {
            return {};
        }
        return roomIter->second->GetMemberInfoList();
    }

    RoomInfo RoomManager::GetRoomInfo(const RoomID roomId) const
    {
        auto roomIter = _roomList.find(roomId);
        if (_roomList.end() == roomIter)
        {
            return {};
        }
        const auto& room = roomIter->second;
        return { room->GetRoomInfo() };
    }

    RoomResult RoomManager::EnterRoom(const RoomID roomId, const int64_t userKey, const std::string& userName)
    {
        const auto room = FindRoom(roomId);
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

        const auto room = FindRoom(roomId);
        if (!room)
        {
            return ROOM_RESULT_NOT_FIND_ROOM;
        }

        if (!room->IsExistUser(userKey))
        {
            return ROOM_RESULT_EXIST_USER;
        }

        room->RemoveUser(userKey);

        return ROOM_RESULT_SUCCESS;
    }
}