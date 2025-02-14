#pragma once
#include <fstream>
#include <mutex>
#include <string>
#include <memory>
#include <queue>

#define DEFAULT_FILENAME "benchmark.log"
#define DEFAULT_FLUSH_INTERVAL std::chrono::seconds(5) // 5 sec
#define MAX_FILE_SIZE 5 * 1024 * 1024  // 5 MB
#define MAX_BUFFER_SIZE 1024  // 1 KB

/* Declare global logger instance */
class Logger;
extern std::unique_ptr<Logger> g_logger;

enum class LogLevel {
	DEBUG,
	INFO,
	WARNING,
	FATAL
};

class Logger {
private:
	LogLevel m_CurrentLevel;
	size_t m_MaxFileSize;
	size_t m_MaxBufferSize;
	std::string m_Filename;
	std::ofstream m_LogFile;
	std::mutex m_LogMutex;
	std::deque<std::string> m_Buffer;
	std::chrono::milliseconds m_FlushInterval;
	std::thread m_FlushThread;
	std::atomic<bool> m_ShouldExit{ false };

	std::string GetTimestamp();
	
	void OpenLogFile();
	void WriteToFile(const std::string& message);
	void CheckRotation();
	void FlushThreadFunction();

public:
	Logger(
		const std::string& filename = DEFAULT_FILENAME,
		LogLevel level = LogLevel::INFO,
		size_t maxFileSize = MAX_FILE_SIZE,
		size_t maxBufferSize = MAX_BUFFER_SIZE,
		std::chrono::milliseconds flushInterval = DEFAULT_FLUSH_INTERVAL
	);
	~Logger();

	void SetFlushInterval(std::chrono::milliseconds interval);
	void FlushBuffer();
	
	static std::string LogLevelToString(LogLevel level);
	static LogLevel StringToLogLevel(const std::string& levelStr);

	void SetLogLevel(LogLevel level);
	void Log(LogLevel level, const std::string& message);
	
	void Debug(const std::string& message);
	void Info(const std::string& message);
	void Warning(const std::string& message);
	void Error(const std::string& message);
};

#define LOG_DEBUG(message)   g_logger->Debug(message)
#define LOG_INFO(message)    g_logger->Info(message)
#define LOG_WARNING(message) g_logger->Warning(message)
#define LOG_ERROR(message)   g_logger->Error(message)