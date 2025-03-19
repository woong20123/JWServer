#pragma once
#ifndef __JW_SERIALIZER_H__
#define __JW_SERIALIZER_H__
#include <memory>
#include <list>
#include <shared_mutex>
#include "Timer.h"

namespace jw
{
    class SerializeObject;

    struct SerializerKey
    {
        int16_t type;
        int32_t id;

        bool operator==(const SerializerKey& rhs) const = default;
        auto operator<=>(const SerializerKey&) const = default;

    };


    class SerializerTimer : public Timer
    {
    public:
        SerializerTimer(SerializerKey serializerKey);
        void OnTimer() override;
        bool Post(const std::shared_ptr<SerializeObject>& so, int32_t delayMilliSeconds);
    private:
        SerializerKey _serializerKey;
        std::list<std::shared_ptr<SerializeObject>> _postObjects;
        std::shared_mutex		                    _postObjectsMutex;
    };

    class Serializer
    {
    public:
        static constexpr int32_t NO_DELAY = 0;

        Serializer(SerializerKey serializerKey);
        ~Serializer();

        SerializerKey GetSerializerKey() const { return _serializerKey; }
        bool Post(const std::shared_ptr<SerializeObject>& so);
        bool Post(const std::shared_ptr<SerializeObject>& so, int32_t delayMilliSeconds);
        void SpliceAll(std::list<std::shared_ptr<SerializeObject>>& toList);
        std::shared_ptr<SerializerTimer> GetTimer() const { return _timer; }
    private:
        SerializerKey _serializerKey;
        std::shared_ptr<SerializerTimer>            _timer;
    };

    // Serialize가 연결 되는 객체들의 부모 클래스
    class AttachedSerializerObject
    {
    public:
        AttachedSerializerObject() = default;
        virtual ~AttachedSerializerObject() = default;
        virtual SerializerKey GetSerializerKey() const = 0;
    };
}

#endif // __JW_SERIALIZER_H__