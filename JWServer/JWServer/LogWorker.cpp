#include "LogWorker.h"
#include "LogBuffer.h"
#include "LogStream.h"
#include "TypeDefinition.h"
#include <memory>
#include <array>
#include <functional>

namespace jw
{
    void LogWorker::Initialize()
    {
        using namespace std::placeholders;

        _producerCon = std::make_shared<Produce_t>(DEFAULT_LOG_WAIT_TICK_MSEC);
        _consumer = std::unique_ptr<Consumer_t>(new Consumer_t(_producerCon));

        _consumer->SetName("LogWorker");
        _consumer->SetPrepareFunc(std::bind(&LogWorker::prepare, this));
        _consumer->SetHandleFunc(std::bind(&LogWorker::handle, this, _1));
    }

    void LogWorker::prepare()
    {
        for (auto& stream : std::as_const(_logStreams))
            stream->Initialize();
    }

    void LogWorker::RegisterLogStream(const std::shared_ptr<LogStream>& stream)
    {
        if (LOG_STREAM_MAX_COUNT <= _logStreamCnt) return;
        {
            WRITE_LOCK(_logStreamMutex);
            _logStreams.emplace_back(stream);
            ++_logStreamCnt;
        }
    }

    size_t LogWorker::getRegistedLogStreamCount()
    {
        READ_LOCK(_logStreamMutex);
        return _logStreamCnt;
    }

    void LogWorker::RunThread()
    {
        _consumer->RunThread();
    }

    void LogWorker::StopThread()
    {
        _consumer->Stop();
    }

    // 작업 처리 내용을 등록합니다. 
    void LogWorker::handle(const Consumer_t::container& objs)
    {
        if (objs.empty()) return;

        for (auto& obj : objs) {
            obj->MakePreFix();
            for (auto& stream : std::as_const(_logStreams))
            {
                if (stream->EnableLogType(obj->GetLogType()))
                    stream->Write(obj);
            }
        }

        for (auto& stream : std::as_const(_logStreams))
            stream->Flush();
    }
}
