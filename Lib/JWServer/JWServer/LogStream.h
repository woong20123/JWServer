#pragma once
#ifndef __JW_LOG_STREAM_H_
#define __JW_LOG_STREAM_H_
#include <memory>
namespace jw
{
    class LogBuffer;
    class LogStream
    {
    public:
        LogStream() {};
        virtual ~LogStream() {}

        virtual void Write(const std::shared_ptr<LogBuffer>& logBuffer) = 0;
        virtual void Flush() {};
    };
};

#endif // !__JW_LOG_STREAM_H_

