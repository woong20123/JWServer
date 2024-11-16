#pragma once
#ifndef __JW_LOG_BUFFER_H__
#define __JW_LOG_BUFFER_H__
#include <memory>
#include <chrono>

namespace jw
{
    enum class LogType : int16_t;

    struct LogBufferInfo
    {
        using BufferType = wchar_t;
        LogBufferInfo() : 
            type{ 0 }, 
            filePath{ nullptr },
            func{nullptr},
            line{ 0 }
        {
        }
        std::chrono::system_clock::time_point   logtime;
        LogType	                                type;
        const BufferType*                       filePath;
        const BufferType*                       func;
        int				                        line;
    };

    class LogBuffer
    {
    private:

    public:
        using BufferType = wchar_t;
        // logBuffer의 사이즈는 페이지의 크기인 4KB를 기준으로 설정합니다. 
        static constexpr size_t LOG_BUFFER_SIZE = 1024 * 4;
        static constexpr size_t PRIFIX_COUNT = 128;
        static constexpr size_t PRIFIX_SIZE = (PRIFIX_COUNT * sizeof(BufferType));
        static constexpr size_t SUFFIX_COUNT = 32;
        static constexpr size_t SUFFIX_SIZE = (SUFFIX_COUNT * sizeof(BufferType));
        // 메시지의 크기는 LOG_BUFFER_SIZE를 기준으로 구합니다. 
        static constexpr size_t MSG_SIZE = LOG_BUFFER_SIZE - sizeof(LogBufferInfo) - PRIFIX_SIZE - SUFFIX_SIZE;
        static constexpr size_t MSG_COUNT = MSG_SIZE / sizeof(BufferType);

        static constexpr const BufferType* LINE_BREAK = L"\r\n";

        LogBuffer();
        ~LogBuffer();
        void Initialize(LogType logType, const BufferType* filePath, const BufferType* func, int line);

        int MakePreFix();
        int WriteMsg(const BufferType* msg);
        const BufferType* GetPrefix() const;
        const BufferType* GetMsg() const;
        const BufferType* GetSuffix() const;
        const BufferType* GetLineBreak() const;
        constexpr size_t GetMsgTotalSize() const;
        const LogType GetLogType() const;

    private:

        BufferType      _prefix[PRIFIX_COUNT] = { 0 };
        BufferType      _msg[MSG_COUNT] = { 0 };
        BufferType      _suffix[SUFFIX_COUNT] = { 0 };
        LogBufferInfo   _info;
    };
}
#endif // __JW_LOG_BUFFER_H__

