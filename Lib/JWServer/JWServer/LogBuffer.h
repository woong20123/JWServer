#pragma once
#ifndef __JW_LOG_BUFFER_H__
#define __JW_LOG_BUFFER_H__
#include <memory>

namespace jw
{
    enum class LogType : int16_t;

    class LogBuffer
    {
    public:
        LogBuffer();
        ~LogBuffer();
        using BufferType = wchar_t;
        void Initialize(LogType logType, const BufferType* filePath, int line);

        int MakePreFix();
        int WriteMsg(const BufferType* msg);
        const BufferType* GetPrefix() const;
        const BufferType* GetMsg() const;
        const BufferType* GetSuffix() const;
        const BufferType* GetLineBreak() const;
        constexpr size_t GetMsgTotalSize() const;

    private:
        struct Impl;
        std::unique_ptr<Impl> _pImpl;
    };
}
#endif // __JW_LOG_BUFFER_H__

