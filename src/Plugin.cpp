#include "Plugin.h"
#include "Log.h"
#include "mmh/PackLoader.h"

namespace mmh
{
	void Plugin::Init()
	{
		InitLogging();
		Info("Plugin::Init begin");

		// Scaffold behavior: scan packs and log what we see.
		auto result = mmh::PackLoader::Scan();
		Info("Pack scan complete. packs=" + std::to_string(result.packsFound) +
		     " entries=" + std::to_string(result.entriesFound));

		Info("Plugin::Init end");
	}
}
