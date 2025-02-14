#include <iostream>
#include <vector>
#include <iomanip>

#include "Benchmark.hpp"
#include "Tests.hpp"
#include "Logger.hpp"

CPUBenchmark::CPUBenchmark(int numThreads) : m_ThreadCount(numThreads) {
	m_UseMultiThreading = m_ThreadCount > 1;

	m_SysInfo = SystemDetector::GetSysInfo();
	m_ReportFile.open("benchmark_report.csv");

	m_ReportFile << "Operating System, CPU Model, Num of Cores, Total Phys RAM (GB)" << std::endl;
	m_ReportFile << m_SysInfo.operatingSystem << ","
		<< m_SysInfo.cpuModel << ","
		<< m_SysInfo.numCores << ","
		<< std::fixed << std::setprecision(2) << (m_SysInfo.totalRAM / (1024.0 * 1024.0 * 1024.0)) << std::endl;
		
	m_ReportFile << std::endl;
		
	m_ReportFile << "Test Name,Score,Duration (ms)" << std::endl;

	LOG_INFO("CPUBenchmark initialized with " + std::to_string(m_ThreadCount) + " threads");
	logSystemInfo();

	createTestsMap();
}

CPUBenchmark::~CPUBenchmark() {
		m_ReportFile.close();
		LOG_INFO("CPUBenchmark destroyed, report file closed");
}

void CPUBenchmark::logSystemInfo() {
	LOG_INFO("System Information: ");
	LOG_INFO("Operating System: " + m_SysInfo.operatingSystem);
	LOG_INFO("CPU Model: " + m_SysInfo.cpuModel);
	LOG_INFO("Number of CPU Cores: " + std::to_string(m_SysInfo.numCores));
	LOG_INFO("Total Physical RAM: " + std::to_string(m_SysInfo.totalRAM / (1024.0 * 1024.0 * 1024.0)) + " GB");
}

void CPUBenchmark::createTestsMap()
{
	m_TestsMap.emplace("matrix_multiplication_test", std::make_unique<MatrixMultiplicationTest>());
	m_TestsMap.emplace("integer_arithmetic_test", std::make_unique<IntegerArithmeticTest>());
	m_TestsMap.emplace("floating_point_test", std::make_unique<FloatingPointTest>());
	m_TestsMap.emplace("prime_calculation_test", std::make_unique<PrimeTest>());
}


void CPUBenchmark::AddTest(std::unique_ptr<BenchmarkTest> test) {
	m_Tests.push_back(std::move(test));
	LOG_INFO("Added test: " + m_Tests.back()->GetName());
}

std::unique_ptr<BenchmarkTest> CPUBenchmark::FindTest(const std::string& testname)
{
	if (m_TestsMap.find(testname) != m_TestsMap.end()) {
		return std::move(m_TestsMap[testname]);
	}
	return nullptr;
}

void CPUBenchmark::RunAllTests() {
	LOG_INFO("Starting all benchmark tests");
	for (const auto& test : m_Tests) {
		try {
			LOG_INFO("Running test: " + test->GetName());

			std::chrono::milliseconds duration;
			if (m_UseMultiThreading)
				duration = measureExecTime([&]() { test->RunMultiThreaded(m_ThreadCount); });
			else
				duration = measureExecTime([&]() { test->Run(); });

			benchmark_float_type score = BENCHMARK_ITERATION_COUNT / static_cast<benchmark_float_type>(duration.count());
			test->SetScore(score);

			LOG_INFO(test->GetName() + " completed in " + std::to_string(duration.count()) + " ms");
			LOG_INFO(test->GetName() + "'s score: " + std::to_string(score) + " iterations/ms");
		
			m_ReportFile << test->GetName() << "," << score << "," << duration.count() << std::endl;
		}
		catch (const BenchmarkException& e) {
			std::cerr << "Error in test " << test->GetName() << ": " << e.what() << std::endl;
		}
		catch (const std::exception& e) {
			std::cerr << "Unexpected error in test " << test->GetName() << ": " << e.what() << std::endl;
		}
	}
	LOG_INFO("All benchmark tests completed");
}