#pragma once

#include "PackModels.h"

#include <mutex>
#include <optional>
#include <unordered_map>

namespace mymodhub::packs
{
    class PackRegistry
    {
    public:
        static PackRegistry& Get();

        // Scans folders and loads packs into memory
        void RefreshFromDisk(const std::filesystem::path& packsRoot);

        // Read-only accessors
        std::vector<LoadedPack> GetPacksSnapshot() const;
        std::optional<PackIndexEntry> FindEntry(const std::string& packId, const std::string& entryId) const;

        // Debug counters
        size_t PackCount() const;
        size_t EntryCount() const;

    private:
        PackRegistry() = default;

        static bool TryLoadPack(const std::filesystem::path& packDir, LoadedPack& outPack);

        // Minimal validation rules for now
        static bool ValidatePack(const LoadedPack& pack, std::string& outError);

        // Parse helpers
        static bool ParseManifest(const nlohmann::json& j, PackManifest& outManifest, std::string& outError);
        static bool ParseIndex(const nlohmann::json& j, PackIndex& outIndex, std::string& outError);

    private:
        mutable std::mutex _mtx;
        std::vector<LoadedPack> _packs;

        // Fast lookup
        std::unordered_map<std::string, PackIndexEntry> _entryByKey;
    };
}
