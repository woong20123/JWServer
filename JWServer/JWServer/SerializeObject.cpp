#include "SerializeObject.h"
#include <ctime>

namespace jw
{
    SerializeObject::SerializeObject(int32_t type, int32_t id) : _type{ type }, _id{ id }, _registMilliSeconds{ 0 }, _executeTick{ 0 }, _serializerId{ 0 }, _delayMilliSeconds{ 0 }

    {
    }

    SerializeObject::~SerializeObject()
    {
    }

    void SerializeObject::Initialize(int32_t delayMilliSeconds)
    {
        _delayMilliSeconds = delayMilliSeconds;
        _registMilliSeconds = std::time(nullptr);
        _executeTick = delayMilliSeconds / 100;
    }

}