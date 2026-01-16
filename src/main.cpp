// src/main.cpp

// CommonLibSSE-NG MUST be included before any Windows API headers
#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

// Your headers
#include "Plugin.h"
#include "Log.h"

// If you truly need Windows.h, include it AFTER CommonLib
// You only need this for DllMain + DisableThreadLibraryCalls.
// If you want, we can remove Windows.h entirely and use the Win32 types via forward decls,
// but this version keeps it simple and correct.
#ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
  #define NOMINMAX
#endif
#include <Windows.h>

using namespace std::literals;

// Keep DllMain trivial to obey loader-lock rules.
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    (void)lpReserved;

    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
    }

    return TRUE;
}

extern "C"
{
    // CommonLibSSE-NG / SKSE plugin version data (AE safe).
    __declspec(dllexport) constinit auto SKSEPlugin_Version =
        []() {
            SKSE::PluginVersionData v{};
            v.PluginName("mymodhub-procedural-loader"sv);
            v.AuthorName("ACLTracks"sv);
            v.PluginVersion(0, 1, 0, 0);
            v.UsesAddressLibrary(true);
            v.CompatibleVersions({ SKSE::RUNTIME_LATEST });
            return v;
        }();

    // SKSE calls this outside loader lock. Put real init here.
    __declspec(dllexport) bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* skse)
    {
        SKSE::Init(skse);

        mmh::InitLogging();
        mmh::Plugin::Init();

        return true;
    }
}
