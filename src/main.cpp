// src/main.cpp

#include <cstdint>
#include <string_view>

#include <SKSE/SKSE.h>

#include "Log.h"
#include "Plugin.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

using namespace std::literals;

// This defines SKSEPlugin_Version (AE) AND SKSEPlugin_Query (SE/VR) for multi-runtime builds.
// Uses Address Library version-independence.
SKSEPluginInfo(
    .Version = "0.1.0.0"_v,
    .Name = "mymodhub-procedural-loader",
    .Author = "ACLTracks",
    .RuntimeCompatibility = SKSE::VersionIndependence::AddressLibrary
)

// Keep DllMain trivial to obey loader-lock rules.
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    (void)lpReserved;

    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
    }

    return TRUE;
}

// SKSE calls this outside loader lock. Put real init here.
extern "C" __declspec(dllexport) bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* skse)
{
    SKSE::Init(skse);

    mmh::InitLogging();
    mmh::Plugin::Init();

    return true;
}
