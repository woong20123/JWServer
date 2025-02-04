#include "SerializeObject.h"
#include <ctime>

namespace jw
{
    SerializeObject::SerializeObject(int id) : _id{ id }, _registMilliSeconds{ 0 }, _executeTick{ 0 }, _serializerId{ 0 }, _delayMilliSeconds{ 0 }

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


    void LambdaSerializeObject::Execute()
    {
        _func();
    }
}