#pragma once
#ifndef __JW_LOG_STREAM_H_
#define __JW_LOG_STREAM_H_
#include <memory>
#include <span>
namespace jw
{
    enum class LogType : int16_t;
    class LogBuffer;
    class LogStream
    {
    public:
        LogStream();
        virtual ~LogStream() {};

        void OnLogTypeFlag(const LogType flag);
        void OnLogTypeFlags(const std::span<LogType> flags);
        void OffLogTypeFlag(const LogType flag);
        void OffLogTypeFlags(const std::span<LogType> flags);
        bool EnableLogType(const LogType flag);
        virtual void Initialize() {}
        virtual void Write(const std::shared_ptr<LogBuffer>& logBuffer) = 0;
        virtual void Flush() {};
    private:
        int16_t _logTypeFlag;
    };
};

#endif // !__JW_LOG_STREAM_H_

