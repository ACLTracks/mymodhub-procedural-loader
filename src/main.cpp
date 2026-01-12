// src/main.cpp
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include "Log.h"
#include "Plugin.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    (void)lpReserved;

    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        {
            // If you don’t want thread attach/detach spam, keep this.
            DisableThreadLibraryCalls(hModule);
            break;
        }

        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        default:
            break;
    }

    return TRUE;
}

extern "C" __declspec(dllexport) void mmh_Start()
{
    mmh::InitLogging();
    mmh::Plugin::Init();
}
