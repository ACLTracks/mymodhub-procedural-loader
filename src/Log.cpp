#include "Log.h"
#include <fstream>
#include <mutex>
#include <windows.h>

namespace
{
	std::mutex gLogMutex;
	std::ofstream gLog;
}

namespace mmh
{
	void InitLogging()
	{
		std::lock_guard<std::mutex> lock(gLogMutex);

		if (gLog.is_open()) {
			return;
		}

		// Minimal file logger for scaffold.
		// Later we swap to spdlog via CommonLibSSE NG.
		char path[MAX_PATH]{};
		GetTempPathA(MAX_PATH, path);

		std::string file = std::string(path) + "mymodhub-procedural-loader.log";
		gLog.open(file, std::ios::out | std::ios::app);

		Info("Logging started.");
	}

	static void Write(const char* level, const std::string& msg)
	{
		std::lock_guard<std::mutex> lock(gLogMutex);
		if (!gLog.is_open()) {
			return;
		}
		gLog << "[" << level << "] " << msg << "\n";
		gLog.flush();
	}

	void Info(const std::string& msg)  { Write("INFO", msg); }
	void Warn(const std::string& msg)  { Write("WARN", msg); }
	void Error(const std::string& msg) { Write("ERROR", msg); }
}
