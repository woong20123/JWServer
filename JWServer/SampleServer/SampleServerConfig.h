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
        SampleServerConfig() = default;
        virtual ~SampleServerConfig() = default;

        void Initialize(std::shared_ptr<ConfigParser> parser) override;
        void OnLoading() override;
        void OnLoaded() override;

        int16_t GetServerPort();
        int16_t GetWorkerThreadCount();
        int64_t GetMaxClientSessionCount();
        int32_t GetTimerTickIntervalMilliSecond();

    };
}

#endif // !__JW_SAMPLE_SERVER_CONFIG_H__
