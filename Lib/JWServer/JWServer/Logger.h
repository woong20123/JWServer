#pragma once
#ifndef __JW_LOGGER_H__
#define __JW_LOGGER_H__
#include "Singleton.hpp"
#include "Producer.hpp"
#include "TypeDefinition.h"
#include <string>
#include <memory>
#if _HAS_CXX20 
#include <format>
#endif

namespace jw
{
    enum class LogType : int16_t
    {
        LOG_FATAL,
        LOG_ERROR,
        LOG_WARN,
        LOG_INFO,
        LOG_DEBUG,
    };

    class LogBuffer;

    class Logger : public Singleton<Logger>
    {
    public:
        using ProducerObj = Producer<std::shared_ptr<LogBuffer>>;

        static constexpr size_t LOG_BUFFER_SIZE = 2000;
        using msgType = wchar_t;
        using msgString = std::wstring;

        void Initialize(const std::shared_ptr<ProducerObj>& producer);

        void SetLevel(LogType logType);
        void Stop();

        void WriteV(LogType type, const msgType* file, const int line, const msgType* fmt, va_list args) const;
        void Write(LogType type, const msgType* file, const int line, const msgType* fmt, ...) const;
        void WriteString(LogType type, const msgType* file, const int line, const msgString& stringMsg) const;

#if _HAS_CXX20 
        template <class... _Types>
        void WriteFormat(LogType type, const msgType* file, const int line, const msgType* fmt, _Types&&... args) const
        {
            if (!isEnable(type)) return;
            return WriteString(type, file, line, std::vformat(std::wstring_view(fmt), std::make_wformat_args(args...)));
        }
#endif // _HAS_CXX20

    protected:
        Logger();
        ~Logger();
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

    private:
        bool enableLogLevel(const LogType level) const;
        bool isEnable(const LogType level) const;

        struct Impl;
        std::unique_ptr<Impl> _pImpl;
        friend class Singleton<Logger>;
    };
}

#define LOGGER jw::Logger::GetInstance

#if _HAS_CXX20 
#define LOG_FETAL_STRING(fmt)           jw::Logger::GetInstance().WriteString(jw::LogType::LOG_FATAL ,__WFILE__, __LINE__, fmt)
#define LOG_ERROR_STRING(fmt)           jw::Logger::GetInstance().WriteString(jw::LogType::LOG_ERROR ,__WFILE__, __LINE__, fmt)
#define LOG_WARN_STRING(fmt)            jw::Logger::GetInstance().WriteString(jw::LogType::LOG_WARN ,__WFILE__, __LINE__, fmt)
#define LOG_INFO_STRING(fmt)            jw::Logger::GetInstance().WriteString(jw::LogType::LOG_INFO ,__WFILE__, __LINE__, fmt)
#define LOG_DEBUG_STRING(fmt)           jw::Logger::GetInstance().WriteString(jw::LogType::LOG_DEBUG ,__WFILE__, __LINE__, fmt)

#define LOG_FETAL(fmt, ...)             jw::Logger::GetInstance().WriteFormat(jw::LogType::LOG_FATAL ,__WFILE__, __LINE__, fmt, __VA_ARGS__)
#define LOG_ERROR(fmt, ...)             jw::Logger::GetInstance().WriteFormat(jw::LogType::LOG_ERROR ,__WFILE__, __LINE__, fmt, __VA_ARGS__)
#define LOG_WARN(fmt, ...)              jw::Logger::GetInstance().WriteFormat(jw::LogType::LOG_WARN ,__WFILE__, __LINE__, fmt, __VA_ARGS__)
#define LOG_INFO(fmt, ...)              jw::Logger::GetInstance().WriteFormat(jw::LogType::LOG_INFO ,__WFILE__, __LINE__, fmt, __VA_ARGS__)
#define LOG_DEBUG(fmt, ...)             jw::Logger::GetInstance().WriteFormat(jw::LogType::LOG_DEBUG ,__WFILE__, __LINE__, fmt, __VA_ARGS__)
#else
#define LOGGER_FETAL(fmt, ...)          Logger::GetInstance().Write(jw::LogType::LOG_FATAL ,__WFILE__, __LINE__, fmt, __VA_ARGS__)
#define LOGGER_ERROR(fmt, ...)          Logger::GetInstance().Write(jw::LogType::LOG_ERROR ,__WFILE__, __LINE__, fmt, __VA_ARGS__)
#define LOGGER_WARN(fmt, ...)           Logger::GetInstance().Write(jw::LogType::LOG_WARN ,__WFILE__, __LINE__, fmt, __VA_ARGS__)
#define LOGGER_INFO(fmt, ...)           Logger::GetInstance().Write(jw::LogType::LOG_INFO ,__WFILE__, __LINE__, fmt, __VA_ARGS__)
#define LOGGER_DEBUG(fmt, ...)          Logger::GetInstance().Write(jw::LogType::LOG_DEBUG ,__WFILE__, __LINE__, fmt, __VA_ARGS__)
#endif // _HAS_CXX20

#endif // __JW_LOGGER_H__
