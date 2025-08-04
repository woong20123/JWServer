#include "StringConverter.h"
#include <windows.h>
#include <memory>
#include <cassert>

namespace jw
{
    // convert from LPWSTR to LPSTR
    std::optional<std::string> StringConverter::StrW2A(const std::wstring& Data, uint32_t codePage)
    {
        if (Data.length() == 0) {
            return "";
        }
        int buflen = ::WideCharToMultiByte(codePage, 0, Data.c_str(), -1, nullptr, 0, nullptr, nullptr) + 8;
        auto dest = std::make_unique<char[]>(buflen);
        int buflen2 = ::WideCharToMultiByte(codePage, 0, Data.c_str(), -1, dest.get(), buflen, NULL, NULL) + 8;
        assert(buflen == buflen2);
        if (buflen == buflen2)
            return dest.get();
        return std::nullopt;
    }

    // convert from LPSTR to LPWSTR
    std::optional<std::wstring> StringConverter::StrA2W(const std::string& Data, uint32_t codePage)
    {
        if (Data.length() == 0) {
            return L"";
        }
        auto buflen = ::MultiByteToWideChar(codePage, 0, Data.c_str(), -1, nullptr, 0) + 8;
        auto dest = std::make_unique<wchar_t[]>(buflen);
        auto buflen2 = ::MultiByteToWideChar(codePage, 0, Data.c_str(), -1, dest.get(), buflen) + 8;
        assert(buflen == buflen2);
        if (buflen == buflen2)
            return dest.get();
        return std::nullopt;
    }

    std::optional<std::string> StringConverter::StrW2AUseUTF8(const std::wstring& Data)
    {
        return StrW2A(Data, CP_UTF8);
    }
    std::optional<std::wstring> StringConverter::StrA2WUseUTF8(const std::string& Data)
    {
        return StrA2W(Data, CP_UTF8);
    }
}
