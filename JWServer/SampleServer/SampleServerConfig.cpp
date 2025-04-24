#include "SampleServerConfig.h"
#include <memory>

namespace jw
{
    void SampleServerConfig::Initialize(std::shared_ptr<ConfigParser> parser)
    {
        Config::Initialize(parser);
    }
    void SampleServerConfig::OnLoading()
    { }

    void SampleServerConfig::OnLoaded()
    { }

    int16_t SampleServerConfig::GetServerPort()
    {
        return GetInt16(SampleServerConfig::SERVER_PORT);
    }

    int16_t SampleServerConfig::GetWorkerThreadCount()
    {
        return GetInt16(SampleServerConfig::WORKER_THREAD);
    }

    int64_t SampleServerConfig::GetMaxClientSessionCount()
    {
        return GetInt64(SampleServerConfig::MAX_CLIENT_SESSION_COUNT);
    }

    int64_t SampleServerConfig::GetTimerTickIntervalMilliSecond()
    {
        return GetInt64(SampleServerConfig::TIMER_TICK_INTERVAL_MILLISECOND);
    }

    int64_t SampleServerConfig::GetSessionRecvCheckTimeSecond()
    {
        return GetInt64(SampleServerConfig::SESSION_RECV_CHECK_TIME_SECOND);
    }
}