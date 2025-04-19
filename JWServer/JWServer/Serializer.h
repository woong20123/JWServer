#pragma once
#ifndef __JW_SERIALIZER_H__
#define __JW_SERIALIZER_H__
#include <memory>
#include <list>
#include <array>
#include <shared_mutex>
#include "Timer.h"

namespace jw
{
    class SerializeObject;

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

        static SerializerKey INVALID_KEY() { return { INVALID_VALUE, INVALID_VALUE }; }
    };


    class SerializerTimer : public Timer
    {
    public:
        static constexpr int32_t SHORT_TERM_MAX_TICK = 10;
        static constexpr int64_t INVALID_TICK = -1;
        using SerializeList = std::list<std::shared_ptr<SerializeObject>>;
        using SerializeListArray = std::array<SerializeList, SHORT_TERM_MAX_TICK>;

        SerializerTimer(SerializerKey serializerKey);
        void OnTimer() override;
        bool Post(const std::shared_ptr<SerializeObject>& so, int32_t delayMilliSeconds);
    private:
        // _postObjectsMutex lock을 사용한 곳에서 호출 해야 합니다. 
        bool post(const std::shared_ptr<SerializeObject>& so);
        void updateExecuteTick();
        int64_t getNextTick(int64_t nextTick) const;


        SerializerKey                                   _serializerKey;
        SerializeListArray                              _shortTermObjects;
        SerializeList                                   _longTermObjects;
        std::shared_mutex		                        _shortTermObjectsMutex;
        std::shared_mutex		                        _longTermObjectsMutex;
        int64_t                                         _executeTick;
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