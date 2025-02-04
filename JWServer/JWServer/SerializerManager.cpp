#include "SerializerManager.h"
#include "Serializer.h"
#include "Logger.h"

namespace jw
{
    SerializerManager::SerializerManager()
    {
    }

    SerializerManager::~SerializerManager()
    {
    }

    void SerializerManager::Initialize()
    {
    }

    bool SerializerManager::RegistSerializer(int64_t serializerId, std::shared_ptr<Serializer>& serializer)
    {
        if (_serializers.contains(serializerId))
        {
            LOG_ERROR(L"Already exist serializer id: {}", serializerId);

            return false;
        }

        _serializers[serializerId] = serializer;
        return true;
    }

    std::shared_ptr<Serializer> SerializerManager::GetSerializer(int64_t serializerId)
    {
        if (!_serializers.contains(serializerId))
        {
            LOG_ERROR(L"Can't find serializer id: {}", serializerId);
            return nullptr;
        }

        return _serializers[serializerId];
    }

    void SerializerManager::Post(int64_t serializerId, std::shared_ptr<SerializeObject>& so)
    {
        return Post(serializerId, so, NO_DELAY_TIME);
    }

    void SerializerManager::Post(int64_t serializerId, std::shared_ptr<SerializeObject>& so, int32_t delayMilliSeconds)
    {
        if (!_serializers.contains(serializerId))
        {
            LOG_ERROR(L"Can't find serializer id: {}", serializerId);
            return;
        }

        _serializers[serializerId]->Post(so, delayMilliSeconds);
    }

    int64_t SerializerManager::MakeSerializeId(int32_t serializeType)
    {
        if (serializeType < 0 || serializeType >= MAX_SERIALIZE_TYPE)
        {
            LOG_ERROR(L"Invalid serialize type: {}", serializeType);
            return 0;
        }
        const auto index = ++_serializeIndexs[serializeType];
        const auto id = (static_cast<int64_t>(serializeType) << 32) | static_cast<int64_t>(index);

        LOG_INFO(L"issue serializer id:{}, type:{} index:{}", id, serializeType, index);

        return id;
    }
}
