#include "SerializerTimer.h"
#include "Serializer.h"
#include "SerializeObject.h"
#include "TypeDefinition.h"
#include "TimerLauncher.h"
#include "Logger.h"

namespace jw
{
    SerializerTimer::SerializerTimer(SerializerKey& serializerKey) : _serializerKey{ serializerKey }, _executeTick{}
    {
    }

    void SerializerTimer::OnTimer()
    {
        bool isExecute = false;
        bool isCheckLongTerm{ false };

        std::list<std::shared_ptr<SerializeObject>> executeObjects;
        {
            // _executeTick는 _shortTermObjectsMutex lock을 선언한 곳에서만 사용해야 합니다.
            WRITE_LOCK(_shortTermObjectsMutex);
            isCheckLongTerm = getIsLongTermCheckTerm();
            auto& currentTickObjects = _shortTermObjects[_executeTick];
            if (!currentTickObjects.empty())
            {
                // list의 splice를 사용하면 상수 시간 복잡도로 처리합니다. 
                executeObjects.splice(std::begin(executeObjects), currentTickObjects);
            }

            updateExecuteTick();
        }

        if (!executeObjects.empty())
        {
            for (auto& serializeObject : executeObjects)
            {
                serializeObject->Execute();
            }
        }

        if (isCheckLongTerm)
        {
            std::list<std::shared_ptr<SerializeObject>> longtermPostObjects;
            {
                WRITE_LOCK(_longTermObjectsMutex);
                if (!_longTermObjects.empty())
                {
                    longtermPostObjects.splice(std::begin(longtermPostObjects), _longTermObjects);
                }
            }

            if (!longtermPostObjects.empty())
            {
                for (auto& serializeObject : longtermPostObjects)
                {
                    serializeObject->ReCalculateTick();
                    post(serializeObject);
                }
                longtermPostObjects.clear();
            }
        }

        GetTimerLauncher().RegistTimer(this);
    }

    bool SerializerTimer::Post(const std::shared_ptr<SerializeObject>& so, int32_t delayMilliSeconds)
    {
        so->Initialize(delayMilliSeconds, GetTimerLauncher().GetTickIntervalMilliSecond());
        {
            post(so);
        }
        return true;
    }

    bool SerializerTimer::post(const std::shared_ptr<SerializeObject>& so)
    {
        const auto& delayTick = so->GetDelayTick();
        if (delayTick < SHORT_TERM_MAX_TICK)
        {
            WRITE_LOCK(_shortTermObjectsMutex);
            const auto nextTick = getNextTick(delayTick);
            if (nextTick == INVALID_TICK)
            {
                LOG_ERROR(L"SerializerTimer::Post invalid delayTick:{}", delayTick);
                return false;
            }
            _shortTermObjects[nextTick].push_back(so);
        }
        else
        {
            WRITE_LOCK(_longTermObjectsMutex);
            // long term timer
            _longTermObjects.push_back(so);
        }
        return true;
    }

    void SerializerTimer::updateExecuteTick()
    {
        _executeTick = (_executeTick + 1) % SHORT_TERM_MAX_TICK;
    }

    int64_t SerializerTimer::getNextTick(int64_t nextTick) const
    {
        if (nextTick < 0 || SHORT_TERM_MAX_TICK <= nextTick) return INVALID_TICK;
        return (_executeTick + nextTick) % SHORT_TERM_MAX_TICK;
    }

    const bool SerializerTimer::getIsLongTermCheckTerm()
    {
        return _executeTick % SHORT_TERM_MAX_TICK == SHORT_TERM_MAX_TICK - 1;
    }
}