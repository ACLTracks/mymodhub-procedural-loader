#include "Plugin.h"

// NOTE: This is NOT a proper SKSE entrypoint yet.
// It just proves the project builds and runs initialization in a controlled test harness.
// Next step is wiring CommonLibSSE NG and SKSEPlugin_Load.

BOOL APIENTRY DllMain(HMODULE, DWORD reason, LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH) {
		mmh::Plugin::Init();
	}
	return TRUE;
}
