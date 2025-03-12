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

    CreatRoomResult RoomManager::CreateRoom(const std::string& roomName)
    {
        auto roomId = _roomIdIssuer.fetch_add(1);

        std::shared_ptr<Room> room = std::make_shared<Room>();
        room->Initialize(roomId, roomName);

        if (!_roomList.insert({ roomId, room }).second)
        {
            LOG_ERROR(L"CreateRoom failed, roomId:{}, roomName:{}", roomId, StringConverter::StrA2WUseUTF8(roomName)->c_str());
            return CreatRoomResult{ false, -1 };
        }
        LOG_DEBUG(L"CreateRoom, roomId:{}, roomName:{}", roomId, StringConverter::StrA2WUseUTF8(roomName)->c_str());
        return CreatRoomResult{ true, roomId };
    }

    std::vector<RoomInfo> RoomManager::GetRoomList()
    {
        std::vector<RoomInfo> roomList;

        for (const auto& roomInfo : _roomList | std::views::take(10))
        {
            roomList.push_back({ roomInfo.first, roomInfo.second->GetRoomName() });
        }
        return roomList;
    }
}