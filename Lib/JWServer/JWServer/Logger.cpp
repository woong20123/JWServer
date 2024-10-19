#include "Logger.h"
#include "LogBuffer.h"
#include <iostream>
#include <cstdarg>
namespace jw
{
    Logger::Logger() :
#ifdef _DEBUG
        _logLevel{ LogType::LOG_DEBUG }
#else
        _logLevel{ LogType::LOG_WARN }
#endif
    {}

    void Logger::SetLevel(LogType logLevel)
    {
        _logLevel = logLevel;
    }

    void Logger::WriteV(LogType type, const msgType* file, const int line, const msgType* fmt, va_list args) const
    {
        if (!enalbleLogLevel(type)) return;
        Logger::msgType msg[LOG_BUFFER_SIZE] = { 0, };
        vsprintf_s(msg, fmt, args);

        LogBuffer logBuffer;
        logBuffer.Initialize(type, file, line);
        logBuffer.WriteMsg(msg);

        // 다음의 동작은 logworker 스레드에서 수행해야 합니다. 
        Logger::msgType prefix[LogBuffer::PRIFIX_SIZE];
        logBuffer.MakePreFix(prefix, sizeof(prefix));
        std::cout << prefix << logBuffer.GetMsg() << "\r\n";
    }

    void Logger::Write(LogType type, const msgType* file, const int line, const msgType* fmt, ...) const
    {
        if (!enalbleLogLevel(type)) return;
        va_list	args;
        va_start(args, fmt);
        WriteV(type, file, line, fmt, args);
        va_end(args);
    }


    void Logger::WriteString(LogType type, const msgType* file, const int line, const msgString& msg) const
    {
        if (!enalbleLogLevel(type)) return;
        LogBuffer* logBuffer = new LogBuffer;
        logBuffer->Initialize(type, file, line);
        logBuffer->WriteMsg(msg.c_str());

        // 다음의 동작은 logworker 스레드에서 수행해야 합니다. 
        Logger::msgType prefix[LogBuffer::PRIFIX_SIZE];
        logBuffer->MakePreFix(prefix, sizeof(prefix));
        std::cout << prefix << logBuffer->GetMsg() << "\r\n";
        delete logBuffer;
    }

    bool Logger::enalbleLogLevel(const LogType level) const
    {
        return level <= _logLevel;
    }
}