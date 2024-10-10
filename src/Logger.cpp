#include "Logger.h"
#include <iostream>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <ctime>

std::string Logger::GetTimestamp() {
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);
	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
	return ss.str();
}

// TODO: Include Topname library to achieve the same functionality
std::string Logger::LogLevelToString(LogLevel level) {
	switch (level) {
	case LogLevel::DEBUG:	return "DEBUG";
	case LogLevel::INFO:	return "INFO";
	case LogLevel::WARNING:	return "WARNING";
	case LogLevel::ERROR:	return "ERROR";
	default:				return "UNKNOWN";
	}
}

Logger::Logger(const std::string& filename, LogLevel level = LogLevel::INFO)
	: m_LogFile(filename, std::ios::app), m_CurrentLevel(level) {
	if (!m_LogFile.is_open()) {
		throw std::runtime_error("Unable to open log file: " + filename);
	}
}

void Logger::SetLogLevel(LogLevel level) {
	m_CurrentLevel = level;
}

void Logger::Log(LogLevel level, const std::string& message) {
	if (level >= m_CurrentLevel) {
		std::lock_guard<std::mutex> lock(m_LogMutex);
		std::string logMessage = GetTimestamp() + " [" + LogLevelToString(level) + "] " + message;
		m_LogFile << logMessage << std::endl;
		std::cout << logMessage << std::endl;
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
	Log(LogLevel::ERROR, message);
}
