#include <ctime>

#include "LogBuffer.h"
#include "Logger.h"
#include "TypeDefinition.h"
#include <iostream>


namespace jw
{
    LogBuffer::LogBuffer()
    {}

    LogBuffer::~LogBuffer()
    {}

    void LogBuffer::Initialize(LogType logType, const BufferType* filePath, int line)
    {
        _info.logtime = std::chrono::system_clock::now();
        _info.type = static_cast<decltype(_info.type)>(logType);
        _info.filePath = filePath;
        _info.line = line;
        STRNCPY(_suffix, countof(_suffix), L"\r\n", wcslen(L"\r\n"));
    }

    int LogBuffer::MakePreFix() {
        using namespace std::chrono;

        // time
        struct tm tmLogTime;
        const time_t logTick = system_clock::to_time_t(_info.logtime);
        localtime_s(&tmLogTime, &logTick);
        const auto millis = duration_cast<milliseconds>(_info.logtime.time_since_epoch()) % 1000;

        // filename
        constexpr size_t MAX_PATH = 256;
        BufferType fileName[MAX_PATH] = { 0 };
        if (_info.filePath) {
            BufferType name[MAX_PATH], ext[MAX_PATH];
            errno_t err = _wsplitpath_s(_info.filePath, NULL, 0, NULL, 0, name, countof(name), ext, countof(ext));
            _snwprintf_s(fileName, MAX_PATH, L"%s%s", name, ext);
        }

        return _snwprintf_s(_prefix, PRIFIX_COUNT, L"%04d/%02d/%02d-%02d:%02d:%02d.%03lld,%s,%d,(%s),",
            tmLogTime.tm_year + 1900, tmLogTime.tm_mon + 1, tmLogTime.tm_mday,
            tmLogTime.tm_hour, tmLogTime.tm_min, tmLogTime.tm_sec,
            millis.count(), fileName, _info.line, Logger::LogTypeToString(_info.type));
    }

    int LogBuffer::WriteMsg(const LogBuffer::BufferType* msg)
    {
        size_t pos{ 0 };
        return STRNCPY(_msg, countof(_msg), msg, wcslen(msg));
    }

    const LogBuffer::BufferType* LogBuffer::GetPrefix() const
    {
        return _prefix;
    }

    const LogBuffer::BufferType* LogBuffer::GetMsg() const
    {
        return _msg;
    }

    const LogBuffer::BufferType* LogBuffer::GetSuffix() const
    {
        return _suffix;
    }
    const LogBuffer::BufferType* LogBuffer::GetLineBreak() const
    {
        return LINE_BREAK;
    }

    const LogType LogBuffer::GetLogType() const
    {
        return _info.type;
    }

    constexpr size_t LogBuffer::GetMsgTotalSize() const
    {
        return MSG_SIZE;
    }
}
