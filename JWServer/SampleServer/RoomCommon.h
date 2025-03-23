#pragma once
#ifndef __JW_ROOM_COMMON_H__
#define __JW_ROOM_COMMON_H__
#include <cstdint>
#include <string>
namespace jw
{
    struct RoomUserInfo
    {
        int64_t _userId;
        std::string _name;
    };

    struct RoomInfo
    {
        int64_t         _roomId;
        std::string     _roomName;
        std::string     _hostUserName;
        int64_t         _hostUserId;
    };

    struct HostInfo
    {
        std::string _name;
        int64_t _userId;
    };

    struct CreatRoomResult
    {
        bool _isSuccess;
        int64_t _roomId;
    };

    enum RoomResult
    {
        ROOM_RESULT_NONE = 0,
        ROOM_RESULT_SUCCESS,
        ROOM_RESULT_NOT_FIND_ROOM,
        ROOM_RESULT_EXIST_USER,
    };

}
#endif // !__JW_ROOM_COMMON_H__