#include "Room.h"
#include "SampleServer.h"
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
    }

}