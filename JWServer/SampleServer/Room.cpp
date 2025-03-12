#include "Room.h"
namespace jw
{
    Room::Room()
    {
        _userList.clear();
    }

    Room::~Room()
    {
    }

    void Room::Initialize(RoomID id, const std::string& name)
    {
        _id = id;
        _name = name;
    }

}