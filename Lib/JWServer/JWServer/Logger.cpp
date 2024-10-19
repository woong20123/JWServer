#include "Logger.h"
#include "LogBuffer.h"
#include "Producer.hpp"
#include <iostream>
#include <cstdarg>
namespace jw
{
    struct Logger::Impl
    {
        Impl() :
#ifdef _DEBUG
            logLevel{ LogType::LOG_DEBUG }
#else
            logLevel{ LogType::LOG_WARN }
#endif
        {}
        Producer<LogBuffer> LogWorkerProducer;
        LogType logLevel;
    };

    Logger::Logger() :
        _pImpl{std::make_unique<Impl>()}

    {}

    Logger::~Logger()
    {

    }

    void Logger::SetLevel(LogType logLevel)
    {
        _pImpl->logLevel = logLevel;
    }
    void Logger::Stop()
    {
        _pImpl->LogWorkerProducer.SetStopSignal();
    }

    void Logger::WriteV(LogType type, const msgType* file, const int line, const msgType* fmt, va_list args) const
    {
        if (!enalbleLogLevel(type)) return;
        Logger::msgType msg[LOG_BUFFER_SIZE] = { 0, };
        vsprintf_s(msg, fmt, args);

        std::shared_ptr<LogBuffer> logBuffer = std::make_shared<LogBuffer>();
        logBuffer->Initialize(type, file, line);
        logBuffer->WriteMsg(msg);

        // 다음의 동작은 logworker 스레드에서 수행해야 합니다. 
        _pImpl->LogWorkerProducer.Push(logBuffer);
        Logger::msgType prefix[LogBuffer::PRIFIX_SIZE];
        logBuffer->MakePreFix(prefix, sizeof(prefix));
        std::cout << prefix << logBuffer->GetMsg() << "\r\n";
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
        std::shared_ptr<LogBuffer> logBuffer = std::make_shared<LogBuffer>();
        logBuffer->Initialize(type, file, line);
        logBuffer->WriteMsg(msg.c_str());

        // 다음의 동작은 logworker 스레드에서 수행해야 합니다. 
        _pImpl->LogWorkerProducer.Push(logBuffer);
        Logger::msgType prefix[LogBuffer::PRIFIX_SIZE];
        logBuffer->MakePreFix(prefix, sizeof(prefix));
        std::cout << prefix << logBuffer->GetMsg() << "\r\n";
    }

    bool Logger::enalbleLogLevel(const LogType level) const
    {
        return level <= _pImpl->logLevel;
    }
}