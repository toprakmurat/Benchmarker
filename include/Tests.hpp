#pragma once
#include <vector>

#include "BenchmarkTest.hpp"
#include "System.hpp"

class MatrixMultiplicationTest : public BenchmarkTest {
public:
	MatrixMultiplicationTest();

	void Run() override;
	void RunMultiThreaded(int numThreads) override;
	void RunSingleIteration() override;

private:
	void _InitializeMatrix(std::vector<std::vector<float>>& m);

	void _BasicMultiplicationSingleThread(const std::vector<std::vector<float>>& a,
								          const std::vector<std::vector<float>>& b,
			                              std::vector<std::vector<float>>& c,
		                                  size_t startRow, size_t endRow);
	void _BasicMultiplicationMultiThread(const std::vector<std::vector<float>>& a,
			                             const std::vector<std::vector<float>>& b,
			                             std::vector<std::vector<float>>& c);

	void _AVX2MultiplicationSingleThread(const std::vector<std::vector<float>>& a,
			                             const std::vector<std::vector<float>>& b,
			                             std::vector<std::vector<float>>& c,
		                                 size_t startRow, size_t endRow);
	void _AVX2MultiplicationMultiThread(const std::vector<std::vector<float>>& a,
			                            const std::vector<std::vector<float>>& b,
			                            std::vector<std::vector<float>>& c);
};

class IntegerArithmeticTest : public BenchmarkTest {
public:
	IntegerArithmeticTest();

	void Run() override;
	void RunSingleIteration() override;

private:
	constexpr static size_t ITERATION_COUNT = 1'000;
	
	constexpr static int64_t MULTIPLIER = 1'103'515'245;
	constexpr static int64_t INCREMENT = 12345;
	constexpr static int64_t MASK = 0x7fffffff;

	int64_t PerformOperations(int64_t result);
};

class FloatingPointTest : public BenchmarkTest {
public:
	FloatingPointTest();
	void Run() override;
	void RunSingleIteration() override;

private:
	benchmark_float_type BasicArithmeticTest();
	benchmark_float_type TranscendentalTest();
	benchmark_float_type SpecialCasesTest();
	benchmark_float_type PrecisionTest();
};

class PrimeTest : public BenchmarkTest {
public:
	PrimeTest();
	void Run() override;
	void RunSingleIteration() override;

private:
	bool isPrime(int n);
};