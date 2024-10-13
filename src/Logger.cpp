#include <iostream>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <vector>
#include <filesystem>
#include <thread>

#include "Logger.hpp"

Logger::Logger(
	const std::string& filename = DEFAULT_FILENAME,
	LogLevel level = LogLevel::INFO, 
	size_t maxFileSize = MAX_FILE_SIZE, 
	size_t maxBufferSize = MAX_BUFFER_SIZE,
	std::chrono::milliseconds flushInterval = std::chrono::seconds(5))
	: m_Filename(filename), 
	m_CurrentLevel(level), 
	m_MaxFileSize(maxFileSize), 
	m_MaxBufferSize(maxBufferSize), 
	m_FlushInterval(flushInterval)
{
	OpenLogFile();
	m_FlushThread = std::thread(&Logger::FlushThreadFunction, this);
}

Logger::~Logger() {
	m_ShouldExit = true;
	if (m_FlushThread.joinable()) {
		m_FlushThread.join();
	}
	FlushBuffer();
}

std::string Logger::GetTimestamp() {
	auto now = std::chrono::system_clock::now();
	std::time_t in_time_t = std::chrono::system_clock::to_time_t(now);

	std::tm tm{};

#ifdef _WIN32
	localtime_s(&tm, &in_time_t);
#else
	localtime_r(&in_time_t, &tm);
#endif

	std::stringstream ss;
	ss << std::put_time(&tm, "%Y-%m-%d %X");
	return ss.str();
}

// TODO: Include Topname library to achieve the same functionality
std::string Logger::LogLevelToString(LogLevel level) {
	switch (level) {
	case LogLevel::DEBUG:	return "DEBUG";
	case LogLevel::INFO:	return "INFO";
	case LogLevel::WARNING:	return "WARNING";
	case LogLevel::FATAL:	return "FATAL";
	default:				return "UNKNOWN";
	}
}

void Logger::OpenLogFile() {
	m_LogFile.open(m_Filename, std::ios::app);
	if (!m_LogFile.is_open()) {
		throw std::runtime_error("Unable to open log file: " + m_Filename);
	}
}

void Logger::WriteToFile(const std::string& message) {
	m_LogFile << message << std::endl;
	CheckRotation();
}

void Logger::CheckRotation() {
	if (std::filesystem::file_size(m_Filename) >= m_MaxFileSize) {
		m_LogFile.close();
		std::string newFilename = m_Filename + "." + GetTimestamp();
		std::filesystem::rename(m_Filename, newFilename);
		OpenLogFile();
	}
}

void Logger::FlushThreadFunction() {
	while (!m_ShouldExit) {
		std::this_thread::sleep_for(m_FlushInterval);
		FlushBuffer();
	}
}

void Logger::SetFlushInterval(std::chrono::milliseconds interval) {
	std::lock_guard<std::mutex> lock(m_LogMutex);
	m_FlushInterval = interval;
}

void Logger::FlushBuffer() {
	std::lock_guard<std::mutex> lock(m_LogMutex);
	for (const auto& message : m_Buffer) {
		WriteToFile(message);
	}
	m_Buffer.clear();
	m_LogFile.flush();  // instantly write to the disk
}


void Logger::SetLogLevel(LogLevel level) {
	m_CurrentLevel = level;
}

void Logger::Log(LogLevel level, const std::string& message) {
	if (level >= m_CurrentLevel) {
		std::lock_guard<std::mutex> lock(m_LogMutex);
		std::string logMessage = GetTimestamp() + " [" + LogLevelToString(level) + "] " + message;
		
		m_Buffer.push_back(logMessage);

		// If buffer is full, flush oldest entry to file
		while (m_Buffer.size() > m_MaxBufferSize) {
			WriteToFile(m_Buffer.front());
			m_Buffer.pop_front();
		}

		std::cout << logMessage << std::endl;  // still print to console immediately
	}
}

void Logger::Debug(const std::string& message) {
	Log(LogLevel::DEBUG, message);
}

void Logger::Info(const std::string& message) {
	Log(LogLevel::INFO, message);
}

void Logger::Warning(const std::string& message) {
	Log(LogLevel::WARNING, message);
}

void Logger::Error(const std::string& message) {
	Log(LogLevel::FATAL, message);
}
