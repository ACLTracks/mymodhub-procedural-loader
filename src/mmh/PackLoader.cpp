#include "PackLoader.h"
#include "PackPaths.h"
#include "../Log.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace mmh
{
	ScanResult PackLoader::Scan()
	{
		ScanResult out{};

		for (const auto& root : PackPaths::DefaultRoots()) {
			Info("Scanning root: " + root);

			std::error_code ec;
			if (!fs::exists(root, ec)) {
				Warn("Root not found: " + root);
				continue;
			}

			// Look for */manifest.json
			for (const auto& dir : fs::directory_iterator(root, ec)) {
				if (ec) {
					Warn("Directory iterator error: " + root);
					break;
				}

				if (!dir.is_directory()) {
					continue;
				}

				auto manifest = dir.path() / "manifest.json";
				if (fs::exists(manifest, ec)) {
					out.packsFound++;
					Info("Found pack: " + dir.path().string());

					// Naive count entries by scanning entries/*.json
					auto entriesDir = dir.path() / "entries";
					if (fs::exists(entriesDir, ec) && fs::is_directory(entriesDir, ec)) {
						for (const auto& f : fs::directory_iterator(entriesDir, ec)) {
							if (f.is_regular_file() && f.path().extension() == ".json") {
								out.entriesFound++;
							}
						}
					}
				}
			}
		}

		return out;
	}
}
