#pragma once
#ifndef __JW_TIMER_H__
#define __JW_TIMER_H__
#include "AsyncObject.h"

namespace jw
{
    class Timer : public AsyncObject
    {
    public:
        Timer();
        uint32_t GetAsyncObjectId() const override;
        bool HandleEvent(AsyncContext* context, paramType nonValue) override;

        virtual void OnTimer() = 0;

        time_t GetIntervalMs() const;
        void SetIntervalMs(const time_t intervalMs);

        time_t GetExcuteTick() const;
        void SetExcuteTick(const time_t executeTick);

        time_t GetExpireTick() const;
        void SetExpireTick(const time_t expireTick);
    private:

        time_t _expireTick;
        time_t _executeTick;
        time_t _intervalMs;
    };
}
#endif // __JW_TIMER_H__

