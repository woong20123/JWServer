#include "Arguments.h"
#include "ArgumentsHandler.h"
#include <cassert>
#include <vector>
#include "TypeDefinition.h"

namespace jw
{
    struct Arguments::Impl
    {
        using ArgumentContainer = std::vector<std::wstring>;
        std::wstring                        processName;
        ArgumentContainer                   argument;
        std::unique_ptr<ArgumentsHandler>   handler;
    };

    Arguments::Arguments() : _pImpl{ std::make_unique<Impl>() }
    {}

    Arguments::~Arguments() {}

    void Arguments::setAgumentContainer(int argc, ArgumentType* argv[])
    {
        _pImpl->argument.reserve(argc);

        if (1 <= argc)
            makeProcessName(argv[0]);

        for (int i = 1; i < argc; ++i) {

            size_t newSize{ strlen(argv[i]) + 1 };
            size_t convertedChars{ 0 };
            wchar_t* wargv = new wchar_t[newSize];
            mbstowcs_s(&convertedChars, wargv, newSize, argv[i], _TRUNCATE);
            _pImpl->argument.emplace_back(wargv);
            delete[]wargv; wargv = nullptr;
        }
    }

    void Arguments::makeProcessName(const ArgumentType* arg)
    {
        size_t newSize{ strlen(arg) + 1 };
        size_t convertedChars{ 0 };
        wchar_t* warg = new wchar_t[newSize];
        mbstowcs_s(&convertedChars, warg, newSize, arg, _TRUNCATE);
        std::wstring executePath{ warg };
        delete[]warg; warg = nullptr;

        auto findIndex = executePath.find_last_of('\\');
        if (std::string::npos != findIndex)
            _pImpl->processName = executePath.substr(findIndex + 1, _pImpl->processName.length() - findIndex);
    }

    void Arguments::Initialize(int argc, ArgumentType* argv[])
    {
        setAgumentContainer(argc, argv);
        _pImpl->handler = std::make_unique<DefaultArgumentHandler>();
        _pImpl->handler->Initialize(_pImpl->processName);
    }

    void Arguments::HandleArgument()
    {
        _pImpl->handler->HandleArgument(_pImpl->argument);
    }

    void Arguments::SetHandler(ArgumentsHandler* handler)
    {
        assert(handler);
        _pImpl->handler.reset(handler);
        _pImpl->handler->Initialize(_pImpl->processName);
    }
}