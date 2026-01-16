#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

// Add these includes to be safe
#include <cstdint>
#include <array>
#include <string_view>

#include "Plugin.h"
#include "Log.h"

#include <SKSE/SKSE.h>

using namespace std::literals;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    (void)lpReserved;
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
    }
    return TRUE;
}

extern "C"
{
    // CommonLibSSE-NG / SKSE plugin version data
    __declspec(dllexport) constinit auto SKSEPlugin_Version =
        []() {
            SKSE::PluginVersionData v{};
            v.PluginName("mymodhub-procedural-loader"sv);
            v.AuthorName("ACLTracks"sv);
            
            // FIX: Use brace initialization for version
            v.PluginVersion({ 0, 1, 0, 0 });
            
            // FIX: Use specific runtime version or remove CompatibleVersions if you depend on Address Library
            v.CompatibleVersions({ SKSE::RUNTIME_1_6_1170 });
            
            v.UsesAddressLibrary(true);
            return v;
        }();

    __declspec(dllexport) bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* skse)
    {
        SKSE::Init(skse);
        mmh::InitLogging();
        mmh::Plugin::Init();
        return true;
    }
}
