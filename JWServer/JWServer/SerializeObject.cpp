#include "SerializeObject.h"
#include <ctime>
#include "TimeUtil.h"

namespace jw
{
    SerializeObject::SerializeObject(int32_t type, int32_t id) : _type{ type }, _id{ id }, _registMilliSeconds{ 0 }, _executeTick{ 0 }, _serializerId{ 0 }, _delayMilliSeconds{ 0 }

    {
    }

    SerializeObject::~SerializeObject()
    {
    }

    void SerializeObject::Initialize(const int32_t delayMilliSeconds, const time_t tickIntervalMilliSecond)
    {
        auto now = std::chrono::system_clock::now();
        _delayMilliSeconds = delayMilliSeconds;
        _registMilliSeconds = TimeUtil::GetCurrentTimeMilliSecond();
        _executeTick = delayMilliSeconds / tickIntervalMilliSecond;
    }

}