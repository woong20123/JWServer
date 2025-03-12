#pragma once

#ifndef __JW_ROOM_MANAGER_H__
#define __JW_ROOM_MANAGER_H__
#include <atomic>
#include <string>
#include <map>
#include <memory>
#include <vector>

namespace jw
{
    class Room;
    struct RoomInfo
    {
        int64_t _roomId;
        std::string _roomName;
    };

    struct CreatRoomResult
    {
        bool _isSuccess;
        int64_t _roomId;
    };

    class RoomManager
    {
    public:
        using RoomID = int64_t;
        RoomManager();
        virtual ~RoomManager();

        CreatRoomResult CreateRoom(const std::string& roomName);
        std::vector<RoomInfo> GetRoomList();

    private:
        std::atomic<RoomID> _roomIdIssuer;
        std::map<RoomID, std::shared_ptr<Room> > _roomList;
    };
}

#endif // __JW_ROOM_MANAGER_H__