#include "Logger.h"
#include "LogBuffer.h"
#include "LogBufferPool.h"
#include <iostream>
#include <cstdarg>
#include <Windows.h>
namespace jw
{
    Logger::Logger() :
        _isRun{ false },
#ifdef _DEBUG
        _logLevel{ LogType::LOG_DEBUG }
#else
        _logLevel{ LogType::LOG_INFO }
#endif

    {}

    Logger::~Logger()
    {}

    void Logger::Initialize(const std::shared_ptr<ProducerObj>& producer)
    {
        _logProducer = producer;
        _isRun = true;
        std::setlocale(LC_ALL, "KOREAN");
        std::wcout.imbue(std::locale(""));
    }

    void Logger::SetLevel(LogType logLevel)
    {
        _logLevel = logLevel;
    }

    void Logger::Stop()
    {
        _logProducer->SetStopSignal();
        _isRun = false;
    }

    void Logger::WriteV(LogType type, const msgType* file, const int line, const msgType* fmt, va_list args) const
    {
        if (!isEnable(type)) return;
        if (!_isRun) return;

        Logger::msgType msg[LOG_BUFFER_SIZE] = { 0, };
        vswprintf_s(msg, fmt, args);

        std::shared_ptr<LogBuffer> logBuffer{ LOG_BUFFER_POOL().Acquire(), [](LogBuffer* obj) { LOG_BUFFER_POOL().Release(obj); } };
        logBuffer->Initialize(type, file, line);
        logBuffer->WriteMsg(msg);

        // worker에 메시지를 전달합니다. 
        _logProducer->Push(logBuffer);
    }

    void Logger::Write(LogType type, const msgType* file, const int line, const msgType* fmt, ...) const
    {
        if (!isEnable(type)) return;
        if (!_isRun) return;

        va_list	args;
        va_start(args, fmt);
        WriteV(type, file, line, fmt, args);
        va_end(args);
    }


    void Logger::WriteString(LogType type, const msgType* file, const int line, const msgString& msg) const
    {
        if (!isEnable(type)) return;
        std::shared_ptr<LogBuffer> logBuffer{ LOG_BUFFER_POOL().Acquire(), [](LogBuffer* obj) { LOG_BUFFER_POOL().Release(obj); } };
        logBuffer->Initialize(type, file, line);
        logBuffer->WriteMsg(msg.c_str());

        _logProducer->Push(logBuffer);
    }

    bool Logger::isEnable(const LogType level) const
    {
        return enableLogLevel(level) && _isRun;
    }

    bool Logger::enableLogLevel(const LogType level) const
    {
        return level <= _logLevel;
    }
}