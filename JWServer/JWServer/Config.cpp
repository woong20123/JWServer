#include "Config.h"
#include "Logger.h"
#include "StringConverter.h"
#include <fstream>
#include <nlohmann/json.hpp>



namespace jw
{
    void Config::Initialize(std::shared_ptr<ConfigParser> parser)
    {
        _parser = parser;
    }

    bool Config::Load(std::filesystem::path filePath)
    {
        OnLoading();
        const auto currentPath = std::filesystem::current_path();

        if (!std::filesystem::is_regular_file(filePath))
        {
            LOG_ERROR(L"path is not regular file, path:{}", filePath.c_str());
            WriteFromDefaultDefinition(filePath);
            return false;
        }

        if (!_parser)
        {
            LOG_INFO(L"parser is null");
            return false;
        }


        _parser->Parse(filePath, _configMap);

        bool isChanged{ false };
        checkDefinition(isChanged, _configMap);

        if (isChanged)
        {
            _parser->Write(filePath, _configMap);
        }

        OnLoaded();

        return true;
    }

    void Config::WriteFromDefaultDefinition(std::filesystem::path filePath)
    {
        if (!_parser)
        {
            LOG_INFO(L"parser is null");
            return;
        }
        if (filePath.empty())
        {
            LOG_ERROR(L"filePath is empty");
            return;
        }

        ConfigMap defaultConfigMap;
        bool isChanged{ false };
        checkDefinition(isChanged, defaultConfigMap);
        _parser->Write(filePath, defaultConfigMap);
    }

    void Config::makeDefintion()
    {
        // { key, defaultValue }를 정의 해주세요.
        RegisterConfigDefinition("server-port", L"13211");
        RegisterConfigDefinition("worker-thread", L"0");

    }

    void Config::checkDefinition(bool& isChanged, ConfigMap& configMap)
    {
        for (const auto& def : _configDefinition)
        {
            auto it = configMap.find(def.key);
            if (it == configMap.end())
            {
                LOG_WARN(L"key not found, key:{}, defaultValue:{}", StringConverter::StrA2WUseUTF8(def.key).value_or(L""), def.defaultValue.c_str());
                configMap[def.key] = def.defaultValue;
                isChanged = true;
            }
        }
    }

    bool JsonConfigParser::Parse(const std::filesystem::path& filePath, ConfigParser::ConfigMap& configMap)
    {
        using json = nlohmann::json;
        std::ifstream configFile(filePath);
        if (!configFile.is_open())
        {
            LOG_INFO(L"config file load fail, path:{}", filePath.c_str());
            return false;
        }

        try {
            json configJson;

            configJson = json::parse(configFile);
            for (json::iterator it = configJson.begin(); it != configJson.end(); ++it)
            {
                const auto& key = it.key().c_str();
                std::wstring value = StringConverter::StrA2WUseUTF8(it.value()).value_or(L"");;
                configMap[key] = value;
            }
        }
        catch (const json::parse_error&) {
            LOG_ERROR(L"json parse error, path:{}", filePath.c_str());
            configFile.close();
            return false;
        }

        configFile.close();

        return true;
    }

    bool JsonConfigParser::Write(const std::filesystem::path& filePath, ConfigParser::ConfigMap& configMap)
    {
        using json = nlohmann::json;
        std::wofstream configFile(filePath);
        if (!configFile.is_open())
        {
            LOG_INFO(L"config file load fail, path:{}", filePath.c_str());
            return false;
        }

        json newConfigJson;
        for (const auto& [key, value] : configMap)
        {
            newConfigJson[key] = StringConverter::StrW2AUseUTF8(value).value_or("");
        }

        std::string jsonString = newConfigJson.dump(4); // 4 spaces for indentation
        std::wstring jsonWString = StringConverter::StrA2WUseUTF8(jsonString).value_or(L"");
        configFile.clear();
        configFile << jsonWString.c_str();
        configFile.close();

        return true;
    }

    int16_t Config::GetInt16(const std::string& key)
    {
        return static_cast<int32_t>(GetInt64(key));
    }

    int32_t Config::GetInt32(const std::string& key)
    {

        return static_cast<int32_t>(GetInt64(key));
    }

    int64_t Config::GetInt64(const std::string& key)
    {
        auto it = _configMap.find(key);
        if (it != _configMap.end())
        {
            return std::stoll(it->second);
        }
        return 0L;
    }


    std::wstring Config::GetString(const std::string& key)
    {
        auto it = _configMap.find(key);
        if (it != _configMap.end())
        {
            return it->second;
        }
        return L"";
    }
}
