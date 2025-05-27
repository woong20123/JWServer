#pragma once
#ifndef __JW_LOGGER_H__
#define __JW_LOGGER_H__
#include "Singleton.hpp"
#include "ProducerContainer.hpp"
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
        LOG_NONE = 0,
        LOG_FATAL = 1 << 0,
        LOG_ERROR = 1 << 1,
        LOG_WARN = 1 << 2,
        LOG_INFO = 1 << 3,
        LOG_DEBUG = 1 << 4,
    };

    class LogBuffer;

    struct WrapperFormat
    {
    public:
        using view = std::wstring_view;
        using result = std::wstring;
        WrapperFormat(view fmt) : _fmt{ fmt } {};

        template <class... _Types>
        result makeFormat(_Types&&... args)
        {
            return std::vformat(std::wstring_view(_fmt), std::make_wformat_args(args...));
        }
        view _fmt;
    };

    class Logger : public Singleton<Logger>
    {
    public:

        using PContainer = ProducerContainer<std::shared_ptr<LogBuffer>>;

        static constexpr size_t LOG_BUFFER_SIZE = 2000;
        using msgType = wchar_t;
        using msgString = std::wstring;
        using msgStringView = std::wstring_view;

        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

        void Initialize(const std::shared_ptr<PContainer>& producerCon);

        void SetLevel(LogType logType);
        void Stop();

        void WriteV(LogType type, const msgType* file, const msgType* func, const int line, const msgType* fmt, va_list args) const;
        void Write(LogType type, const msgType* file, const msgType* func, const int line, const msgType* fmt, ...) const;
        void WriteString(LogType type, const msgType* file, const msgType* func, const int line, const msgStringView stringMsg) const;

#if _HAS_CXX20 
        template <class... _Types>
        void WriteFormat(LogType type, const msgType* file, const int line, const msgType* fmt, _Types&&... args) const
        {
            if (!isEnable(type)) return;
            return WriteString(type, file, line, std::vformat(std::wstring_view(fmt), std::make_wformat_args(args...)));
        }
#endif // _HAS_CXX20

        static const Logger::msgType* LogTypeToString(LogType type)
        {
            switch (type)
            {
            case LogType::LOG_FATAL:
                return L"fetal";
            case LogType::LOG_ERROR:
                return L"error";
            case LogType::LOG_WARN:
                return L"warn";
            case LogType::LOG_INFO:
                return L"info";
            case LogType::LOG_DEBUG:
                return L"debug";
            }
            return L"none";
        }

    protected:
        Logger();
        ~Logger();

    private:
        bool enableLogLevel(const LogType level) const;
        bool isEnable(const LogType level) const;

        std::shared_ptr<PContainer>     _logProducerCon;
        LogType                         _logLevel;
        bool                            _isRun;

        friend class Singleton<Logger>;

    };

    Logger& GetLogger();
}

#if _HAS_CXX20 
#define LOG_WRITE(type, fmt, ...) \
{jw::WrapperFormat wformat{fmt};\
jw::Logger::GetInstance().WriteString(type ,__WFILE__, __FUNCTIONW__, __LINE__, wformat.makeFormat(__VA_ARGS__));}

#define LOG_FETAL(fmt, ...)             LOG_WRITE(jw::LogType::LOG_FATAL, fmt, __VA_ARGS__)
#define LOG_ERROR(fmt, ...)             LOG_WRITE(jw::LogType::LOG_ERROR, fmt, __VA_ARGS__)
#define LOG_WARN(fmt, ...)              LOG_WRITE(jw::LogType::LOG_WARN, fmt, __VA_ARGS__)
#define LOG_INFO(fmt, ...)              LOG_WRITE(jw::LogType::LOG_INFO, fmt, __VA_ARGS__)
#define LOG_DEBUG(fmt, ...)             LOG_WRITE(jw::LogType::LOG_DEBUG, fmt, __VA_ARGS__)

#else
#define LOGGER_FETAL(fmt, ...)          Logger::GetInstance().Write(jw::LogType::LOG_FATAL ,__WFILE__, __LINE__, fmt, __VA_ARGS__)
#define LOGGER_ERROR(fmt, ...)          Logger::GetInstance().Write(jw::LogType::LOG_ERROR ,__WFILE__, __LINE__, fmt, __VA_ARGS__)
#define LOGGER_WARN(fmt, ...)           Logger::GetInstance().Write(jw::LogType::LOG_WARN ,__WFILE__, __LINE__, fmt, __VA_ARGS__)
#define LOGGER_INFO(fmt, ...)           Logger::GetInstance().Write(jw::LogType::LOG_INFO ,__WFILE__, __LINE__, fmt, __VA_ARGS__)
#define LOGGER_DEBUG(fmt, ...)          Logger::GetInstance().Write(jw::LogType::LOG_DEBUG ,__WFILE__, __LINE__, fmt, __VA_ARGS__)
#endif // _HAS_CXX20

#endif // __JW_LOGGER_H__
