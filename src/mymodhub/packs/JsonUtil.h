#pragma once

#include <filesystem>
#include <string>

#include <nlohmann/json.hpp>

namespace mymodhub::packs
{
    class JsonUtil
    {
    public:
        static bool ReadFileText(const std::filesystem::path& path, std::string& outText);
        static bool ParseJson(const std::string& text, nlohmann::json& outJson, std::string& outError);
        static bool LoadJsonFile(const std::filesystem::path& path, nlohmann::json& outJson, std::string& outError);
    };
}
