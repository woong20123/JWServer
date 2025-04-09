#pragma once
#ifndef __JW_CONFIG_H__
#define __JW_CONFIG_H__

#include <filesystem>
#include <unordered_map>
#include <vector>

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
        using ConfigMap = std::unordered_map<std::string, std::wstring>;
        Config() = default;
        virtual ~Config() = default;
        virtual void Initialize(std::shared_ptr<ConfigParser> parser);
        bool Load(std::filesystem::path filePath);
        void WriteFromDefaultDefinition(std::filesystem::path filePath);

        virtual void OnLoading() = 0;
        virtual void OnLoaded() = 0;

        void RegisterConfigDefinition(const std::string& key, const std::wstring& defaultValue)
        {
            _configDefinition.emplace_back(key, defaultValue);
        }

    protected:
        std::shared_ptr<ConfigParser> _parser;

        int16_t GetInt16(const std::string& key);
        int32_t GetInt32(const std::string& key);
        int64_t GetInt64(const std::string& key);
        std::wstring GetString(const std::string& key);

    private:
        void makeDefintion();
        void checkDefinition(bool& isChanged, ConfigMap& configMap);
        std::vector<ConfigDefinition> _configDefinition;
        ConfigMap _configMap;
    };

    class ConfigParser
    {
    public:
        using ConfigMap = Config::ConfigMap;
        ConfigParser() = default;
        virtual ~ConfigParser() = default;
        virtual bool Parse(const std::filesystem::path& filePath, ConfigMap& configMap) = 0;
        virtual bool Write(const std::filesystem::path& filePath, ConfigMap& configMap) = 0;
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

