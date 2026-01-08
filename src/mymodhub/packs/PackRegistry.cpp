#include "PackRegistry.h"
#include "JsonUtil.h"

#include <algorithm>
#include <set>

// If you already have a logger wrapper, use that.
// Otherwise spdlog is typical in SKSE loaders.
#include <spdlog/spdlog.h>

namespace mymodhub::packs
{
    PackRegistry& PackRegistry::Get()
    {
        static PackRegistry instance;
        return instance;
    }

    void PackRegistry::RefreshFromDisk(const std::filesystem::path& packsRoot)
    {
        std::lock_guard lock(_mtx);

        _packs.clear();
        _entryByKey.clear();

        if (!std::filesystem::exists(packsRoot)) {
            spdlog::info("[MyModHub] Packs root does not exist: {}", packsRoot.string());
            return;
        }

        spdlog::info("[MyModHub] Scanning packs root: {}", packsRoot.string());

        size_t loadedPacks = 0;
        size_t loadedEntries = 0;
        size_t skipped = 0;

        for (const auto& dirEnt : std::filesystem::directory_iterator(packsRoot)) {
            if (!dirEnt.is_directory()) {
                continue;
            }

            const auto packDir = dirEnt.path();
            LoadedPack pack;

            if (!TryLoadPack(packDir, pack)) {
                skipped++;
                continue;
            }

            std::string validateErr;
            if (!ValidatePack(pack, validateErr)) {
                spdlog::warn("[MyModHub] Skipping pack '{}': {}", packDir.filename().string(), validateErr);
                skipped++;
                continue;
            }

            // Register entries
            for (const auto& e : pack.index.entries) {
                const auto key = MakeEntryKey(pack.manifest.pack_id, e.id);
                if (_entryByKey.contains(key)) {
                    spdlog::warn("[MyModHub] Duplicate entry key '{}', skipping entry id='{}' in pack '{}'",
                        key, e.id, pack.manifest.pack_id);
                    continue;
                }
                _entryByKey.emplace(key, e);
                loadedEntries++;
            }

            spdlog::info("[MyModHub] Loaded pack '{}' (v{}) entries={}",
                pack.manifest.pack_id, pack.manifest.version, pack.index.entries.size());

            _packs.emplace_back(std::move(pack));
            loadedPacks++;
        }

        spdlog::info("[MyModHub] Pack scan complete. packs_loaded={} entries_loaded={} skipped={}",
            loadedPacks, loadedEntries, skipped);
    }

    std::vector<LoadedPack> PackRegistry::GetPacksSnapshot() const
    {
        std::lock_guard lock(_mtx);
        return _packs;
    }

    std::optional<PackIndexEntry> PackRegistry::FindEntry(const std::string& packId, const std::string& entryId) const
    {
        std::lock_guard lock(_mtx);
        const auto key = MakeEntryKey(packId, entryId);

        auto it = _entryByKey.find(key);
        if (it == _entryByKey.end()) {
            return std::nullopt;
        }
        return it->second;
    }

    size_t PackRegistry::PackCount() const
    {
        std::lock_guard lock(_mtx);
        return _packs.size();
    }

    size_t PackRegistry::EntryCount() const
    {
        std::lock_guard lock(_mtx);
        return _entryByKey.size();
    }

    bool PackRegistry::TryLoadPack(const std::filesystem::path& packDir, LoadedPack& outPack)
    {
        const auto manifestPath = packDir / "manifest.json";
        const auto indexPath = packDir / "index.json";

        if (!std::filesystem::exists(manifestPath) || !std::filesystem::exists(indexPath)) {
            spdlog::warn("[MyModHub] Pack missing manifest/index: {}", packDir.string());
            return false;
        }

        nlohmann::json jm;
        nlohmann::json ji;
        std::string err;

        if (!JsonUtil::LoadJsonFile(manifestPath, jm, err)) {
            spdlog::warn("[MyModHub] Failed to load manifest: {} ({})", manifestPath.string(), err);
            return false;
        }
        if (!JsonUtil::LoadJsonFile(indexPath, ji, err)) {
            spdlog::warn("[MyModHub] Failed to load index: {} ({})", indexPath.string(), err);
            return false;
        }

        PackManifest manifest;
        PackIndex index;
        if (!ParseManifest(jm, manifest, err)) {
            spdlog::warn("[MyModHub] Bad manifest in {} ({})", manifestPath.string(), err);
            return false;
        }
        if (!ParseIndex(ji, index, err)) {
            spdlog::warn("[MyModHub] Bad index in {} ({})", indexPath.string(), err);
            return false;
        }

        outPack.root_dir = packDir;
        outPack.manifest = std::move(manifest);
        outPack.index = std::move(index);
        return true;
    }

