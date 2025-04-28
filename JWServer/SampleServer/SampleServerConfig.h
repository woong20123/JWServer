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

        static constexpr const char* SERVER_PORT = "server-port";
        static constexpr const char* WORKER_THREAD = "worker-thread";
        static constexpr const char* MAX_CLIENT_SESSION_COUNT = "max-client-session-count";
        static constexpr const char* TIMER_TICK_INTERVAL_MSEC = "timer-tick-interval-msec";
        static constexpr const char* LOGGER_TICK_INTERVAL_MSEC = "logger-tick-interval-msec";
        static constexpr const char* SESSION_RECV_CHECK_TIME_SECOND = "session-recv-check-time-second";
        static constexpr const char* BAD_IP_BLOCK_ENABLE = "bad-ip-block-enable";
        static constexpr const char* BAD_IP_BLOCK_SANCTION_TIME_SECOND = "bad-ip-block-sanction-time-second";
        static constexpr const char* BAD_IP_BLOCK_TRIGGERING_SESSION_COUNT = "bad-ip-block-triggering-session-count";
        static constexpr const char* BAD_IP_BLOCK_THRESHOLD_CHECKED_COUNT = "bad-ip-block-threshold-checked-count";

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
    private:
        void onLoading() override;
        void onLoaded() override;
    };
}

#endif // !__JW_SAMPLE_SERVER_CONFIG_H__
