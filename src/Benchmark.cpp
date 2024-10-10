#include <iostream>
#include <vector>

#include "Benchmark.h"
#include "Tests.h"
#include "System.h"
#include "Logger.h"

CPUBenchmark::CPUBenchmark(bool multiThreaded, int threads)
	: m_UseMultiThreading(multiThreaded), m_ThreadCount(threads) {
		m_ReportFile.open("benchmark_report.csv");
		m_ReportFile << "Test Name,Score,Duration (ms)" << std::endl;
	}

CPUBenchmark::~CPUBenchmark() {
		m_ReportFile.close();
}

void CPUBenchmark::AddTest(std::unique_ptr<BenchmarkTest> test) {
	m_Tests.push_back(std::move(test));
}

void CPUBenchmark::RunAllTests() {
	for (const auto& test : m_Tests) {
		try {
			std::cout << "Running test: " << test->GetName() << std::endl;

			std::chrono::milliseconds duration;
			if (m_UseMultiThreading)
				duration = measureExecTime([&]() { test->RunMultiThreaded(m_ThreadCount); });
			else
				duration = measureExecTime([&]() { test->Run(); });

			benchmark_float_type score = BENCHMARK_ITERATION_COUNT / static_cast<benchmark_float_type>(duration.count());
			test->SetScore(score);
			std::cout << "Test completed in " << duration.count() << " ms" << std::endl;
			std::cout << "Score: " << score << " iterations/ms" << std::endl;
			m_ReportFile << test->GetName() << "," << score << "," << duration.count() << std::endl;
		}
		catch (const BenchmarkException& e) {
			std::cerr << "Error in test " << test->GetName() << ": " << e.what() << std::endl;
		}
		catch (const std::exception& e) {
			std::cerr << "Unexpected error in test " << test->GetName() << ": " << e.what() << std::endl;
		}
	}
}


int main(int argc, char* argv[]) {
	try {
		bool useMultiThreading = true;
		int threadCount = BENCHMARK_THREAD_COUNT;

		// Simple command-line argument parsing
		for (int i = 1; i < argc; ++i) {
			std::string arg = argv[i];
			if (arg == "--single-thread") {
				useMultiThreading = false;
			}
			else if (arg == "--threads" && i + 1 < argc) {
				threadCount = std::stoi(argv[++i]);
			}
		}

		CPUBenchmark benchmark(useMultiThreading, threadCount);

		benchmark.AddTest(std::make_unique<IntegerArithmeticTest>());
		benchmark.AddTest(std::make_unique<FloatingPointTest>());

		benchmark.RunAllTests();

	}
	catch (const std::exception& e) {
		std::cerr << "Fatal error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}