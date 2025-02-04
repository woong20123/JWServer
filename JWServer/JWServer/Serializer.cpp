#include "Serializer.h"
#include "SerializeObject.h"
#include "SerializerManager.h"
#include "TypeDefinition.h"
#include "TimerLauncher.h"
#include "Logger.h"

namespace jw
{
    SerializerTimer::SerializerTimer(int32_t serializerId) : _serializerId{ serializerId }
    {
    }

    void SerializerTimer::OnTimer()
    {
        //const auto& serializer = SERIALIZER_MANAGER().GetSerializer(_serializerId);

        //std::list<std::shared_ptr<SerializeObject>> serializeObjectList;
        //serializer->SpliceAll(serializeObjectList);

        for (auto& serializeObject : _postObjects)
        {
            serializeObject->Execute();
        }

        TIMER_LAUNCHER().RegistTimer(this);

        LOG_DEBUG(L"SerializerTimer::OnTimer()");
    }

    void SerializerTimer::Post(std::shared_ptr<SerializeObject>& so, int32_t delayMilliSeconds)
    {
        so->Initialize(delayMilliSeconds);
        {
            WRITE_LOCK(_postObjectsMutex);
            _postObjects.push_back(so);
        }
    }

    Serializer::Serializer(int32_t serializerId) : _serializerId{ serializerId }
    {
        _timer = std::make_unique<SerializerTimer>(_serializerId);
        _timer->SetExpireMs(100);
        TIMER_LAUNCHER().RegistTimer(_timer.get());
    }

    Serializer::~Serializer()
    {
    }

    void Serializer::Post(std::shared_ptr<SerializeObject>& so)
    {
        Post(so, NO_DELAY);
    }

    void Serializer::Post(std::shared_ptr<SerializeObject>& so, int32_t delayMilliSeconds)
    {
        _timer->Post(so, delayMilliSeconds);
    }

    void Serializer::SpliceAll(std::list<std::shared_ptr<SerializeObject>>& toList)
    {
        /*WRITE_LOCK(_postObjectsMutex);
        _postObjects.splice(_postObjects.begin(), toList);*/
    }
}