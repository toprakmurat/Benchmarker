#include "Benchmark.hpp"
#include "BenchmarkTest.hpp"
#include "Logger.hpp"
#include "System.hpp"
#include "Tests.hpp"

// Global logger
std::unique_ptr<Logger> g_logger;

int main(int argc, char* argv[]) {
	try {
		bool useMultiThreading = true;
		int threadCount = BENCHMARK_THREAD_COUNT;
		LogLevel logLevel = LogLevel::INFO;

		/* TODO: Argument Parsing */
		for (int i = 1; i < argc; ++i) {
			std::string arg = argv[i];
			if (arg == "--single-threaded") {
				useMultiThreading = false;
			} 
			else if (arg == "--threads" && i + 1 < argc) {
				threadCount = std::stoi(argv[++i]);
			}
			else if (arg == "--log-level" && i + 1 < argc) {
				std::string level = argv[++i];
				if (level == "debug") logLevel = LogLevel::DEBUG;
				else if (level == "info") logLevel = LogLevel::INFO;
				else if (level == "warning") logLevel = LogLevel::WARNING;
				else if (level == "error") logLevel = LogLevel::ERROR;
			}
		}
		
		// Define global logger instance
		g_logger = std::make_unique<Logger>("benchmark.log", logLevel);
		g_logger->Info("CPU Benchmark tool started");

		CPUBenchmark benchmark(useMultiThreading, threadCount);
		
		benchmark.AddTest(std::make_unique<IntegerArithmeticTest>());
		benchmark.AddTest(std::make_unique<FloatingPointTest>());
		benchmark.AddTest(std::make_unique<PrimeTest>());

		benchmark.RunAllTests();

		g_logger->Info("CPU Benchmark tool finished successfully");
	}
	catch (const std::exception& e) {
		std::cerr << "Fatal error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}