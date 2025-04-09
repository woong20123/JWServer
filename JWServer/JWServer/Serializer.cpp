#include "Serializer.h"
#include "SerializeObject.h"
#include "SerializerManager.h"
#include "TypeDefinition.h"
#include "TimerLauncher.h"
#include "Logger.h"

namespace jw
{
    SerializerTimer::SerializerTimer(SerializerKey serializerKey) : _serializerKey{ serializerKey }
    {
    }

    void SerializerTimer::OnTimer()
    {
        std::list<std::shared_ptr<SerializeObject>> executePostObjects;
        bool isExecute = false;
        {
            WRITE_LOCK(_postObjectsMutex);
            if (!_postObjects.empty())
            {
                isExecute = true;

                // 처리할 객체가 있다면 executePostObjects에 복사하여 lock 구간을 최소화 합니다.
                // list의 splice를 사용하면 상수 시간 복잡도로 처리할 수 있습니다. 
                executePostObjects.splice(std::begin(executePostObjects), _postObjects);
            }
        }

        if (isExecute)
        {
            for (auto& serializeObject : executePostObjects)
            {
                serializeObject->Execute();
            }
        }

        TIMER_LAUNCHER().RegistTimer(this);

        //LOG_DEBUG(L"SerializerTimer::OnTimer()");
    }

    bool SerializerTimer::Post(const std::shared_ptr<SerializeObject>& so, int32_t delayMilliSeconds)
    {
        so->Initialize(delayMilliSeconds);
        {
            WRITE_LOCK(_postObjectsMutex);
            _postObjects.push_back(so);
        }
        return true;
    }

    Serializer::Serializer(SerializerKey serializerKey) : _serializerKey{ serializerKey }
    {
        _timer = std::make_shared<SerializerTimer>(_serializerKey);
        _timer->SetExpireMs(100);
        TIMER_LAUNCHER().RegistTimer(_timer.get());
    }

    Serializer::~Serializer()
    {
    }

    bool Serializer::Post(const std::shared_ptr<SerializeObject>& so)
    {
        return Post(so, NO_DELAY);
    }

    bool Serializer::Post(const std::shared_ptr<SerializeObject>& so, int32_t delayMilliSeconds)
    {
        return _timer->Post(so, delayMilliSeconds);
    }

    void Serializer::SpliceAll(std::list<std::shared_ptr<SerializeObject>>& toList)
    {
        /*WRITE_LOCK(_postObjectsMutex);
        _postObjects.splice(_postObjects.begin(), toList);*/
    }
}