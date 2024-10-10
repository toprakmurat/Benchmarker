#pragma once
#include <string>
#include <stdexcept>

#ifndef BENCHMARK_ITERATION_COUNT
#define BENCHMARK_ITERATION_COUNT 10'000'000
#endif

#ifndef BENCHMARK_THREAD_COUNT
#define BENCHMARK_THREAD_COUNT 4
#endif

using benchmark_float_type = double;

class BenchmarkException : public std::runtime_error {
public:
	BenchmarkException(const std::string& msg) : std::runtime_error(msg) {}
};

class BenchmarkTest {
protected:
	std::string m_Name;
	benchmark_float_type m_Score = 0.0;

public:
	BenchmarkTest(const std::string& testName);
	virtual ~BenchmarkTest() = default;

	virtual void Run() = 0;
	virtual void RunMultiThreaded(int numThreads);
	virtual void RunSingleIteration() = 0;
	
	std::string GetName() const;
	void SetScore(benchmark_float_type score);
	benchmark_float_type GetScore() const;
};