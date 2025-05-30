﻿#include "Logger.h"
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

    void Logger::Initialize(const std::shared_ptr<PContainer>& producerCon)
    {
        _logProducerCon = producerCon;
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
        if (_logProducerCon)
            _logProducerCon->SetStopSignal();
        _isRun = false;
    }

    void Logger::WriteV(LogType type, const msgType* file, const msgType* func, const int line, const msgType* fmt, va_list args) const
    {
        if (!isEnable(type)) return;
        if (!_isRun) return;

        Logger::msgType msg[LOG_BUFFER_SIZE] = { 0, };
        vswprintf_s(msg, fmt, args);

        std::shared_ptr<LogBuffer> logBuffer{ LOG_BUFFER_POOL().Acquire(), [](LogBuffer* obj) { LOG_BUFFER_POOL().Release(obj); } };
        logBuffer->Initialize(type, file, func, line);
        logBuffer->WriteMsg(msg);

        // worker에 메시지를 전달합니다. 
        _logProducerCon->Push(logBuffer);
        _logProducerCon->Flush();
    }

    void Logger::Write(LogType type, const msgType* file, const msgType* func, const int line, const msgType* fmt, ...) const
    {
        if (!isEnable(type)) return;
        if (!_isRun) return;

        va_list	args;
        va_start(args, fmt);
        WriteV(type, file, func, line, fmt, args);
        va_end(args);
    }


    void Logger::WriteString(LogType type, const msgType* file, const msgType* func, const int line, const msgStringView msg) const
    {
        if (!isEnable(type)) return;
        std::shared_ptr<LogBuffer> logBuffer{ LOG_BUFFER_POOL().Acquire(), [](LogBuffer* obj) { LOG_BUFFER_POOL().Release(obj); } };
        logBuffer->Initialize(type, file, func, line);
        logBuffer->WriteMsg(msg.data());

        _logProducerCon->Push(logBuffer);
        _logProducerCon->Flush();
    }

    bool Logger::isEnable(const LogType level) const
    {
        return enableLogLevel(level) && _isRun;
    }

    bool Logger::enableLogLevel(const LogType level) const
    {
        return level <= _logLevel;
    }

    Logger& GetLogger() { return Logger::GetInstance(); }
}