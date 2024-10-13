#include <cstdlib>
#include <functional>
#include <random>
#include <array>

#include "Logger.hpp"
#include "Tests.hpp"

/* TODO: Declare the variable based on compilation options(volatile or not) */

/* Integer Arithmetic Test Class */
IntegerArithmeticTest::IntegerArithmeticTest() 
	: BenchmarkTest("Integer Arithmetic Test") {}

void IntegerArithmeticTest::Run() {
	int64_t result = 0;
	for (size_t i = 0; i < ITERATION_COUNT; ++i) {
		result += (static_cast<int64_t>(i) * (i + 1)) / 2;
		result = PerformOperations(result);
	}
	
	if (result == 0) {
		LOG_ERROR("Integer arithmetic test result is 0");
		throw BenchmarkException("Unexpected result in Integer Arithmetic Test");
	}
}

void IntegerArithmeticTest::RunSingleIteration() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int64_t> dist(0, RAND_MAX);

	int64_t result = 0;
	result += (dist(gen) * (dist(gen) + 1)) / 2;
	result += PerformOperations(result);
}

int64_t IntegerArithmeticTest::PerformOperations(int64_t result) {
	result *= MULTIPLIER;
	result += INCREMENT;
	result &= MASK;
	return result;
}


/* Floating Point Test Class */
FloatingPointTest::FloatingPointTest()
	: BenchmarkTest("Floating Point Test") {}

void FloatingPointTest::Run() {
	benchmark_float_type result = 0.0;
	result += BasicArithmeticTest();
	result += TranscendentalTest();
	result += SpecialCasesTest();
	result += PrecisionTest();

	if (std::isnan(result) || std::isinf(result)) {
		throw BenchmarkException("Invalid result in Floating Point Test");
	}
}

void FloatingPointTest::RunSingleIteration() {
	static std::mt19937 gen(std::random_device{}());
	static std::uniform_real_distribution<benchmark_float_type> dis(-1000.0, 1000.0);

	benchmark_float_type x = dis(gen);
	benchmark_float_type y = dis(gen);

	volatile benchmark_float_type result = 0.0;
	result += std::sin(x) * std::cos(y);
	result += std::exp(std::fmod(x, 5.0));
	result += std::log(std::abs(x) + 1.0);
	result += std::sqrt(std::abs(x * y));
}

benchmark_float_type FloatingPointTest::BasicArithmeticTest() {
	benchmark_float_type result = 0.0;
	for (int i = 0; i < BENCHMARK_ITERATION_COUNT; ++i) {
		benchmark_float_type x = static_cast<benchmark_float_type>(i) * 0.1;
		result += x + x;
		result -= x * x;
		result *= 1.01;
		result /= 1.01;
	}
	return result;
}

benchmark_float_type FloatingPointTest::TranscendentalTest() {
	benchmark_float_type result = 0.0;
	for (int i = 0; i < BENCHMARK_ITERATION_COUNT; ++i) {
		benchmark_float_type x = static_cast<benchmark_float_type>(i) * 0.1;
		result += std::sin(x) * std::cos(x);
		result += std::exp(std::fmod(x, 5.0));
		result += std::log(x + 1.0);
		result += std::sqrt(std::abs(x));
	}
	return result;
}

benchmark_float_type FloatingPointTest::SpecialCasesTest() {
	std::array<benchmark_float_type, 6> specialValues = {
		0.0,
		std::numeric_limits<benchmark_float_type>::epsilon(),
		std::numeric_limits<benchmark_float_type>::min(),
		std::numeric_limits<benchmark_float_type>::max(),
		std::numeric_limits<benchmark_float_type>::infinity(),
		std::numeric_limits<benchmark_float_type>::quiet_NaN()
	};

	benchmark_float_type result = 0.0;
	for (auto val : specialValues) {
		result += std::isfinite(val) ? val : 0.0;
		result += std::isnormal(val) ? 1.0 : 0.0;
		result += std::fpclassify(val);
	}
	return result;
}

benchmark_float_type FloatingPointTest::PrecisionTest() {
	benchmark_float_type result = 0.0;
	benchmark_float_type small = 1.0;
	for (int i = 0; i < 100; ++i) {
		small /= 10.0;
		result += 1.0 + small;
	}
	return result - 100.0; // close to zero
}


/* Prime Test Class */
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