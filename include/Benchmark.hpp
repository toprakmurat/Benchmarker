#pragma once
#include <iostream>
#include <chrono>
#include <vector>
#include <fstream>
#include <memory>
#include <unordered_map>

#include "BenchmarkTest.hpp"
#include "System.hpp"

// Helper function template to measure execution time of any function 
template<typename Func, typename... Args>
std::chrono::milliseconds measureExecTime(Func&& func, Args&&... args) {
	auto start = std::chrono::high_resolution_clock::now();

	if constexpr (std::is_void_v<std::invoke_result_t<Func, Args...>>)
		std::forward<Func>(func)(std::forward<Args>(args)...);
	else
		[[maybe_unused]] auto result = std::forward<Func>(func)(std::forward<Args>(args)...);  // No use case of result for now

	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
}

class CPUBenchmark {
private:
	std::unordered_map<std::string, std::unique_ptr<BenchmarkTest>> m_TestsMap;
	std::vector<std::unique_ptr<BenchmarkTest>> m_Tests;
	bool m_UseMultiThreading;
	int m_ThreadCount;
	int m_IterationCount;
	std::ofstream m_ReportFile;
	SystemInfo m_SysInfo;

	void logSystemInfo();
	void createTestsMap();

public:
	CPUBenchmark(int threads = BENCHMARK_THREAD_COUNT);
	~CPUBenchmark();

	void AddTest(std::unique_ptr<BenchmarkTest> test);
	std::unique_ptr<BenchmarkTest> FindTest(const std::string& testname);
	void RunAllTests();
};
