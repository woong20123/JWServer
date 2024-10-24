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
        static constexpr size_t MEMORY_PAGE_SIZE = 1024 * 4;
        static constexpr size_t PRIFIX_SIZE = 128;
        static constexpr size_t SUFFIX_SIZE = 16;

        LogBuffer();
        ~LogBuffer();
        using BufferType = char;
        void Initialize(LogType logType, const BufferType* filePath, int line);

        int MakePreFix();
        void WriteMsg(const BufferType* msg);
        const BufferType* GetPrefix() const;
        const BufferType* GetMsg() const;
        constexpr size_t GetMsgTotalSize() const;

    private:
        struct Impl;
        std::unique_ptr<Impl> _pImpl;
    };
}
#endif // !__JW_LOG_BUFFER_H__

