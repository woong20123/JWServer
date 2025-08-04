#pragma once
#ifndef __JW_SAMPLE_SERVER_CONFIG_H__
#define __JW_SAMPLE_SERVER_CONFIG_H__
#include "Config.h"

namespace jw
{
    class ConfigParser;
    class SampleServerConfig : public Config
    {
    public:
        struct ConfigDefinition
        {
            const char* key;
            const wchar_t* defaultValue;
        };

        enum : uint16_t
        {
            CONFIG_DEF_NONE = 0,
            CONFIG_DEF_SERVER_PORT,
            CONFIG_DEF_WORKER_THREAD,
            CONFIG_DEF_MAX_CLIENT_SESSION_COUNT,
            CONFIG_DEF_TIMER_TICK_INTERVAL_MSEC,
            CONFIG_DEF_LOGGER_TICK_INTERVAL_MSEC,
            CONFIG_DEF_SESSION_RECV_CHECK_TIME_SECOND,
            CONFIG_DEF_BAD_IP_BLOCK_ENABLE,
            CONFIG_DEF_BAD_IP_BLOCK_SANCTION_TIME_SECOND,
            CONFIG_DEF_BAD_IP_BLOCK_TRIGGERING_SESSION_COUNT,
            CONFIG_DEF_BAD_IP_BLOCK_THRESHOLD_CHECKED_COUNT,
            CONFIG_DEF_THREAD_FROZEN_CHECK_TIME_SECOND,
            CONFIG_DEF_MAX
        };

        static constexpr ConfigDefinition CONFIG_DEFINITIONS[(size_t)CONFIG_DEF_MAX] = {
            { "none", L"none" },
            { "server-port", L"13211" },
            { "worker-thread", L"0" },
            { "max-client-session-count", L"5000" },
            { "timer-tick-interval-msec", L"100" },
            { "logger-tick-interval-msec", L"100" },
            { "session-recv-check-time-second", L"300" },
            { "bad-ip-block-enable", Config::BOOL_FALSE },
            { "bad-ip-block-sanction-time-second", L"3600" },
            { "bad-ip-block-triggering-session-count", L"5000" },
            { "bad-ip-block-threshold-checked-count", L"1" },
            { "thread-frozen-check-time-second", L"60"},
        };

        SampleServerConfig() = default;
        virtual ~SampleServerConfig() = default;

        void Initialize(std::shared_ptr<ConfigParser> parser) override;

        const int16_t GetServerPort();
        const int16_t GetWorkerThreadCount();
        const int64_t GetMaxClientSessionCount();
        const int64_t GetTimerTickIntervalMilliSecond();
        const int64_t GetLoggerTickIntervalMilliSecond();
        const int64_t GetSessionRecvCheckTimeSecond();
        const bool GetBadIpBlockEnable();
        const int64_t GetBadIpBlockSanctionTimeSecond();
        const int32_t GetBadIpBlockTriggeringSessionCount();
        const int32_t GetBadIpBlockThresholdCheckedCount();
        const int32_t GetThreadFrozenCheckTimeSecond();
    private:
        void onLoading() override;
        void onLoaded() override;
    };
}

#endif // !__JW_SAMPLE_SERVER_CONFIG_H__
