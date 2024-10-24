#pragma once
#ifndef __JW_LOG_WORKER_H__
#define __JW_LOG_WORKER_H__
#include "Producer.hpp"
#include "Singleton.hpp"
#include "Consumer.hpp"
#include "LogStream.h"
#include <vector>

namespace jw
{
    class LogBuffer;
    class LogWorker : public CONSUMER(std::shared_ptr<LogBuffer>)
    {
        CONSUMER_BASE_DECLARE(LogWorker, LogBuffer);
    public:
        static constexpr size_t LOG_STREAM_MAX_COUNT = 5;
        void RegisterLogStream(const std::shared_ptr<LogStream> &stream);
    private:
        std::vector<std::shared_ptr<LogStream> >    _logStreams;
        size_t                                      _logStreamCnt{ 0 };
    };
}
#endif // __JW_LOG_WORKER_H__

