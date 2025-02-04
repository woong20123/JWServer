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
        bool HandleEvent(AsyncContext* context, paramType param) override;

        virtual void OnTimer() = 0;

        time_t GetExpireMs() const;
        void SetExpireMs(const time_t expireMs);
        void AddExpireMs(const time_t expireMs);

        time_t GetExpireTick() const;
        void SetExpireTick(const time_t expireTick);
    private:

        time_t _expireTick;
        time_t _expireMs;
    };
}
#endif // __JW_TIMER_H__

