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

    class SerializerTimer : public Timer
    {
    public:
        SerializerTimer(int32_t serializerId);
        void OnTimer() override;
        void Post(std::shared_ptr<SerializeObject>& so, int32_t delayMilliSeconds);
    private:
        int32_t _serializerId;
        std::list<std::shared_ptr<SerializeObject>> _postObjects;
        std::shared_mutex		                    _postObjectsMutex;
    };

    class Serializer
    {
    public:
        static constexpr int32_t NO_DELAY = 0;

        Serializer(int32_t serializerId);
        ~Serializer();

        int32_t GetSerializerId() const { return _serializerId; }
        void Post(std::shared_ptr<SerializeObject>& so);
        void Post(std::shared_ptr<SerializeObject>& so, int32_t delayMilliSeconds);
        void SpliceAll(std::list<std::shared_ptr<SerializeObject>>& toList);
    private:
        int32_t _serializerId;
        std::unique_ptr<SerializerTimer>            _timer;
    };
}

#endif // __JW_SERIALIZER_H__