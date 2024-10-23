#pragma once
#ifndef __JW_LOG_WORKER_H__
#define __JW_LOG_WORKER_H__
#include "Producer.hpp"
#include "Singleton.hpp"
#include "Consumer.hpp"

namespace jw
{
    class LogBuffer;
    class LogWorker : public CONSUMER(std::shared_ptr<LogBuffer>)
    {
        CONSUMER_BASE_DECLARE(LogWorker, LogBuffer);
    public :
        static constexpr size_t LOG_STREAM_MAX_COUNT = 5;
    private:
        //std::array<LogStream, LOG_STREAM_MAX_COUNT> logStreams;
    };
}
#endif // __JW_LOG_WORKER_H__

