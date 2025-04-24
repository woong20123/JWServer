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
        static constexpr const char* TIMER_TICK_INTERVAL_MILLISECOND = "timer-tick-interval-milliSecond";
        static constexpr const char* SESSION_RECV_CHECK_TIME_SECOND = "session-recv-check-time-second";

        SampleServerConfig() = default;
        virtual ~SampleServerConfig() = default;

        void Initialize(std::shared_ptr<ConfigParser> parser) override;
        void OnLoading() override;
        void OnLoaded() override;

        int16_t GetServerPort();
        int16_t GetWorkerThreadCount();
        int64_t GetMaxClientSessionCount();
        int64_t GetTimerTickIntervalMilliSecond();
        int64_t GetSessionRecvCheckTimeSecond();

    };
}

#endif // !__JW_SAMPLE_SERVER_CONFIG_H__
