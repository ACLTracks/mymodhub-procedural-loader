#include "Plugin.h"
#include "Log.h"
#include "mmh/PackLoader.h"
#include <atomic>

namespace mmh
{
	namespace
	{
		std::atomic_bool g_initialized{false};
	}

	void Plugin::Init()
	{
		bool expected = false;
		if (!g_initialized.compare_exchange_strong(expected, true, std::memory_order_acq_rel))
		{
			return;
		}

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
		bool expected = true;
		if (!g_initialized.compare_exchange_strong(expected, false, std::memory_order_acq_rel))
		{
			return;
		}

		Info("Plugin::Shutdown begin");

		// TODO: real cleanup later (free resources, close handles, etc.)

		Info("Plugin::Shutdown begin");
		Info("Plugin::Shutdown end");
	}
}
