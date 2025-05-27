#include "SerializeObject.h"
#include "TimeUtil.h"

namespace jw
{
    SerializeObject::SerializeObject(int32_t type, int32_t id) : _type{ type }, _id{ id }, _registMilliSeconds{ 0 }, _delayTick{ 0 }, _serializerId{ 0 }, _delayMilliSeconds{ 0 }

    {
    }

    void SerializeObject::Initialize(const int64_t delayMilliSeconds, const time_t tickIntervalMilliSecond)
    {
        _intervalMilliSeconds = tickIntervalMilliSecond;
        _delayMilliSeconds = delayMilliSeconds;
        _registMilliSeconds = TimeUtil::GetCurrentTimeMilliSecond();
        _delayTick = _delayMilliSeconds / _intervalMilliSeconds;
    }

    void SerializeObject::ReCalculateTick()
    {
        const auto currentMilliSecond = TimeUtil::GetCurrentTimeMilliSecond();
        const auto diffMilliSecond = currentMilliSecond - _registMilliSeconds;

        _registMilliSeconds = currentMilliSecond;
        _delayMilliSeconds -= diffMilliSecond;
        if (_delayMilliSeconds < 0) {
            _delayMilliSeconds = 0;
            _delayTick = 0;
            return;
        }
        _delayTick = _delayMilliSeconds / _intervalMilliSeconds;
    }

}