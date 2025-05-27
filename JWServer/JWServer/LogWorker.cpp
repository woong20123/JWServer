#include "LogWorker.h"
#include "LogBuffer.h"
#include "LogStream.h"
#include "TypeDefinition.h"
#include <memory>
#include <array>

namespace jw
{
    void LogWorker::prepare()
    {
        joinWaitThread();

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

    // 작업 처리 내용을 등록합니다. 
    void LogWorker::handle(const std::list<obj>& objs)
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
