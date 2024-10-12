#include <cstdlib>
#include <functional>

#include "Tests.hpp"

IntegerArithmeticTest::IntegerArithmeticTest() 
	: BenchmarkTest("Integer Arithmetic Test") {}

void IntegerArithmeticTest::Run() {
	volatile int result = 0;
	for (int i = 0; i < BENCHMARK_ITERATION_COUNT; ++i) {
		result += i * (i + 1) / 2;
	}
	if (result == 0) {
		throw BenchmarkException("Unexpected result in Integer Arithmetic Test");
	}
}

void IntegerArithmeticTest::RunSingleIteration() {
	volatile int result = 0;
	result += rand() * (rand() + 1) / 2;
}


FloatingPointTest::FloatingPointTest()
	: BenchmarkTest("Floating Point Test") {}

void FloatingPointTest::Run() {
	volatile benchmark_float_type result = 0.0;
	for (int i = 0; i < BENCHMARK_ITERATION_COUNT; ++i) {
		result += std::sqrt(static_cast<benchmark_float_type>(i) * 3.14159);
	}
	if (std::isnan(result)) {
		throw BenchmarkException("NaN result in Floating-Point Arithmetic Test");
	}
}

void FloatingPointTest::RunSingleIteration() {
	volatile benchmark_float_type result = 0.0;
	result += std::sqrt(static_cast<benchmark_float_type>(rand()) * 3.14159);
}

PrimeTest::PrimeTest()
	: BenchmarkTest("Prime Test") {}

void PrimeTest::Run() {
	int count = 0;
	for (int i = 2; i < BENCHMARK_ITERATION_COUNT; ++i) {
		if (isPrime(i)) ++count;
	}
}

void PrimeTest::RunSingleIteration() {
	isPrime(rand());
}

bool PrimeTest::isPrime(int n) {
	if (n <= 1) return false;
	if (n == 2 || n == 3) return true;
	if ((n & 1) == 0 || n % 3 == 0) return false;

	for (int i = 5; i * i < n; i += 6)
		if (n % i == 0 || n % (i + 2) == 0)
			return false;
	
	return true;
}