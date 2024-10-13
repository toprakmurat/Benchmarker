#pragma once
#include "BenchmarkTest.hpp"

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