#include "Serializer.h"
#include "TimerLauncher.h"
#include "SerializerTimer.h"

namespace jw
{
    Serializer::Serializer(SerializerKey serializerKey, const time_t tickIntervalMs) : _serializerKey{ serializerKey }
    {
        _timer = std::make_shared<SerializerTimer>(_serializerKey);
        _timer->SetIntervalMs(tickIntervalMs);
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

}