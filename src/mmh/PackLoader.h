#pragma once
#include <cstdint>

namespace mmh
{
	struct ScanResult
	{
		std::uint32_t packsFound{ 0 };
		std::uint32_t entriesFound{ 0 };
	};

	class PackLoader
	{
	public:
		static ScanResult Scan();
	};
}
