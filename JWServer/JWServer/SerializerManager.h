#pragma once
#ifndef __JW_SERIALIZER_MANAGER_H__
#define __JW_SERIALIZER_MANAGER_H__
#include "Singleton.hpp"
#include <memory>
#include <map>
#include "Serializer.h"

namespace jw
{
    class SerializeObject;
    class SerializerManager : public Singleton<SerializerManager>
    {
    public:
        constexpr static int32_t MAX_SERIALIZE_TYPE = 32;
        constexpr static int32_t NO_DELAY_TIME = 0;

        SerializerManager(const SerializerManager&) = delete;
        SerializerManager& operator=(const SerializerManager&) = delete;

        void Initialize();
        bool RegistSerializer(SerializerKey& serializerKey, std::shared_ptr<Serializer>& serializer);
        std::shared_ptr<Serializer> GetSerializer(const SerializerKey& serializerKey);
        bool Post(SerializerKey& serializerKey, const std::shared_ptr<SerializeObject>& so);
        bool Post(SerializerKey& serializerKey, const std::shared_ptr<SerializeObject>& so, int32_t delayMilliSeconds);
        int32_t MakeSerializeId(const int16_t serializeType);
    protected:
        SerializerManager();
        ~SerializerManager() = default;
    private:
        friend class Singleton<SerializerManager>;
        std::map<SerializerKey, std::shared_ptr<Serializer>> _serializers;
        std::atomic<int32_t> _serializeIndexs[MAX_SERIALIZE_TYPE];
    };
}

#define SERIALIZER_MANAGER jw::SerializerManager::GetInstance

#endif // __JW_SERIALIZER_MANAGER_H__

