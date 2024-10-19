#include "Arguments.h"
#include "ArgumentsHandler.h"
#include <cassert>
#include <vector>
#include <string>

namespace jw
{
    struct Arguments::Impl
    {
        using ArgumentContainer = std::vector<std::string>;
        std::string                         processName;
        ArgumentContainer        argument;
        std::unique_ptr<ArgumentsHandler>   handler;
    };

    Arguments::Arguments() : _pImpl{ std::make_unique<Impl>() }
    {}

    Arguments::~Arguments() {}

    void Arguments::setAgumentContainer(int argc, char* argv[])
    {
        _pImpl->argument.reserve(argc);

        if (argc <= 0)
            assert(false);

        makeProcessName(argv[0]);

        for (int i = 1; i < argc; ++i)
            _pImpl->argument.emplace_back(argv[i]);
    }

    void Arguments::makeProcessName(const char* arg)
    {
        std::string executePath{ arg };
        auto findIndex = executePath.find_last_of('\\');
        if (std::string::npos != findIndex)
            _pImpl->processName = executePath.substr(findIndex + 1, _pImpl->processName.length() - findIndex);
    }

    void Arguments::Initialize(int argc, char* argv[])
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