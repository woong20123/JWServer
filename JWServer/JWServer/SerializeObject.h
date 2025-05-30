﻿#pragma once
#ifndef __JW_SERIALIZE_OBJECT_H__
#define __JW_SERIALIZE_OBJECT_H__
#include <cstdint>
#include <functional>
#include <memory>

namespace jw
{
    enum SerializeObjectType
    {
        SERAILIZE_OBJECT_TYPE_NONE = 0,
        SERAILIZE_OBJECT_TYPE_LAMBDA,
        SERAILIZE_OBJECT_TYPE_MAX,
    };

    class SerializeObject
    {
    public:
        SerializeObject(int32_t type, int32_t id);
        ~SerializeObject() = default;

        void Initialize(const int64_t delayMilliSeconds, const time_t intervalMilliSeconds);
        virtual void Execute() = 0;

        int32_t GetType() const { return _type; }
        int32_t GetID() const { return _id; }
        int64_t GetDelayTick() const { return _delayTick; }
        void ReCalculateTick();

    private:
        int32_t _type;
        int32_t _id;
        int64_t _delayMilliSeconds;
        int64_t _registMilliSeconds;
        int64_t _delayTick;
        int32_t _serializerId;
        time_t _intervalMilliSeconds;
    };
}

#endif // __JW_SERIALIZE_OBJECT_H__