    bool PackRegistry::ValidatePack(const LoadedPack& pack, std::string& outError)
    {
        if (pack.manifest.pack_id.empty()) {
            outError = "manifest.pack_id is empty";
            return false;
        }

        // Entry id uniqueness inside pack
        std::set<std::string> ids;
        for (const auto& e : pack.index.entries) {
            if (e.id.empty()) {
                outError = "index entry missing id";
                return false;
            }
            if (!ids.insert(e.id).second) {
                outError = "duplicate entry id in pack: " + e.id;
                return false;
            }

            // File must exist relative to pack root
            if (e.file.empty()) {
                outError = "entry '" + e.id + "' missing file path";
                return false;
            }
            const auto entryPath = pack.root_dir / e.file;
            if (!std::filesystem::exists(entryPath)) {
                outError = "entry file missing for '" + e.id + "': " + entryPath.string();
                return false;
            }
        }

        return true;
    }

    bool PackRegistry::ParseManifest(const nlohmann::json& j, PackManifest& outManifest, std::string& outError)
    {
        // Minimal required fields only
        if (!j.is_object()) {
            outError = "manifest is not an object";
            return false;
        }

        auto getStr = [&](const char* key, bool required, std::string& dst) -> bool {
            if (!j.contains(key)) {
                if (required) {
                    outError = std::string("missing required key: ") + key;
                    return false;
                }
                dst.clear();
                return true;
            }
            if (!j[key].is_string()) {
                outError = std::string("key is not a string: ") + key;
                return false;
            }
            dst = j[key].get<std::string>();
            return true;
        };

        if (!getStr("pack_id", true, outManifest.pack_id)) return false;
        if (!getStr("name", true, outManifest.name)) return false;
        if (!getStr("version", true, outManifest.version)) return false;

        // Optional
        getStr("tier", false, outManifest.tier);
        getStr("author", false, outManifest.author);
        getStr("description", false, outManifest.description);

        return true;
    }

    bool PackRegistry::ParseIndex(const nlohmann::json& j, PackIndex& outIndex, std::string& outError)
    {
        if (!j.is_object()) {
            outError = "index is not an object";
            return false;
        }
        if (!j.contains("entries") || !j["entries"].is_array()) {
            outError = "index.entries missing or not an array";
            return false;
        }

        outIndex.entries.clear();
        for (const auto& je : j["entries"]) {
            if (!je.is_object()) {
                outError = "entry is not an object";
                return false;
            }

            PackIndexEntry e;

            auto reqStr = [&](const char* key, std::string& dst) -> bool {
                if (!je.contains(key) || !je[key].is_string()) {
                    outError = std::string("entry missing/invalid string key: ") + key;
                    return false;
                }
                dst = je[key].get<std::string>();
                return true;
            };

            if (!reqStr("id", e.id)) return false;
            if (!reqStr("type", e.type)) return false;
            if (!reqStr("name", e.name)) return false;
            if (!reqStr("file", e.file)) return false;

            // tags optional
            if (je.contains("tags")) {
                if (!je["tags"].is_array()) {
                    outError = "entry.tags is not an array";
                    return false;
                }
                for (const auto& t : je["tags"]) {
                    if (t.is_string()) {
                        e.tags.push_back(t.get<std::string>());
                    }
                }
            }

            outIndex.entries.push_back(std::move(e));
        }

        return true;
    }
}
