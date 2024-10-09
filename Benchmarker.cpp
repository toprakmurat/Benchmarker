#include <iostream>
#include <vector>
#include <chrono>
#include <functional>
#include <string>
#include <memory>
#include <stdexcept>
#include <cmath>
#include <utility>
#include <thread>
#include <atomic>
#include <random>
#include <fstream>

#ifndef BENCHMARK_ITERATION_COUNT
#define BENCHMARK_ITERATION_COUNT 10'000'000
#endif

#ifndef BENCHMARK_THREAD_COUNT
#define BENCHMARK_THREAD_COUNT 4
#endif

// Helper function to measure execution time of any function 
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

class BenchmarkException : public std::runtime_error {
public:
	BenchmarkException(const std::string& msg) : std::runtime_error(msg) {}
};

class BenchmarkTest {
public:
	using benchmark_float_type = double;

protected:
	std::string name;
	benchmark_float_type score;

public:
	BenchmarkTest(const std::string& testName) : name(testName), score(0) {}
	virtual ~BenchmarkTest() = default;
	virtual void run() = 0;
	virtual void runMultiThreaded(int numThreads) {
		std::vector<std::thread> threads;
		std::atomic<int> counter(0);
		for (int i = 0; i < numThreads; ++i) {
			threads.emplace_back([this, &counter, numThreads]() {
				while (counter.fetch_add(1) < BENCHMARK_ITERATION_COUNT) {
					this->runSingleIteration();
				}
			});
		}
		for (auto& thread : threads) {
			thread.join();
		}
	}
	virtual void runSingleIteration() = 0;
	std::string getName() const { return name; }
	void setScore(benchmark_float_type s) { score = s; }
	benchmark_float_type getScore() const { return score; }
};

class CPUBenchmark {
public:
	using benchmark_float_type = double;

private:
	std::vector<std::unique_ptr<BenchmarkTest>> tests;
	bool useMultiThreading;
	int threadCount;
	std::ofstream reportFile;

public:
	CPUBenchmark(bool multiThreaded = true, int threads = BENCHMARK_THREAD_COUNT)
		: useMultiThreading(multiThreaded), threadCount(threads) {
		reportFile.open("benchmark_report.csv");
		reportFile << "Test Name,Score,Duration (ms)" << std::endl;
	}

	~CPUBenchmark() {
		reportFile.close();
	}

	void addTest(std::unique_ptr<BenchmarkTest> test) {
		tests.push_back(std::move(test));
	}

	void runAllTests() {
		for (const auto& test : tests) {
			try {
				std::cout << "Running test: " << test->getName() << std::endl;

				std::chrono::milliseconds duration;
				if (useMultiThreading)
					duration = measureExecTime([&]() { test->runMultiThreaded(threadCount); });
				else
					duration = measureExecTime([&]() { test->run(); });

				benchmark_float_type score = BENCHMARK_ITERATION_COUNT / static_cast<benchmark_float_type>(duration.count());
				test->setScore(score);
				std::cout << "Test completed in " << duration.count() << " ms" << std::endl;
				std::cout << "Score: " << score << " iterations/ms" << std::endl;
				reportFile << test->getName() << "," << score << "," << duration.count() << std::endl;
			}
			catch (const BenchmarkException& e) {
				std::cerr << "Error in test " << test->getName() << ": " << e.what() << std::endl;
			}
			catch (const std::exception& e) {
				std::cerr << "Unexpected error in test " << test->getName() << ": " << e.what() << std::endl;
			}
		}
	}
};

class IntegerArithmeticTest : public BenchmarkTest {
public:
	IntegerArithmeticTest() : BenchmarkTest("Integer Arithmetic Test") {}
	void run() override {
		volatile int result = 0;
		for (int i = 0; i < BENCHMARK_ITERATION_COUNT; ++i) {
			result += i * (i + 1) / 2;
		}
		if (result == 0) {
			throw BenchmarkException("Unexpected result in Integer Arithmetic Test");
		}
	}

	void runSingleIteration() override {
		volatile int result = 0;
		result += rand() * (rand() + 1) / 2;
	}
};

class FloatingPointTest : public BenchmarkTest {
public:
	FloatingPointTest() : BenchmarkTest("Floating Point Arithmetic Test") {}
	void run() override {
		volatile benchmark_float_type result = 0.0;
		for (int i = 0; i < BENCHMARK_ITERATION_COUNT; ++i) {
			result += std::sqrt(static_cast<benchmark_float_type>(i) * 3.14159);
		}
		if (std::isnan(result)) {
			throw BenchmarkException("NaN result in Floating-Point Arithmetic Test");
		}
	}
	void runSingleIteration() override {
		volatile benchmark_float_type result = 0.0;
		result += std::sqrt(static_cast<benchmark_float_type>(rand()) * 3.14159);
	}
};


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

		benchmark.addTest(std::make_unique<IntegerArithmeticTest>());
		benchmark.addTest(std::make_unique<FloatingPointTest>());

		benchmark.runAllTests();

	}
	catch (const std::exception& e) {
		std::cerr << "Fatal error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}