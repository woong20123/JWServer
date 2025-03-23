#pragma once

#ifndef __JW_ROOM_MANAGER_H__
#define __JW_ROOM_MANAGER_H__
#include <atomic>
#include <string>
#include <map>
#include <memory>
#include <vector>
#include "RoomCommon.h"

namespace jw
{
    class Room;
    class RoomManager
    {
    public:
        using RoomID = int64_t;
        RoomManager();
        virtual ~RoomManager();

        CreatRoomResult CreateRoom(const std::string& roomName, const HostInfo& hostInfo);
        std::shared_ptr<Room> FindRoom(const RoomID roomId) const;
        std::vector<RoomInfo> GetRoomList() const;
        std::vector<RoomID> GetRoomMemberIds(RoomID roomId) const;
        std::vector<RoomUserInfo> GetRoomMemberInfoList(RoomID roomId) const;
        RoomInfo GetRoomInfo(const RoomID roomId) const;

        RoomResult EnterRoom(const RoomID roomId, const int64_t userKey, const std::string& userName);
        RoomResult LeaveRoom(const RoomID roomId, const int64_t userKey);

    private:
        std::atomic<RoomID> _roomIdIssuer;
        std::map<RoomID, std::shared_ptr<Room> > _roomList;
    };
}

#endif // __JW_ROOM_MANAGER_H__