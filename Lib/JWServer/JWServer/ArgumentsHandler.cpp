#include <iostream>
#include <unordered_map>
#include "ArgumentsHandler.h"
#include "Logger.h"

namespace jw
{
    struct ArgumentsHandler::Impl
    {
        using HandlerContainer = std::unordered_map<HandlerKey, HandlerValue>;
        HandlerContainer                        handlerContainer;
        std::wstring                            processName;
    };

    ArgumentsHandler::ArgumentsHandler() : _pImpl{ std::make_unique<Impl>() }
    {

    }
    ArgumentsHandler::~ArgumentsHandler()
    {}

    void ArgumentsHandler::HandleArgument(const ArgumentContainer& arguments)
    {
        int16_t index{ 0 };
        for (const auto& arg : arguments)
        {
#if _HAS_CXX20 
            if (_pImpl->handlerContainer.contains(arg))
#else
            if (_pImpl->handlerContainer->count(arg))
#endif
            {
                _pImpl->handlerContainer[arg](index);
            }

            ++index;
        }
    }

    const std::wstring& ArgumentsHandler::GetProcessName() const
    {
        return _pImpl->processName;
    }

    void ArgumentsHandler::Initialize(const std::wstring& processName)
    {
        _pImpl->processName = processName;
        registerHandler();
    }

    bool ArgumentsHandler::addHandler(const ArgumentsHandler::HandlerKey& key, const ArgumentsHandler::HandlerValue& value)
    {
        return _pImpl->handlerContainer.emplace(key, value).second;
    }

    DefaultArgumentHandler::~DefaultArgumentHandler() {}

    void DefaultArgumentHandler::registerHandler()
    {
        addHandler(HELP_OPTION, [this](uint16_t index) {
            printHelp();
            return;
            });
    }

    void DefaultArgumentHandler::printHelp()
    {
        wprintf(L"%s 사용법\r\n", GetProcessName().c_str());
        wprintf(L"%s : 사용방법을 출력합니다.\r\n", HELP_OPTION);

        LOG_FETAL(L"{:s} 사용법 ", GetProcessName().c_str());
        LOG_FETAL(L"{:s} : 사용방법을 출력합니다.", HELP_OPTION);
    }
}