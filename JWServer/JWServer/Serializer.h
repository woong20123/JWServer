#pragma once
#ifndef __JW_SERIALIZER_H__
#define __JW_SERIALIZER_H__
#include <memory>


namespace jw
{
    class SerializeObject;
    class SerializerTimer;
    struct SerializerKey
    {
        constexpr static int32_t INVALID_VALUE = -1;

        SerializerKey() = default;
        ~SerializerKey() = default;
        SerializerKey(int16_t type, int32_t id) : type{ type }, id{ id } {}

        int16_t type;
        int32_t id;

        bool operator==(const SerializerKey& rhs) const = default;
        auto operator<=>(const SerializerKey&) const = default;

        static const SerializerKey INVALID_KEY() { return { INVALID_VALUE, INVALID_VALUE }; }
    };


    class Serializer
    {
    public:
        static constexpr int32_t NO_DELAY = 0;

        Serializer(SerializerKey serializerKey, const time_t tickIntervalMs);
        ~Serializer();

        SerializerKey GetSerializerKey() const { return _serializerKey; }
        bool Post(const std::shared_ptr<SerializeObject>& so);
        bool Post(const std::shared_ptr<SerializeObject>& so, int32_t delayMilliSeconds);
        std::shared_ptr<SerializerTimer> GetTimer() const { return _timer; }
    private:
        SerializerKey                                   _serializerKey;
        std::shared_ptr<SerializerTimer>                _timer;
    };
}

#endif // __JW_SERIALIZER_H__