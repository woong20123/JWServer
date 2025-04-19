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
        // 한 틱이 가지는 기본 시간 값( 100ms )
        static constexpr int32_t   DEFAULT_TIMER_LOGIC_TICK_INTERVAL_MILLISECOND = 100;

        // 관리되는 최대 타이머 틱
        static constexpr int32_t   DEFAULT_TIMER_MANAGE_MAX_TICK = 600;

        using TimerList = std::list<Timer*>;
        using TimerListArray = std::array<TimerList, DEFAULT_TIMER_MANAGE_MAX_TICK>;


        void Initialize(const int32_t tickIntervalMilliSecond);
        void Run();
        void Stop();

        void RegistTimer(Timer* timer);

        virtual int32_t GetTickIntervalMillisecond();

        virtual int64_t GetCurrentTimerTick();
        int32_t GetTickIntervalMilliSecond() const { return _tickIntervalMilliSecond; }
        int32_t GetLongTermCheckTimeMilliSecond() const { return _tickIntervalMilliSecond * DEFAULT_TIMER_MANAGE_MAX_TICK; }
    protected:
        TimerLauncher();
        ~TimerLauncher();
        TimerLauncher(const TimerLauncher&) = delete;
        TimerLauncher& operator=(const TimerLauncher&) = delete;
    private:
        void registTimer(Timer* timer);
        void registLongTermTimer(Timer* timer);

        int32_t getNextTimerTickToIndex(int32_t intervalIndex);
        int32_t getLastTimerTickToIndex();
        int32_t getCurrentTimerTickToIndex();

        friend class Singleton<TimerLauncher>;

        bool                                    _isRun;
        uint64_t                                _timerTick;
        std::shared_mutex                       _timerMutex;
        TimerListArray                          _timerEventArray;
        std::list<Timer*>                       _longTermTimerList;
        std::thread                             _timerLogicThread;
        int32_t                                 _tickIntervalMilliSecond;
    };
}

#define TIMER_LAUNCHER jw::TimerLauncher::GetInstance
#endif // !__JW_TIMER_LAUNCHER_H__
