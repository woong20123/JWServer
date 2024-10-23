#pragma once
#ifndef __WLOGGER_H_
#define __WLOGGER_H_
#include "Singleton.hpp"
#include "Producer.hpp"
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
        using msgType = char;
        using msgString = std::string;

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
            return WriteString(type, file, line, std::vformat(std::string_view(fmt), std::make_format_args(args...)));
        }
#endif

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

#if _HAS_CXX20 
#define LOGGER_FETAL(fmt, ...)          Logger::GetInstance().WriteFormat(LogType::LOG_FATAL ,__FILE__, __LINE__, fmt, __VA_ARGS__)
#define LOGGER_ERROR(fmt, ...)          Logger::GetInstance().WriteFormat(LogType::LOG_ERROR ,__FILE__, __LINE__, fmt, __VA_ARGS__)
#define LOGGER_WARN(fmt, ...)           Logger::GetInstance().WriteFormat(LogType::LOG_WARN ,__FILE__, __LINE__, fmt, __VA_ARGS__)
#define LOGGER_INFO(fmt, ...)           Logger::GetInstance().WriteFormat(LogType::LOG_INFO ,__FILE__, __LINE__, fmt, __VA_ARGS__)
#define LOGGER_DEBUG(fmt, ...)          Logger::GetInstance().WriteFormat(LogType::LOG_DEBUG ,__FILE__, __LINE__, fmt, __VA_ARGS__)
#else
#define LOGGER_FETAL(fmt, ...)          Logger::GetInstance().Write(LogType::LOG_FATAL ,__FILE__, __LINE__, fmt, __VA_ARGS__)
#define LOGGER_ERROR(fmt, ...)          Logger::GetInstance().Write(LogType::LOG_ERROR ,__FILE__, __LINE__, fmt, __VA_ARGS__)
#define LOGGER_WARN(fmt, ...)           Logger::GetInstance().Write(LogType::LOG_WARN ,__FILE__, __LINE__, fmt, __VA_ARGS__)
#define LOGGER_INFO(fmt, ...)           Logger::GetInstance().Write(LogType::LOG_INFO ,__FILE__, __LINE__, fmt, __VA_ARGS__)
#define LOGGER_DEBUG(fmt, ...)          Logger::GetInstance().Write(LogType::LOG_DEBUG ,__FILE__, __LINE__, fmt, __VA_ARGS__)
#endif

#endif
