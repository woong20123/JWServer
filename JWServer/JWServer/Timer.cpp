#include "Timer.h"
#include "Logger.h"
#include <ctime>

namespace jw
{
    Timer::Timer() : _expireTick(0), _expireMs(0)
    {}

    uint32_t Timer::GetAsyncObjectId() const
    {
        return ASYNC_OBJ_TIMER;
    }
    bool Timer::HandleEvent(AsyncContext* context, paramType nonValue)
    {
        if (_executeTick != _expireTick)
        {
            // 틱에 오차가 있습니다. 
            LOG_WARN(L"currentTick != _expireTick, _executeTick:{}, expireTick:{}", _executeTick, _expireTick);
        }

        OnTimer();

        return true;
    }

    time_t Timer::GetExpireMs() const
    {
        return _expireMs;
    }

    void Timer::SetExpireMs(const time_t expireMs)
    {
        _expireMs = expireMs;
    }

    void Timer::AddExpireMs(const time_t expireMs)
    {
        _expireMs += expireMs;
        if (_expireMs < 0)
        {
            _expireMs = 0;
        }
    }

    time_t Timer::GetExcuteTick() const
    {
        return _executeTick;
    }
    void Timer::SetExcuteTick(const time_t executeTick)
    {
        _executeTick = executeTick;
    }

    time_t Timer::GetExpireTick() const
    {
        return _expireTick;
    }

    void Timer::SetExpireTick(const time_t expireTick)
    {
        _expireTick = expireTick;
    }
}
