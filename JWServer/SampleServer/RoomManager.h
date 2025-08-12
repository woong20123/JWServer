#pragma once

#ifndef __JW_ROOM_MANAGER_H__
#define __JW_ROOM_MANAGER_H__
#include <atomic>
#include <string>
#include <map>
#include <memory>
#include <vector>
#include <shared_mutex>
#include "RoomCommon.h"
#include "Serializer.h"

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

        bool IsExistRoom(const RoomID roomId) const;
        SerializerKey GetRoomSerializerKey(const RoomID roomId) const;
        std::vector<RoomInfo> GetRoomList() const;
        bool HasMembers(const RoomID roomId) const;
        std::vector<RoomID> GetRoomMemberIds(RoomID roomId) const;
        std::vector<RoomUserInfo> GetRoomMemberInfoList(RoomID roomId) const;
        RoomInfo GetRoomInfo(const RoomID roomId) const;

        RoomResult EnterRoom(const RoomID roomId, const int64_t userKey, const std::string& userName);
        RoomResult LeaveRoom(const RoomID roomId, const int64_t userKey);

        void ShutDown();

    private:
        bool destoryRoom(const int64_t roomId);
        std::shared_ptr<Room> findRoom(const RoomID roomId) const;
        std::vector<RoomID>   getAllRoomIds() const;
        std::atomic<RoomID>                         _roomIdIssuer;
        mutable std::shared_mutex                   _roomListMutex;
        std::map<RoomID, std::shared_ptr<Room> >    _roomList;

    };
}

#endif // __JW_ROOM_MANAGER_H__