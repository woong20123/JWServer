#include "SampleServerConfig.h"
#include "Logger.h"
#include "StringConverter.h"
#include <memory>

namespace jw
{
    void SampleServerConfig::Initialize(std::shared_ptr<ConfigParser> parser)
    {
        Config::Initialize(parser);
    }
    void SampleServerConfig::onLoading()
    {
        bool isValid{ true };
        for (const auto& def : CONFIG_DEFINITIONS)
        {
            if (def.key == "none") { continue; }

            if (0 == strnlen_s(def.key, 128))
            {
                SetValidate(false);
                LOG_ERROR(L"is not valid, key is empty");
            }

            RegisterConfigDefinition(def.key, def.defaultValue);
        }


    }

    void SampleServerConfig::onLoaded()
    {
        LOG_INFO(L"[Config Print]");
        for (const auto& def : CONFIG_DEFINITIONS)
        {
            if (def.key == "none") { continue; }
            if (GetString(def.key).empty())
            {
                LOG_ERROR(L"key is empty, key:{}, defaultValue:{}", StringConverter::StrA2WUseUTF8(def.key)->c_str(), def.defaultValue);
                SetValidate(false);
            }
            else
            {
                LOG_INFO(L"key:{}, value:{}", StringConverter::StrA2WUseUTF8(def.key)->c_str(), GetString(def.key));
            }
        }
    }

    const int16_t SampleServerConfig::GetServerPort()
    {
        return GetInt16(CONFIG_DEFINITIONS[CONFIG_DEF_SERVER_PORT].key);
    }

    const int16_t SampleServerConfig::GetWorkerThreadCount()
    {
        return GetInt16(CONFIG_DEFINITIONS[CONFIG_DEF_WORKER_THREAD].key);
    }

    const int64_t SampleServerConfig::GetMaxClientSessionCount()
    {
        return GetInt64(CONFIG_DEFINITIONS[CONFIG_DEF_MAX_CLIENT_SESSION_COUNT].key);
    }

    const int64_t SampleServerConfig::GetTimerTickIntervalMilliSecond()
    {
        return GetInt64(CONFIG_DEFINITIONS[CONFIG_DEF_TIMER_TICK_INTERVAL_MSEC].key);
    }

    const int64_t SampleServerConfig::GetLoggerTickIntervalMilliSecond()
    {
        return GetInt64(CONFIG_DEFINITIONS[CONFIG_DEF_LOGGER_TICK_INTERVAL_MSEC].key);
    }

    const int64_t SampleServerConfig::GetSessionRecvCheckTimeSecond()
    {
        return GetInt64(CONFIG_DEFINITIONS[CONFIG_DEF_SESSION_RECV_CHECK_TIME_SECOND].key);
    }

    const bool SampleServerConfig::GetBadIpBlockEnable()
    {
        return GetBool(CONFIG_DEFINITIONS[CONFIG_DEF_BAD_IP_BLOCK_ENABLE].key);
    }
    const int64_t SampleServerConfig::GetBadIpBlockSanctionTimeSecond()
    {
        return GetInt64(CONFIG_DEFINITIONS[CONFIG_DEF_BAD_IP_BLOCK_SANCTION_TIME_SECOND].key);
    }
    const int32_t SampleServerConfig::GetBadIpBlockTriggeringSessionCount()
    {
        return GetInt32(CONFIG_DEFINITIONS[CONFIG_DEF_BAD_IP_BLOCK_TRIGGERING_SESSION_COUNT].key);
    }
    const int32_t SampleServerConfig::GetBadIpBlockThresholdCheckedCount()
    {
        return GetInt32(CONFIG_DEFINITIONS[CONFIG_DEF_BAD_IP_BLOCK_THRESHOLD_CHECKED_COUNT].key);
    }

    const int32_t SampleServerConfig::GetThreadFrozenCheckTimeSecond()
    {
        return GetInt32(CONFIG_DEFINITIONS[CONFIG_DEF_THREAD_FROZEN_CHECK_TIME_SECOND].key);
    }
}