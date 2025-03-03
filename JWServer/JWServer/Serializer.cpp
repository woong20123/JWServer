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
        for (auto& serializeObject : _postObjects)
        {
            serializeObject->Execute();
        }

        _postObjects.clear();

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