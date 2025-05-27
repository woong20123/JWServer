#include "LogFileStream.h"
#include "TypeDefinition.h"
#include "TimeUtil.h"
#include <string>
#include <windows.h>
#include <filesystem>

namespace jw
{
    LogFileStream::LogFileStream(const wchar_t* path, const wchar_t* processName, size_t bufferSize) :
        _path{ path },
        _processname{ processName },
        _streamBufferPos{ 0 },
        _isSetfileName{ false },
        _streamBufferSize{ bufferSize }
    {
        _streamBuffer = new LogBuffer::BufferType[bufferSize];
    }

    LogFileStream::~LogFileStream()
    {
        if (_streamBuffer) {
            delete[] _streamBuffer;
            _streamBuffer = nullptr;
        }
    }

    void LogFileStream::Initialize() {
        if (!std::filesystem::is_directory(_path))
            makeFolder(_path);
    }

    void LogFileStream::Write(const std::shared_ptr<LogBuffer>& logBuffer)
    {
        const auto prefixLen = STRLEN(logBuffer->GetPrefix());
        const auto msgLen = STRLEN(logBuffer->GetMsg());
        const auto lineBreakLen = STRLEN(logBuffer->GetLineBreak());
        size_t totalMsgLen = prefixLen + msgLen + lineBreakLen;

        if (_streamBufferSize <= _streamBufferPos + totalMsgLen)
            Flush();

        // 메시지 복사
        STRNCPY(&_streamBuffer[_streamBufferPos], remainBuffer(), logBuffer->GetPrefix(), prefixLen);
        _streamBufferPos += prefixLen;
        STRNCPY(&_streamBuffer[_streamBufferPos], remainBuffer(), logBuffer->GetMsg(), msgLen);
        _streamBufferPos += msgLen;
        STRNCPY(&_streamBuffer[_streamBufferPos], remainBuffer(), logBuffer->GetLineBreak(), lineBreakLen);
        _streamBufferPos += lineBreakLen;
    }

    void LogFileStream::Flush()
    {
        makeFileName();
        HANDLE h{ INVALID_HANDLE_VALUE };
        constexpr int TRY_COUNT = 20;
        for (int i{ 1 }; i < TRY_COUNT; ++i)
        {
            if (INVALID_HANDLE_VALUE != (h = CreateFileW(_fullFileName.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)))
                break;
            Sleep(100);
        }

        if (INVALID_HANDLE_VALUE == h)
        {
            _streamBufferPos = 0;
            return;
        }

        DWORD written = 0;
        LONG highPos = 0;
        const auto pos = SetFilePointer(h, 0L, &highPos, FILE_END);

        int result{ 0 };
        if (pos == 0)
        {
            // 파일이 처음 생성되면 유니코드 표기 기록
            unsigned short mark = 0xFEFF;
            result = ::WriteFile(h, &mark, sizeof(mark), &written, NULL);
        }

        result = ::WriteFile(h, _streamBuffer, (DWORD)_streamBufferPos * sizeof(wchar_t), &written, NULL);
        ::CloseHandle(h);

        initBuffer();
    }
    void LogFileStream::initBuffer()
    {
        _streamBufferPos = 0;
        _streamBuffer[0] = '\0';
    }

    void LogFileStream::makeFileName()
    {
        if (!_isSetfileName)
        {
            time_t now = TimeUtil::GetCurrentTimeT();
            struct tm tmNow;
            localtime_s(&tmNow, &now);
            TimeInfo timeInfo = TimeUtil::ToTimeInfo(&tmNow);

            _fullFileName.clear();
            _fullFileName.append(_path);
            _fullFileName.append(_processname);
            _fullFileName.append(std::format(L"({},{:06})_{:02}-{:02}-{:02}_{:02}_{:02}.log", makeNowTick(&tmNow), ::GetCurrentProcessId(), timeInfo.year, timeInfo.month, timeInfo.day, timeInfo.hour, timeInfo.minute));
            _isSetfileName = true;
        }
    }

    const int64_t LogFileStream::makeNowTick(tm* ptmNow) const
    {
        return static_cast<int64_t>((ptmNow->tm_year + 1900)) * YEAR_DIGIT +
            static_cast<int64_t>((ptmNow->tm_mon + 1) * MONTH_DIGIT) +
            static_cast<int64_t>(ptmNow->tm_mday * DAY_DIGIT) +
            static_cast<int64_t>(ptmNow->tm_hour * HOUR_DIGIT) +
            ptmNow->tm_min;
    }

    void LogFileStream::makeFolder(std::wstring_view path)
    {
        if (std::filesystem::create_directory(path))
        {
            std::wcout << L"폴더 생성 완료 : " << path << '\n';
        }
        else
        {
            ASSERT_WITH_MSG(false, std::format(L"폴더 생성 실패 : {}", path));
        }
    }
}