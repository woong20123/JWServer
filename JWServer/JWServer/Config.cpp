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
        _validate = true;

        _boolCheckedValues.emplace_back(L"yes", true);
        _boolCheckedValues.emplace_back(L"y", true);
        _boolCheckedValues.emplace_back(L"true", true);

        _boolCheckedValues.emplace_back(L"no", false);
        _boolCheckedValues.emplace_back(L"n", false);
        _boolCheckedValues.emplace_back(L"false", false);

    }

    bool Config::Load(std::filesystem::path filePath)
    {
        onLoading();
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

        checkAndWriteEmptyKey(filePath);

        onLoaded();

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

        ConfigMap emptyConfigMap;
        bool isChangedConfigMap{ false };
        compareConfigMapAndDefinition(isChangedConfigMap, emptyConfigMap);
        _parser->Write(filePath, emptyConfigMap);
    }

    void Config::makeDefintion()
    {
        // { key, defaultValue }를 정의 해주세요.
        RegisterConfigDefinition("server-port", L"13211");
        RegisterConfigDefinition("worker-thread", L"0");

    }

    void Config::compareConfigMapAndDefinition(bool& isChanged, ConfigMap& configMap)
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

    void Config::checkAndWriteEmptyKey(const std::filesystem::path& filePath)
    {
        bool isChangedConfigMap{ false };
        compareConfigMapAndDefinition(isChangedConfigMap, _configMap);

        if (isChangedConfigMap)
        {
            _parser->Write(filePath, _configMap);
        }
    }

    std::shared_ptr<ConfigParser> ConfigParser::CreateParser(const ParserType parserType)
    {
        switch (parserType)
        {
        case ConfigParser::ParserType::JSON:
            return std::make_shared<JsonConfigParser>();
        }

        LOG_ERROR(L"parser type is not supported, parserType:{}", static_cast<int>(parserType));
        return nullptr;
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

    const bool Config::GetBool(const std::string& key) const
    {
        const auto value = getValue(key);

        // _boolCheckedValues를 key 검색하여 일치하는 값이 있는지 확인 합니다. 
        const auto findIt = std::find_if(begin(_boolCheckedValues), end(_boolCheckedValues),
            [&value](const BoolCheckValue& checkValue)
            {
                return checkValue.first == value;
            });

        if (findIt != end(_boolCheckedValues))
        {
            return findIt->second;
        }


        // _boolCheckedValues에 등록되지 않은 값을 셋팅했다면 프로그램을 종료 시킵니다. 
        LOG_ERROR(L"Config::GetBool parser Error, key:{}, value:{}", StringConverter::StrA2WUseUTF8(key).value().c_str(), value);
        _validate = false;

        return false;
    }


    const int16_t Config::GetInt16(const std::string& key) const
    {
        return static_cast<int16_t>(GetInt64(key));
    }

    const int32_t Config::GetInt32(const std::string& key) const
    {

        return static_cast<int32_t>(GetInt64(key));
    }

    const int64_t Config::GetInt64(const std::string& key) const
    {
        auto value = getValue(key);
        int64_t result{ 0L };
        if (wcscmp(value, NONE_VALUE) != 0)
        {
            try
            {
                result = std::stoll(value);
            }
            catch (std::exception const& ex)
            {
                LOG_ERROR(L"Config::GetInt parser Error, key:{}, value:{}, ex:{}", StringConverter::StrA2WUseUTF8(key).value().c_str(), value,
                    StringConverter::StrA2WUseUTF8(ex.what()).value().c_str());
                _validate = false;
            }
        }
        return result;
    }

    const std::wstring Config::GetString(const std::string& key) const
    {
        auto value = getValue(key);
        if (wcscmp(value, NONE_VALUE) == 0)
        {
            LOG_ERROR(L"Config::GetString parser Error, key:{}, value:{}", StringConverter::StrA2WUseUTF8(key).value().c_str(), value);
            _validate = false;
        }
        return value;
    }

    const wchar_t* Config::getValue(const std::string& key) const
    {
        auto it = _configMap.find(key);
        if (it != _configMap.end())
        {
            return it->second.c_str();
        }
        return NONE_VALUE;
    }
}
