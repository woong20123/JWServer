#pragma once
#ifndef _JW_LOG_WORKER_H_
#define _JW_LOG_WORKER_H_
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
#endif // _JW_LOG_WORKER_H_

