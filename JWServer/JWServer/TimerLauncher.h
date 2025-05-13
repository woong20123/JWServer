#pragma once
#ifndef __JW_TIMER_LAUNCHER_H__
#define __JW_TIMER_LAUNCHER_H__
#include <cstdint>
#include "Singleton.hpp"
#include <atomic>
#include <shared_mutex>
#include <list>
#include <array>

namespace jw
{
    class Timer;
    class TimerLauncher : public Singleton<TimerLauncher>
    {
    public:
        enum
        {
            STATE_NONE,
            STATE_RUN,
            STATE_STOP
        };
        // 한 틱이 가지는 기본 시간 값( 100ms )
        static constexpr int64_t   DEFAULT_TIMER_LOGIC_TICK_INTERVAL_MILLISECOND = 100;

        // 관리되는 최대 타이머 틱
        static constexpr int64_t   DEFAULT_TIMER_MANAGE_MAX_TICK = 600;

        using TimerList = std::list<Timer*>;
        using TimerListArray = std::array<TimerList, DEFAULT_TIMER_MANAGE_MAX_TICK>;

        TimerLauncher(const TimerLauncher&) = delete;
        TimerLauncher& operator=(const TimerLauncher&) = delete;

        void Initialize(const int64_t tickIntervalMilliSecond);
        void Run();
        void Stop();

        void RegistTimer(Timer* timer);

        virtual int64_t GetTickIntervalMillisecond();

        virtual int64_t GetCurrentTimerTick();
        int64_t GetTickIntervalMilliSecond() const { return _tickIntervalMilliSecond; }
        int64_t GetLongTermCheckTimeMilliSecond() const { return _tickIntervalMilliSecond * DEFAULT_TIMER_MANAGE_MAX_TICK; }
    protected:
        TimerLauncher();
        ~TimerLauncher();
    private:
        void registTimer(Timer* timer);
        void registLongTermTimer(Timer* timer);

        void postTimerList(std::list<Timer*>& timerList);
        void processLongTermTimerList();
        void rumTimeCheckAndWait(int64_t& baseTimeMs);

        int64_t getNextTimerTickToIndex(int64_t intervalIndex);
        TimerList& getCurrentTimerList();

        void setState(int8_t state);

        friend class Singleton<TimerLauncher>;

        uint8_t                                 _state;
        std::thread                             _timerLogicThread;

        uint64_t                                _timerTick;

        std::shared_mutex                       _timerMutex;
        TimerListArray                          _timerEventArray;
        std::list<Timer*>                       _longTermTimerList;

        int64_t                                 _tickIntervalMilliSecond;
    };
}

#define TIMER_LAUNCHER jw::TimerLauncher::GetInstance
#endif // !__JW_TIMER_LAUNCHER_H__
