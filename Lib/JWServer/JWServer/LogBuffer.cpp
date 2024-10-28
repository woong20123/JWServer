#include <ctime>
#include <chrono>
#include "LogBuffer.h"
#include "Logger.h"
#include "TypeDefinition.h"
#include <iostream>


namespace jw
{
    struct LogBufferInfo
    {
        LogBufferInfo() : type{ 0 }, filePath{ nullptr }, line{ 0 }
        {
        }
        std::chrono::system_clock::time_point   logtime;
        LogType	                                type;
        const LogBuffer::BufferType* filePath;
        int				                        line;
    };

    struct LogBuffer::Impl
    {
        // LogBuffer 객체의 사이즈를 페이지 사이즈인 4KB로 고정 시킵니다. 
        static constexpr size_t MSG_SIZE = MEMORY_PAGE_SIZE - sizeof(LogBufferInfo) - (PRIFIX_SIZE * sizeof(BufferType)) - (SUFFIX_SIZE * sizeof(BufferType));
        BufferType prefix[PRIFIX_SIZE] = { 0 };
        BufferType msg[MSG_SIZE] = { 0 };
        BufferType suffix[MSG_SIZE] = { 0 };
        LogBufferInfo info;
    };

    LogBuffer::LogBuffer() : _pImpl{ std::make_unique<Impl>() }
    {
    }

    LogBuffer::~LogBuffer()
    {
    }

    void LogBuffer::Initialize(LogType logType, const BufferType* filePath, int line)
    {
        _pImpl->info.logtime = std::chrono::system_clock::now();
        _pImpl->info.type = static_cast<decltype(_pImpl->info.type)>(logType);
        _pImpl->info.filePath = filePath;
        _pImpl->info.line = line;
        STRNCPY(_pImpl->suffix, countof(_pImpl->suffix), L"\r\n", wcslen(L"\r\n"));
    }

    int LogBuffer::MakePreFix() {
        using namespace std::chrono;

        // time
        struct tm tmLogTime;
        const time_t logTick = system_clock::to_time_t(_pImpl->info.logtime);
        localtime_s(&tmLogTime, &logTick);
        const auto millis = duration_cast<milliseconds>(_pImpl->info.logtime.time_since_epoch()) % 1000;

        // filename
        constexpr size_t MAX_PATH = 256;
        BufferType fileName[MAX_PATH] = { 0 };
        if (_pImpl->info.filePath) {
            BufferType name[MAX_PATH], ext[MAX_PATH];
            errno_t err = _wsplitpath_s(_pImpl->info.filePath, NULL, 0, NULL, 0, name, countof(name), ext, countof(ext));
            _snwprintf_s(fileName, MAX_PATH, L"%s%s", name, ext);
        }

        return _snwprintf_s(_pImpl->prefix, PRIFIX_SIZE, L"%04d/%02d/%02d-%02d:%02d:%02d.%03lld,%s,%d,",
            tmLogTime.tm_year + 1900, tmLogTime.tm_mon + 1, tmLogTime.tm_mday,
            tmLogTime.tm_hour, tmLogTime.tm_min, tmLogTime.tm_sec,
            millis.count(), fileName, _pImpl->info.line);
    }

    int LogBuffer::WriteMsg(const LogBuffer::BufferType* msg)
    {
        size_t pos{ 0 };
        return STRNCPY(_pImpl->msg, countof(_pImpl->msg), msg, wcslen(msg));
    }

    const LogBuffer::BufferType* LogBuffer::GetPrefix() const
    {
        return _pImpl->prefix;
    }

    const LogBuffer::BufferType* LogBuffer::GetMsg() const
    {
        return _pImpl->msg;
    }

    const LogBuffer::BufferType* LogBuffer::GetSuffix() const
    {
        return _pImpl->suffix;
    }

    constexpr size_t LogBuffer::GetMsgTotalSize() const
    {
        return Impl::MSG_SIZE;
    }
}