#include "RoomSerializeObject.h"
#include "SerializerInfo.h"

namespace jw
{
    RoomSerializeObject::RoomSerializeObject(int32_t id) : SerializeObject(EXPAND_SERAILIZE_OBJECT_TYPE_ROOM, id)
    {
    }
}