#pragma once
#include <fstream>
#include <mutex>
#include <string>
#include <memory>

/* Declare global logger instance */
class Logger;
extern std::unique_ptr<Logger> g_logger;

enum class LogLevel {
	DEBUG,
	INFO,
	WARNING,
	ERROR
};

class Logger {
private:
	std::ofstream m_LogFile;
	std::mutex m_LogMutex;
	LogLevel m_CurrentLevel;

	std::string GetTimestamp();
	std::string LogLevelToString(LogLevel level);

public:
	Logger(const std::string& filename, LogLevel level);

	void SetLogLevel(LogLevel level);
	void Log(LogLevel level, const std::string& message);
	
	void Debug(const std::string& message);
	void Info(const std::string& message);
	void Warning(const std::string& message);
	void Error(const std::string& message);
};