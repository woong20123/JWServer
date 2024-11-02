#include "LogWorker.h"
#include "LogBuffer.h"
#include "LogStream.h"
#include <memory>
#include <array>

namespace jw
{
    void LogWorker::prepare()
    {
        for (const auto& stream : _logStreams)
            stream->Initialize();
    }

    void LogWorker::RegisterLogStream(const std::shared_ptr<LogStream>& stream)
    {
        if (LOG_STREAM_MAX_COUNT <= _logStreamCnt) return;

        _logStreams.push_back(stream);
        ++_logStreamCnt;
    }

    // 작업 처리 내용을 등록합니다. 
    void LogWorker::handle(const std::list<queueObject>& objs)
    {
        if (objs.empty()) return;

        for (const auto& obj : objs) {
            obj->MakePreFix();
            for (const auto& stream : _logStreams)
            {
                if(stream->EnableLogType(obj->GetLogType()))
                    stream->Write(obj);
            }
        }

        for (const auto& stream : _logStreams)
            stream->Flush();
    }
}
