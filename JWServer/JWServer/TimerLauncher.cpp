#include "TimerLauncher.h"
#include "Timer.h"
#include "Logger.h"
#include "Network.h"
#include "TimeUtil.h"
#include <algorithm>

namespace jw
{
    TimerLauncher::TimerLauncher() : _state{ STATE_NONE }, _timerTick{ 0 }, _timerEventArray{}, _longTermTimerList{}, _timerLogicThread{}, _tickIntervalMilliSecond{ DEFAULT_TIMER_LOGIC_TICK_INTERVAL_MILLISECOND }
    {

    }

    TimerLauncher::~TimerLauncher()
    {
        Stop();

        if (_timerLogicThread.joinable()) _timerLogicThread.join();
    }

    void TimerLauncher::Initialize(const int64_t tickIntervalMilliSecond)
    {
        _tickIntervalMilliSecond = tickIntervalMilliSecond;
    }

    void TimerLauncher::Run()
    {
        _timerLogicThread = std::thread([this]() {
            uint64_t lastTimerTick{ 0 };
            setState(STATE_RUN);

            assert(_tickIntervalMilliSecond > 0);

            LOG_INFO(L"TimerLauncher Thread Run");

            auto baseTimeMs = TimeUtil::GetCurrentTimeMilliSecond();
            int64_t startTimeMs{ 0 }, endTimeMs{ 0 };
            while (true)
            {
                startTimeMs = TimeUtil::GetCurrentTimeMilliSecond();
                if (STATE_STOP == _state)
                {
                    LOG_INFO(L"StopSignal send to Server");
                    break;
                }

                // 현재 틱에 해당하는 타이머 목록을 가져옵니다. 
                std::list<Timer*> tempCurrentTimerList;
                {
                    std::unique_lock<std::shared_mutex> lk(_timerMutex);
                    auto& currentList = getCurrentTimerList();
                    tempCurrentTimerList.splice(tempCurrentTimerList.begin(), currentList);
                    lastTimerTick = _timerTick++;
                }

                postTimerList(tempCurrentTimerList);

                processLongTermTimerList();

                rumTimeCheckAndWait(baseTimeMs);
            }
            LOG_INFO(L"TimerLauncher Thread Stop");
            });
    }

    void TimerLauncher::Stop()
    {
        LOG_INFO(L"TimerLauncher Stop");
        setState(STATE_STOP);
    }

    void TimerLauncher::setState(int8_t state)
    {
        int8_t beforeState = _state;
        if (STATE_STOP == state)
        {
            if (beforeState != STATE_RUN) return;
        }

        _state = state;

        LOG_INFO(L"setState, beforeState:{}, afterState:{}", beforeState, _state);
    }

    int64_t TimerLauncher::GetTickIntervalMillisecond()
    {
        return _tickIntervalMilliSecond;
    }

    int64_t TimerLauncher::GetCurrentTimerTick()
    {
        return _timerTick;
    }

    int64_t TimerLauncher::getNextTimerTickToIndex(int64_t intervalTick)
    {
        if (intervalTick < 0 || DEFAULT_TIMER_MANAGE_MAX_TICK <= intervalTick)
        {
            LOG_ERROR(L"invalid intervalIndex:{}", intervalTick)
                return -1;
        }
        return (_timerTick + intervalTick) % DEFAULT_TIMER_MANAGE_MAX_TICK;
    }

    TimerLauncher::TimerList& TimerLauncher::getCurrentTimerList()
    {
        return _timerEventArray[_timerTick % DEFAULT_TIMER_MANAGE_MAX_TICK];
    }

    void TimerLauncher::RegistTimer(Timer* timer)
    {
        registTimer(timer);
    }

