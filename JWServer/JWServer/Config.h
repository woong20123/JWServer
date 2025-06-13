#pragma once
#ifndef __JW_CONFIG_H__
#define __JW_CONFIG_H__

#include <filesystem>
#include <unordered_map>
#include <vector>
#include <string_view>

namespace jw
{
    struct ConfigDefinition
    {
        std::string key;
        std::wstring defaultValue;
    };

    class ConfigParser;
    class Config
    {
    public:
        static constexpr const wchar_t* BOOL_TRUE = L"yes";
        static constexpr const wchar_t* BOOL_FALSE = L"no";
        static constexpr const wchar_t* NONE_VALUE = L"";

        using ConfigDefinitionList = std::vector<ConfigDefinition>;
        using ConfigMap = std::unordered_map<std::string, std::wstring>;
        using BoolCheckedCon = std::unordered_map<std::wstring, bool>;

        Config() = default;
        virtual ~Config() = default;
        virtual void Initialize(std::shared_ptr<ConfigParser> parser);
        bool Load(std::filesystem::path filePath);
        void WriteFromDefaultDefinition(std::filesystem::path filePath);

        void RegisterConfigDefinition(const std::string& key, const std::wstring& defaultValue)
        {
            _configDefinition.emplace_back(key, defaultValue);
        }

        bool IsValidate() const
        {
            return _validate;
        }

    protected:
        std::shared_ptr<ConfigParser> _parser;

        const bool GetBool(const std::string& key) const;
        const int16_t GetInt16(const std::string& key) const;
        const int32_t GetInt32(const std::string& key) const;
        const int64_t GetInt64(const std::string& key) const;
        const std::wstring GetString(const std::string& key) const;

    private:
        void makeCheckedValues();

        virtual void onLoading() = 0;
        virtual void onLoaded() = 0;

        const wchar_t* getValue(const std::string& key) const;

        void checkAndWriteEmptyKey(const std::filesystem::path& filePath);

        void makeDefintion();
        void compareConfigMapAndDefinition(bool& isChanged, ConfigMap& configMap);

        ConfigDefinitionList        _configDefinition;
        ConfigMap                   _configMap;
        BoolCheckedCon              _boolCheckedContainer;
        mutable bool                _validate;
    };

    class ConfigParser
    {
    public:
        enum class ParserType
        {
            NONE = 0,
            JSON,
            MAX
        };

        using ConfigMap = Config::ConfigMap;
        ConfigParser() = default;
        virtual ~ConfigParser() = default;
        virtual bool Parse(const std::filesystem::path& filePath, ConfigMap& configMap) = 0;
        virtual bool Write(const std::filesystem::path& filePath, ConfigMap& configMap) = 0;

        // ConfigParser의 Factory 함수
        static std::shared_ptr<ConfigParser> CreateParser(const ParserType parserType);
    private:
        std::filesystem::path _filePath;
    };

    class JsonConfigParser : public ConfigParser
    {
    public:
        JsonConfigParser() = default;
        virtual ~JsonConfigParser() = default;
        bool Parse(const std::filesystem::path& filePath, ConfigMap& configMap) override;
        bool Write(const std::filesystem::path& filePath, ConfigMap& configMap) override;
    private:
        std::filesystem::path _filePath;
    };
}



#endif // __JW_CONFIG_H__

