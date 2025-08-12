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
    class LogWorker /*: public Consumer< std::shared_ptr<LogBuffer> >*/
    {
    public:
        using objectType = std::shared_ptr<LogBuffer>;
        using Consumer_t = Consumer<objectType>;
        using Produce_t = ProducerContainer<objectType>;
        static constexpr size_t LOG_STREAM_MAX_COUNT = 10;
        static constexpr uint32_t DEFAULT_LOG_WAIT_TICK_MSEC = 100;

        LogWorker() = default;

        void Initialize();

        void RegisterLogStream(const std::shared_ptr<LogStream>& stream);
        size_t getRegistedLogStreamCount();

        void RunThread();
        void StopThread();

        std::shared_ptr<Produce_t> GetProducerContainer() const
        {
            return _producerCon;
        }

    private:
        void handle(const Consumer_t::container& objs);
        void prepare();

        std::vector<std::shared_ptr<LogStream> >    _logStreams;
        size_t                                      _logStreamCnt{ 0 };
        std::shared_mutex                           _logStreamMutex;

        std::unique_ptr<Consumer_t>                 _consumer;
        std::shared_ptr<Produce_t>                  _producerCon;
    };
}
#endif // __JW_LOG_WORKER_H__

