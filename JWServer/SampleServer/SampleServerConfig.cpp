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
        return GetInt16("server-port");
    }

    int16_t SampleServerConfig::GetWorkerThreadCount()
    {
        return GetInt16("worker-thread");
    }

    int64_t SampleServerConfig::GetMaxClientSessionCount()
    {
        return GetInt64("max-client-session-count");
    }

    int32_t SampleServerConfig::GetTimerTickIntervalMilliSecond()
    {
        return GetInt32("timer-tick-interval-milliSecond");
    }
}