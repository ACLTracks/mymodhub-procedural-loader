#include "JsonUtil.h"

#include <fstream>
#include <sstream>

namespace mymodhub::packs
{
    bool JsonUtil::ReadFileText(const std::filesystem::path& path, std::string& outText)
    {
        std::ifstream file(path, std::ios::in | std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        std::ostringstream ss;
        ss << file.rdbuf();
        outText = ss.str();
        return true;
    }

    bool JsonUtil::ParseJson(const std::string& text, nlohmann::json& outJson, std::string& outError)
    {
        try {
            outJson = nlohmann::json::parse(text);
            return true;
        } catch (const std::exception& e) {
            outError = e.what();
            return false;
        }
    }

    bool JsonUtil::LoadJsonFile(const std::filesystem::path& path, nlohmann::json& outJson, std::string& outError)
    {
        std::string text;
        if (!ReadFileText(path, text)) {
            outError = "Failed to read file";
            return false;
        }
        return ParseJson(text, outJson, outError);
    }
}