    void TimerLauncher::registTimer(Timer* timer)
    {
        std::unique_lock<std::shared_mutex> lk(_timerMutex);
        const auto currentTick = _timerTick;

        // 타이머의 expireMs가 DEFAULT_TIMER_LOGIC_INTERVAL_MILLISECOND 보다 작다면 가장 최근 타이머에 등록
        const auto intervalTick = timer->GetIntervalMs() <= GetTickIntervalMillisecond() ?
            0 : static_cast<int32_t>((timer->GetIntervalMs() / GetTickIntervalMillisecond()) - 1);

        timer->SetExpireTick(currentTick + intervalTick);

        if (intervalTick < DEFAULT_TIMER_MANAGE_MAX_TICK)
        {
            const auto currentIndex = getNextTimerTickToIndex(intervalTick);
            _timerEventArray[currentIndex].push_back(timer);
        }
        else
        {
            _longTermTimerList.push_back(timer);
        }

    }

    void TimerLauncher::registLongTermTimer(Timer* timer)
    {
        std::unique_lock<std::shared_mutex> lk(_timerMutex);
        const auto tickOfRemainUntilExpire = timer->GetExpireTick() - _timerTick;
        if (tickOfRemainUntilExpire < DEFAULT_TIMER_MANAGE_MAX_TICK)
        {
            const auto currentIndex = getNextTimerTickToIndex(tickOfRemainUntilExpire);
            _timerEventArray[currentIndex].push_back(timer);
        }
        else
        {
            _longTermTimerList.push_back(timer);
        }
    }

    void TimerLauncher::postTimerList(std::list<Timer*>& timerList)
    {
        // 가져온 타이머들을 IOCP에 Post합니다. 
        if (!timerList.empty())
        {
            std::for_each(std::begin(timerList), std::end(timerList), [](Timer* timer) {
                timer->SetExcuteTick(timer->GetExpireTick());
                ::PostQueuedCompletionStatus(NETWORK().GetIOCPHandle(), 0, (ULONG_PTR)timer, nullptr);
                });
            timerList.clear();
        }
    }

    void TimerLauncher::processLongTermTimerList()
    {
        // DEFAULT_TIMER_MANAGE_MAX_TICK를 기준으로 순회를 마쳤다면 
        // longTermTimerList의 tick을 DEFAULT_TIMER_MANAGE_MAX_TICK만큼 차감하여 다시 등록합니다. 
        if (_timerTick >= DEFAULT_TIMER_MANAGE_MAX_TICK && _timerTick % DEFAULT_TIMER_MANAGE_MAX_TICK == 0)
        {
            std::list<Timer*> tempTimerList;
            {
                std::unique_lock<std::shared_mutex> lk(_timerMutex);
                tempTimerList.splice(std::begin(tempTimerList), _longTermTimerList);
            }

            if (!tempTimerList.empty())
            {
                std::for_each(std::begin(tempTimerList), std::end(tempTimerList), [this](Timer* timer) {
                    registLongTermTimer(timer);
                    });
                tempTimerList.clear();
            }

        }
    }

    void TimerLauncher::rumTimeCheckAndWait(int64_t& baseTimeMs)
    {
        int64_t endTimeMs = TimeUtil::GetCurrentTimeMilliSecond();
        int64_t diffTimeMs = endTimeMs - baseTimeMs;
        int64_t adjustedIntervalMs = _tickIntervalMilliSecond - diffTimeMs;

        // baseTimeMs는 틱마다 tickIntervalMilliSecond만큼 증가 시켜 고정 기준 값을 유지 합니다. 
        baseTimeMs += _tickIntervalMilliSecond;

        if (adjustedIntervalMs > 0)
            std::this_thread::sleep_for(std::chrono::milliseconds(adjustedIntervalMs));
        else
            LOG_WARN(L"timerEvent diffTimeMs:{}, adjustedIntervalMs:{}, baseTimeMs:{}, endTimeMs:{}, tickIntervalMilliSecond:{}", diffTimeMs, adjustedIntervalMs, baseTimeMs, endTimeMs, _tickIntervalMilliSecond);
    }
}
