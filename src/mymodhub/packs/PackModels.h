#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>

namespace mymodhub::packs
{
    struct PackManifest
    {
        std::string pack_id;     // stable id, folder-independent if possible
        std::string name;
        std::string version;
        std::string tier;        // featured | experimental (string for now)
        std::string author;
        std::string description;
    };

    struct PackIndexEntry
    {
        std::string id;          // entry id inside pack
        std::string type;        // racemenu.morph, etc
        std::string name;
        std::vector<std::string> tags;
        std::string file;        // relative path like entries/soft_nord_glow.json
    };

    struct PackIndex
    {
        std::vector<PackIndexEntry> entries;
    };

    struct LoadedPack
    {
        std::filesystem::path root_dir;   // absolute folder path
        PackManifest manifest;
        PackIndex index;
    };

    // Unique runtime key for entries to prevent collisions across packs
    // pack_id:entry_id is enough and human readable.
    inline std::string MakeEntryKey(const std::string& packId, const std::string& entryId)
    {
        return packId + ":" + entryId;
    }
}
