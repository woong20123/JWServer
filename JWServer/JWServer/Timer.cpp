#include "Timer.h"
#include "Logger.h"
#include <ctime>

namespace jw
{
    Timer::Timer() : _expireTick{ 0 }, _executeTick{ 0 }, _intervalMs{ 0 }
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
            LOG_ERROR(L"currentTick != _expireTick, _executeTick:{}, expireTick:{}", _executeTick, _expireTick);
        }

        OnTimer();

        return true;
    }

    void Timer::HandleFailedEvent(AsyncContext* context, paramType param)
    {
        LOG_ERROR(L"failed event, timerId:{} _executeTick:{}, expireTick:{}", context->_id, _executeTick, _expireTick);
    }

    time_t Timer::GetIntervalMs() const
    {
        return _intervalMs;
    }
    void Timer::SetIntervalMs(const time_t intervalMs)
    {
        _intervalMs = intervalMs;
        if (_intervalMs < 0) _intervalMs = 0;
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
