#include "PackRegistry.h"
#include "JsonUtil.h"

#include <algorithm>
#include <set>
#include <system_error>

#include <filesystem>

// If you already have a logger wrapper, use that.
// Otherwise spdlog is typical in SKSE loaders.
#include <spdlog/spdlog.h>

namespace mymodhub::packs
{
    namespace
    {
        bool IsPathWithin(const std::filesystem::path& root, const std::filesystem::path& candidate, std::error_code& ec)
        {
            const auto canonicalCandidate = std::filesystem::weakly_canonical(candidate, ec);
            if (ec) {
                return false;
            }

            auto rootIt = root.begin();
            auto candidateIt = canonicalCandidate.begin();

            for (; rootIt != root.end(); ++rootIt, ++candidateIt) {
                if (candidateIt == canonicalCandidate.end() || *rootIt != *candidateIt) {
                    return false;
                }
            }

            return true;
        }
    }

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

        // Trust model: packs must be physical directories under packsRoot; symlinks are rejected.
        std::error_code ec;
        const auto canonicalPacksRoot = std::filesystem::weakly_canonical(packsRoot, ec);
        if (ec) {
            const auto rootEc = ec;
            const bool packsRootExists = std::filesystem::exists(packsRoot, ec);
            if (ec) {
                spdlog::warn("[MyModHub] Failed to access packs root '{}': {}", packsRoot.string(), ec.message());
            } else if (!packsRootExists) {
                spdlog::info("[MyModHub] Packs root does not exist: {}", packsRoot.string());
            } else {
                spdlog::warn("[MyModHub] Failed to canonicalize packs root '{}': {}", packsRoot.string(), rootEc.message());
            }
            return;
        }

        spdlog::info("[MyModHub] Scanning packs root: {}", canonicalPacksRoot.string());

        size_t loadedPacks = 0;
        size_t loadedEntries = 0;
        size_t skipped = 0;

        std::filesystem::directory_iterator iter(canonicalPacksRoot, ec);
        if (ec) {
            spdlog::warn("[MyModHub] Failed to open packs root '{}' for iteration: {}", canonicalPacksRoot.string(), ec.message());
            return;
        }

        for (const auto& dirEnt : iter) {
            std::error_code entryEc;
            const auto status = dirEnt.symlink_status(entryEc);
            if (entryEc) {
                spdlog::warn("[MyModHub] Failed to inspect pack candidate '{}': {}", dirEnt.path().string(), entryEc.message());
                skipped++;
                continue;
            }

            if (std::filesystem::is_symlink(status)) {
                spdlog::warn("[MyModHub] Rejecting symlinked pack directory: {}", dirEnt.path().string());
                skipped++;
                continue;
            }

            if (!std::filesystem::is_directory(status)) {
                continue;
            }

            const auto packDir = dirEnt.path();
            const auto canonicalPackDir = std::filesystem::weakly_canonical(packDir, entryEc);
            if (entryEc) {
                spdlog::warn("[MyModHub] Failed to canonicalize pack directory '{}': {}", packDir.string(), entryEc.message());
                skipped++;
                continue;
            }

            if (!IsPathWithin(canonicalPacksRoot, canonicalPackDir, entryEc)) {
                if (entryEc) {
                    spdlog::warn("[MyModHub] Failed to validate pack directory '{}': {}", packDir.string(), entryEc.message());
                } else {
                    spdlog::warn("[MyModHub] Rejecting pack directory outside packs root: {}", packDir.string());
                }
                skipped++;
                continue;
            }
            LoadedPack pack;

            if (!TryLoadPack(canonicalPackDir, pack)) {
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

        std::error_code ec;
        const bool manifestExists = std::filesystem::exists(manifestPath, ec);
        if (ec) {
            spdlog::warn("[MyModHub] Failed to access manifest '{}': {}", manifestPath.string(), ec.message());
            return false;
        }
        if (!manifestExists) {
            spdlog::warn("[MyModHub] Pack missing manifest: {}", manifestPath.string());
            return false;
        }

        const bool indexExists = std::filesystem::exists(indexPath, ec);
        if (ec) {
            spdlog::warn("[MyModHub] Failed to access index '{}': {}", indexPath.string(), ec.message());
            return false;
        }
        if (!indexExists) {
            spdlog::warn("[MyModHub] Pack missing index: {}", indexPath.string());
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
            std::error_code ec;
            if (!IsPathWithin(pack.root_dir, entryPath, ec)) {
                if (ec) {
                    outError = "failed to canonicalize entry file for '" + e.id + "': " + entryPath.string() + " (" + ec.message() + ")";
                } else {
                    outError = "entry file escapes pack root for '" + e.id + "': " + entryPath.string();
                }
                return false;
            }

            const bool entryExists = std::filesystem::exists(entryPath, ec);
            if (ec) {
                outError = "failed to access entry file for '" + e.id + "': " + entryPath.string() + " (" + ec.message() + ")";
                return false;
            }
            if (!entryExists) {
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
