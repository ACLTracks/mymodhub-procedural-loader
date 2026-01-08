#include <SKSE/SKSE.h>
#include <spdlog/spdlog.h>

#include "Log.h"
#include "Plugin.h"

using namespace std::literals;

extern "C"
{
    __declspec(dllexport) constinit SKSE::PluginVersionData SKSEPlugin_Version =
    {
        .dataVersion = SKSE::PluginVersionData::kVersion,
        .pluginVersion = 1,
        .pluginName = "MyModHub Procedural Loader",
        .author = "ACLTracks",
        .supportEmail = "",
        .version = SKSE::PluginVersionData::VersionIndependence::AddressLibrary,
        .usesAddressLibrary = true,
        .hasNoStructUse = false
    };
}

SKSEPluginLoad(const SKSE::LoadInterface* skse)
{
    SKSE::Init(skse);

    mmh::InitLogging();
    spdlog::info("MyModHub Procedural Loader loaded");

    mmh::Plugin::Init();

    return true;
}
