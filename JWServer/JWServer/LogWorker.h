#pragma once
#ifndef __JW_LOG_WORKER_H__
#define __JW_LOG_WORKER_H__
#include "ProducerContainer.hpp"
#include "Singleton.hpp"
#include "Consumer.hpp"
#include "LogStream.h"
#include <vector>
#include <shared_mutex>

namespace jw
{
    class LogBuffer;
    class LogWorker : public Consumer< std::shared_ptr<LogBuffer> >
    {
        CONSUMER_BASE_DECLARE(LogWorker, LogBuffer);
    public:
        static constexpr size_t LOG_STREAM_MAX_COUNT = 10;

        // 스레드 수행전 해야 할 작업 등록
        // 스레드 세이프 합니다. 
        void prepare() override;

        void RegisterLogStream(const std::shared_ptr<LogStream>& stream);
        size_t getRegistedLogStreamCount();
    private:
        std::vector<std::shared_ptr<LogStream> >    _logStreams;
        size_t                                      _logStreamCnt{ 0 };
        std::shared_mutex                           _logStreamMutex;
    };
}
#endif // __JW_LOG_WORKER_H__

