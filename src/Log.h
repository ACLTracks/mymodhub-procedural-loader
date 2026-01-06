#pragma once
#include <string>

namespace mmh
{
	void InitLogging();
	void Info(const std::string& msg);
	void Warn(const std::string& msg);
	void Error(const std::string& msg);
}
