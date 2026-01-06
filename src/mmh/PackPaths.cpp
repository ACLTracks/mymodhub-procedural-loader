#include "PackPaths.h"

namespace mmh
{
	std::vector<std::string> PackPaths::DefaultRoots()
	{
		// Planned: Data/MyModHub/ProceduralPacks
		// Scaffold: user can drop a test folder path in later.
		return {
			"Data/MyModHub/ProceduralPacks"
		};
	}
}
