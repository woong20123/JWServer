#pragma once
#ifndef __JW_STRING_CONVERTER_H__
#define __JW_STRING_CONVERTER_H__
#include <optional>
#include <string>

namespace jw
{
    class StringConverter
    {
    public:
        static std::optional<std::string> StrW2A(const std::wstring& Data, uint32_t codePage);
        static std::optional<std::wstring> StrA2W(const std::string& Data, uint32_t codePage);
        static std::optional<std::string> StrW2AUseUTF8(const std::wstring& Data);
        static std::optional<std::wstring> StrA2WUseUTF8(const std::string& Data);
    };
}

#endif