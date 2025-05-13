#include "SerializerManager.h"
#include "Logger.h"

namespace jw
{
    SerializerManager::SerializerManager()
    {
    }
    void SerializerManager::Initialize()
    {
    }

    bool SerializerManager::RegistSerializer(SerializerKey& serializerKey, std::shared_ptr<Serializer>& serializer)
    {
        if (_serializers.contains(serializerKey))
        {
            LOG_ERROR(L"Already exist serializer type:{}, id:{}", serializerKey.type, serializerKey.id);
            return false;
        }

        _serializers[serializerKey] = serializer;
        return true;
    }

    std::shared_ptr<Serializer> SerializerManager::GetSerializer(const SerializerKey& serializerKey)
    {
        if (!_serializers.contains(serializerKey))
        {
            LOG_ERROR(L"Can't find serializer, type:{}, id:{}", serializerKey.type, serializerKey.id);
            return nullptr;
        }

        return _serializers[serializerKey];
    }

    bool SerializerManager::Post(SerializerKey& serializerKey, const std::shared_ptr<SerializeObject>& so)
    {
        return Post(serializerKey, so, NO_DELAY_TIME);
    }

    bool SerializerManager::Post(SerializerKey& serializerKey, const std::shared_ptr<SerializeObject>& so, int32_t delayMilliSeconds)
    {
        if (!_serializers.contains(serializerKey))
        {
            LOG_ERROR(L"Can't find serializer type:{} ,id:{}", serializerKey.type, serializerKey.id);
            return false;
        }

        return _serializers[serializerKey]->Post(so, delayMilliSeconds);
    }

    int32_t SerializerManager::MakeSerializeId(const int16_t serializeType)
    {
        if (serializeType < 0 || serializeType >= MAX_SERIALIZE_TYPE)
        {
            LOG_ERROR(L"Invalid serialize type: {}", serializeType);
            return 0;
        }
        const auto id = ++_serializeIndexs[serializeType];

        LOG_INFO(L"issue serializer id:{}, type:{}", id, serializeType);

        return id;
    }
}
