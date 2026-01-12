#include "Plugin.h"
#include "Log.h"
#include "mmh/PackLoader.h"

namespace mmh
{
	namespace
	{
		bool g_initialized = false;
	}

	void Plugin::Init()
	{
		if (g_initialized)
		{
			return;
		}

		g_initialized = true;
		InitLogging();
		Info("Plugin::Init begin");

		// Scaffold behavior: scan packs and log what we see.
		auto result = mmh::PackLoader::Scan();
		Info("Pack scan complete. packs=" + std::to_string(result.packsFound) +
		     " entries=" + std::to_string(result.entriesFound));

		Info("Plugin::Init end");
	}

	void Plugin::Shutdown()
	{
		if (!g_initialized)
		{
			return;
		}

		g_initialized = false;
		Info("Plugin::Shutdown begin");
		Info("Plugin::Shutdown end");
	}
}
