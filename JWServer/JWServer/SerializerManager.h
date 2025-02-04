#pragma once
#ifndef __JW_SERIALIZER_MANAGER_H__
#define __JW_SERIALIZER_MANAGER_H__
#include "Singleton.hpp"
#include <memory>
#include <unordered_map>

namespace jw
{
    class Serializer;
    class SerializeObject;
    class SerializerManager : public Singleton<SerializerManager>
    {
    public:
        constexpr static int32_t MAX_SERIALIZE_TYPE = 32;
        constexpr static int32_t NO_DELAY_TIME = 0;

        void Initialize();
        bool RegistSerializer(int64_t serializerId, std::shared_ptr<Serializer>& serializer);
        std::shared_ptr<Serializer> GetSerializer(int64_t serializerId);
        void Post(int64_t serializerId, std::shared_ptr<SerializeObject>& so);
        void Post(int64_t serializerId, std::shared_ptr<SerializeObject>& so, int32_t delayMilliSeconds);
        int64_t MakeSerializeId(int32_t serializeType);
    protected:
        SerializerManager();
        ~SerializerManager();
        SerializerManager(const SerializerManager&) = delete;
        SerializerManager& operator=(const SerializerManager&) = delete;
    private:
        friend class Singleton<SerializerManager>;
        std::unordered_map<int64_t, std::shared_ptr<Serializer>> _serializers;
        std::atomic<int32_t> _serializeIndexs[MAX_SERIALIZE_TYPE];
    };
}

#define SERIALIZER_MANAGER jw::SerializerManager::GetInstance

#endif // __JW_SERIALIZER_MANAGER_H__

