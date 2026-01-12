// src/main.cpp
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include "Plugin.h"
#include "Log.h"

// Temporary DLL entrypoint while CommonLibSSE-NG / SKSE integration is not wired up yet.
// Once SKSE is added, replace this with SKSEPlugin_Load / SKSEPlugin_Query patterns.
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    (void)hModule;
    (void)lpReserved;

    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        {
            // If you don’t want thread attach/detach spam, keep this.
            DisableThreadLibraryCalls(hModule);

            mmh::InitLogging();
            mmh::Plugin::Init();
            break;
        }

        case DLL_PROCESS_DETACH:
        {
            mmh::Plugin::Shutdown();
            break;
        }

        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        default:
            break;
    }

    return TRUE;
}
