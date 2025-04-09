#include "TimerLauncher.h"
#include "Timer.h"
#include "Logger.h"
#include "Network.h"

namespace jw
{
    TimerLauncher::TimerLauncher() : _isRun{ false }, _timerTick{ 0 }, _lastTimerTick{ 0 }, _timerEventArray{}, _longTermTimerList{}, _timerLogicThread{}, _intervalMilliSecond{ DEFAULT_TIMER_LOGIC_INTERVAL_MILLISECOND }
    {

    }

    TimerLauncher::~TimerLauncher()
    {
        Stop();
        _timerLogicThread.join();
    }

    void TimerLauncher::Initialize(const int32_t intervalMilliSecond)
    {
        _intervalMilliSecond = intervalMilliSecond;
    }

    void TimerLauncher::Run()
    {
        _timerLogicThread = std::thread([this]() {
            time_t runTimeMs{ 0 };
            time_t remainIntervalMs{ 0 };
            _isRun = true;

            LOG_INFO(L"TimerLauncher Run");

            while (true)
            {
                if (!_isRun)
                {
                    LOG_INFO(L"StopSignal send to Server");
                    break;
                }

                const auto startTimeMs = ::time(nullptr);

                // 현재 틱에 해당하는 타이머 목록을 가져옵니다. 
                std::list<Timer*> timerList;
                {
                    std::unique_lock<std::shared_mutex> lk(_timerMutex);
                    const auto currentIndex = GetCurrentTimerTickToIndex();
                    TimerList& currentList = _timerEventArray[currentIndex];
                    timerList.splice(timerList.begin(), currentList);
                    _lastTimerTick = _timerTick.fetch_add(1);
                }


                // 가져온 타이머들을 IOCP에 Post합니다. 
                if (!timerList.empty())
                {
                    for (auto& timer : timerList)
                    {
                        timer->SetExcuteTick(_lastTimerTick);
                        ::PostQueuedCompletionStatus(NETWORK().GetIOCPHandle(), 0, (ULONG_PTR)timer, nullptr);
                    }
                    timerList.clear();
                }

                // DEFAULT_TIMER_MANAGE_MAX_TICK를 기준으로 순회를 마쳤다면 
                // longTermTimerList의 tick을 DEFAULT_TIMER_MANAGE_MAX_TICK만큼 차감하여 다시 등록합니다. 
                if (_lastTimerTick % DEFAULT_TIMER_MANAGE_MAX_TICK == 0)
                {
                    {
                        std::unique_lock<std::shared_mutex> lk(_timerMutex);
                        timerList.splice(std::begin(timerList), _longTermTimerList);
                    }

                    for (auto& timer : timerList)
                    {
                        timer->AddExpireMs(-GetLongTermCheckTimeMilliSecond());
                        RegistTimer(timer);
                    }

                    if (!timerList.empty()) timerList.clear();
                }

                // runTimeMs : 현재 틱의 timer 등록 수행 시간 
                // remainIntervalMs : 다음 틱까지 대기해야 하는 시간
                runTimeMs = startTimeMs - ::time(nullptr);
                remainIntervalMs = GetTimerLogicInterval() - runTimeMs;

                if (remainIntervalMs > 0)
                    std::this_thread::sleep_for(std::chrono::milliseconds(remainIntervalMs));
                else
                    LOG_WARN(L"timerEvent runTimeMs:{}", runTimeMs);
            }
            LOG_INFO(L"TimerLauncher Stop");
            });
    }

    void TimerLauncher::Stop()
    {
        LOG_INFO(L"TimerLauncher Stop");
        _isRun = false;
    }

    int32_t TimerLauncher::GetTimerLogicInterval()
    {
        return _intervalMilliSecond;
    }

    int64_t TimerLauncher::GetCurrentTimerTick()
    {
        int64_t currentTimeTick = _timerTick;
        return currentTimeTick;
    }

    int32_t TimerLauncher::GetNextTimerTickToIndex(int32_t intervalTick)
    {
        if (intervalTick < 0 || DEFAULT_TIMER_MANAGE_MAX_TICK <= intervalTick)
        {
            LOG_ERROR(L"invalid intervalIndex:{}", intervalTick)
                return -1;
        }
        return (GetCurrentTimerTick() + intervalTick) % DEFAULT_TIMER_MANAGE_MAX_TICK;
    }

    int32_t TimerLauncher::GetCurrentTimerTickToIndex()
    {
        return GetCurrentTimerTick() % DEFAULT_TIMER_MANAGE_MAX_TICK;
    }
    int32_t TimerLauncher::GetLastTimerTickToIndex()
    {
        return GetNextTimerTickToIndex(DEFAULT_TIMER_MANAGE_MAX_TICK - 1);
    }

    void TimerLauncher::RegistTimer(Timer* timer)
    {
        registTimer(timer);
    }

    void TimerLauncher::registTimer(Timer* timer)
    {
        const auto currentTick = GetCurrentTimerTick();

        // 타이머의 expireMs가 DEFAULT_TIMER_LOGIC_INTERVAL_MILLISECOND 보다 작다면 가장 최근 타이머에 등록
        const auto intervalTick = timer->GetExpireMs() <= _intervalMilliSecond ?
            0 : static_cast<int32_t>(timer->GetExpireMs() / GetTimerLogicInterval());
        timer->SetExpireTick(currentTick + intervalTick);

        if (DEFAULT_TIMER_MANAGE_MAX_TICK < intervalTick)
        {
            std::unique_lock<std::shared_mutex> lk(_timerMutex);
            _longTermTimerList.push_back(timer);
            return;
        }
        else
        {
            std::unique_lock<std::shared_mutex> lk(_timerMutex);
            const auto currentIndex = GetNextTimerTickToIndex(intervalTick);
            _timerEventArray[currentIndex].push_back(timer);
            return;
        }
    }
}


