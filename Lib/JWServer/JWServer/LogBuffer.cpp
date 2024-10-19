#include <ctime>
#include <chrono>
#include "LogBuffer.h"
#include "Logger.h"

namespace jw
{
    struct LogBufferInfo
    {
        LogBufferInfo() : type{ 0 }, filePath{ nullptr }, line{ 0 }
        {
        }
        std::chrono::system_clock::time_point   logtime;
        LogType	                                type;
        const	char* filePath;
        int				                        line;
    };

    struct LogBuffer::Impl
    {
        // LogBuffer 객체의 사이즈를 페이지 사이즈인 4KB로 고정 시킵니다. 
        static constexpr size_t MSG_SIZE = MEMORY_PAGE_SIZE - sizeof(LogBufferInfo);

        BufferType msg[MSG_SIZE] = { 0, };
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
    }

    int LogBuffer::MakePreFix(BufferType* prefixBuffer, size_t bufferSize) {
        using namespace std::chrono;

        // time
        struct tm tmLogTime;
        const time_t logTick = system_clock::to_time_t(_pImpl->info.logtime);
        localtime_s(&tmLogTime, &logTick);
        const auto millis = duration_cast<milliseconds>(_pImpl->info.logtime.time_since_epoch()) % 1000;

        // filename
        constexpr size_t MAX_PATH = 260;
        BufferType fileName[MAX_PATH] = { 0 };
        if (_pImpl->info.filePath) {
            BufferType name[MAX_PATH], ext[MAX_PATH];
            _splitpath_s(_pImpl->info.filePath, NULL, 0, NULL, 0, name, sizeof(name), ext, sizeof(ext));
            snprintf(fileName, MAX_PATH, "%s%s", name, ext);
        }

        return snprintf(prefixBuffer, bufferSize, "%04d/%02d/%02d-%02d:%02d:%02d.%03lld,%s,%d,",
            tmLogTime.tm_year + 1900, tmLogTime.tm_mon, tmLogTime.tm_mday,
            tmLogTime.tm_hour, tmLogTime.tm_min, tmLogTime.tm_sec,
            millis.count(), fileName, _pImpl->info.line);
    }

    int LogBuffer::WriteMsg(const LogBuffer::BufferType* msg)
    {
        return strncpy_s(_pImpl->msg, msg, strlen(msg));
    }

    const LogBuffer::BufferType* LogBuffer::GetMsg() const
    {
        return _pImpl->msg;
    }
    constexpr size_t LogBuffer::GetMsgTotalSize() const
    {
        return Impl::MSG_SIZE;
    }
}