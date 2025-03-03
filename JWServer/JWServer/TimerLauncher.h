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
        static constexpr int32_t   DEFAULT_TIMER_LOGIC_INTERVAL_MILLISECOND = 100;
        // 관리되는 최대 타이머 틱
        // 등록하는 타이머의 틱이 해당 값 보다 크다면 라스트 틱에 타이머를 등록한 후 틱을 감소 시킵니다. 
        // 다음의 작업을 반복해서 동작할 수 있도록 구성합니다. 
        static constexpr int32_t   DEFAULT_TIMER_MANAGE_MAX_TICK = 600;
        static constexpr int32_t   LONGTERM_CHECK_TIME_MILLISECOND = DEFAULT_TIMER_LOGIC_INTERVAL_MILLISECOND * DEFAULT_TIMER_MANAGE_MAX_TICK;

        using TimerList = std::list<Timer*>;
        using TimerListArray = std::array<TimerList, DEFAULT_TIMER_MANAGE_MAX_TICK>;

        void Initialize();
        void Run();
        void Stop();

        void RegistTimer(Timer* timer);

        virtual int32_t GetTimerLogicInterval();

        virtual int64_t GetCurrentTimerTick();
        virtual int32_t GetNextTimerTickToIndex(int32_t intervalIndex);
        virtual int32_t GetCurrentTimerTickToIndex();
        virtual int32_t GetLastTimerTickToIndex();
    protected:
        TimerLauncher();
        ~TimerLauncher();
        TimerLauncher(const TimerLauncher&) = delete;
        TimerLauncher& operator=(const TimerLauncher&) = delete;
    private:
        void registTimer(Timer* timer);

        friend class Singleton<TimerLauncher>;

        bool                                    _isRun;
        std::atomic<uint64_t>                   _timerTick;
        uint64_t                                _lastTimerTick;
        std::shared_mutex                       _timerMutex;
        TimerListArray                          _timerEventArray;
        std::list<Timer*>                       _longTermTimerList;
        std::thread                             _timerLogicThread;
    };
}

#define TIMER_LAUNCHER jw::TimerLauncher::GetInstance
#endif // !__JW_TIMER_LAUNCHER_H__
