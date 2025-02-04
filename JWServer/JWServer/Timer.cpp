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
    bool Timer::HandleEvent(AsyncContext* context, paramType currentTick)
    {
        if (currentTick != _expireTick)
        {
            // 데이터 오류가 있습니다. 
            LOG_ERROR(L"currentTick != _expireTick, currentTick:{}, expireTick:{}", currentTick, _expireTick);
            return true;
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

    time_t Timer::GetExpireTick() const
    {
        return _expireTick;
    }

    void Timer::SetExpireTick(const time_t expireTick)
    {
        _expireTick = expireTick;
    }
}
