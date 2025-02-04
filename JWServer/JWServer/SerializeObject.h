#pragma once
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
        SERAILIZE_OBJECT_TYPE_LAMBDA = 1,
    };

    class SerializeObject
    {
    public:
        SerializeObject(int id);
        ~SerializeObject();

        void Initialize(int32_t delayMilliSeconds);
        virtual void Execute() = 0;

    private:
        int64_t _id;
        int32_t _delayMilliSeconds;
        int64_t _registMilliSeconds;
        int64_t _executeTick;
        int32_t _serializerId;
    };

    class LambdaSerializeObject : public SerializeObject
    {
    public:
        LambdaSerializeObject(std::function<void()> func) : SerializeObject(SERAILIZE_OBJECT_TYPE_LAMBDA)
        {};
        ~LambdaSerializeObject() {};

        void Execute() override;
    private:
        std::function<void()> _func;
    };
}

#endif // __JW_SERIALIZE_OBJECT_H__

