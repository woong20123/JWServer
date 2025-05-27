#include "Arguments.h"
#include "ArgumentsHandler.h"
#include <cassert>
#include <vector>
#include "TypeDefinition.h"
#include "Logger.h"

namespace jw
{
    struct Arguments::Impl
    {
        using ArgumentContainer = std::vector<std::wstring>;
        std::wstring                        _processFullName;
        std::wstring                        _processName;
        ArgumentContainer                   _argument;
        std::unique_ptr<ArgumentsHandler>   _handler;
        std::filesystem::path               _path;
    };

    Arguments::Arguments() : _pImpl{ std::make_unique<Impl>() }
    {}

    Arguments::~Arguments() {}

    void Arguments::setAgumentContainer(int argc, ArgumentType* argv[])
    {
        _pImpl->_argument.reserve(argc);

        if (1 <= argc)
            makeProcessName(argv[0]);

        for (int i = 1; i < argc; ++i) {

            size_t newSize{ strlen(argv[i]) + 1 };
            size_t convertedChars{ 0 };
            wchar_t* wargv = new wchar_t[newSize];
            mbstowcs_s(&convertedChars, wargv, newSize, argv[i], _TRUNCATE);
            _pImpl->_argument.emplace_back(wargv);
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
        if (std::string::npos != findIndex) {
            _pImpl->_processFullName = executePath.substr(findIndex + 1, _pImpl->_processName.length() - findIndex);
            auto dotIndex = _pImpl->_processFullName.find_last_of('.');
            if (std::string::npos != dotIndex) {
                _pImpl->_processName = _pImpl->_processFullName.substr(0, dotIndex);
            }
        }
    }

    void Arguments::Initialize(int argc, ArgumentType* argv[])
    {
        setAgumentContainer(argc, argv);
        _pImpl->_handler = std::make_unique<DefaultArgumentHandler>();
        _pImpl->_handler->Initialize(_pImpl->_processName);
    }

    void Arguments::HandleArgument()
    {
        _pImpl->_handler->HandleArgument(_pImpl->_argument);
    }

    void Arguments::SetHandler(ArgumentsHandler* handler)
    {
        assert(handler);
        _pImpl->_handler.reset(handler);
        _pImpl->_handler->Initialize(_pImpl->_processName);
    }

    void Arguments::SetPath(std::filesystem::path path)
    {
        _pImpl->_path = path;
        if (!std::filesystem::is_directory(path))
        {
            LOG_ERROR(L"path is not directory, path:{}", path.c_str());
            return;
        }
        std::filesystem::current_path(path);
    }

    std::filesystem::path Arguments::GetPath() const
    {
        return _pImpl->_path;
    }

    std::wstring Arguments::getProcessName() const
    {
        return _pImpl->_processName;
    }

    Arguments& GetArguments()
    {
        return Arguments::GetInstance();
    }
}