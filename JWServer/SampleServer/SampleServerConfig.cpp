#include "SampleServerConfig.h"
#include <memory>

namespace jw
{
    void SampleServerConfig::Initialize(std::shared_ptr<ConfigParser> parser)
    {
        Config::Initialize(parser);
    }
    void SampleServerConfig::onLoading()
    {
        RegisterConfigDefinition(SampleServerConfig::SERVER_PORT, L"13211");
        RegisterConfigDefinition(SampleServerConfig::WORKER_THREAD, L"0");
        RegisterConfigDefinition(SampleServerConfig::MAX_CLIENT_SESSION_COUNT, L"5000");
        RegisterConfigDefinition(SampleServerConfig::TIMER_TICK_INTERVAL_MSEC, L"100");
        RegisterConfigDefinition(SampleServerConfig::LOGGER_TICK_INTERVAL_MSEC, L"100");
        RegisterConfigDefinition(SampleServerConfig::SESSION_RECV_CHECK_TIME_SECOND, L"300");
        RegisterConfigDefinition(SampleServerConfig::BAD_IP_BLOCK_ENABLE, Config::BOOL_FALSE);
        RegisterConfigDefinition(SampleServerConfig::BAD_IP_BLOCK_SANCTION_TIME_SECOND, L"3600");
        RegisterConfigDefinition(SampleServerConfig::BAD_IP_BLOCK_TRIGGERING_SESSION_COUNT, L"5000");
        RegisterConfigDefinition(SampleServerConfig::BAD_IP_BLOCK_THRESHOLD_CHECKED_COUNT, L"1");
    }

    void SampleServerConfig::onLoaded()
    { }

    const int16_t SampleServerConfig::GetServerPort()
    {
        return GetInt16(SampleServerConfig::SERVER_PORT);
    }

    const int16_t SampleServerConfig::GetWorkerThreadCount()
    {
        return GetInt16(SampleServerConfig::WORKER_THREAD);
    }

    const int64_t SampleServerConfig::GetMaxClientSessionCount()
    {
        return GetInt64(SampleServerConfig::MAX_CLIENT_SESSION_COUNT);
    }

    const int64_t SampleServerConfig::GetTimerTickIntervalMilliSecond()
    {
        return GetInt64(SampleServerConfig::TIMER_TICK_INTERVAL_MSEC);
    }

    const int64_t SampleServerConfig::GetLoggerTickIntervalMilliSecond()
    {
        return GetInt64(SampleServerConfig::LOGGER_TICK_INTERVAL_MSEC);
    }

    const int64_t SampleServerConfig::GetSessionRecvCheckTimeSecond()
    {
        return GetInt64(SampleServerConfig::SESSION_RECV_CHECK_TIME_SECOND);
    }

    const bool SampleServerConfig::GetBadIpBlockEnable()
    {
        return GetBool(SampleServerConfig::BAD_IP_BLOCK_ENABLE);
    }
    const int64_t SampleServerConfig::GetBadIpBlockSanctionTimeSecond()
    {
        return GetInt64(SampleServerConfig::BAD_IP_BLOCK_SANCTION_TIME_SECOND);
    }
    const int32_t SampleServerConfig::GetBadIpBlockTriggeringSessionCount()
    {
        return GetInt32(SampleServerConfig::BAD_IP_BLOCK_TRIGGERING_SESSION_COUNT);
    }
    const int32_t SampleServerConfig::GetBadIpBlockThresholdCheckedCount()
    {
        return GetInt32(SampleServerConfig::BAD_IP_BLOCK_THRESHOLD_CHECKED_COUNT);
    }
}