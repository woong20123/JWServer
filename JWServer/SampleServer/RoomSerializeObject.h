#pragma once
#ifndef __JW_ROOM_SERIALIZE_OBJECT_H__
#define __JW_ROOM_SERIALIZE_OBJECT_H__
#include "SerializeObject.h"

namespace jw
{
    class RoomSerializeObject : public SerializeObject
    {
    public:
        RoomSerializeObject(int32_t id);
    protected:
        virtual void Execute() = 0;
    };
}
#endif // !__JW_ROOM_SERIALIZE_OBJECT_H__

