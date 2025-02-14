#include <cstdlib>
#include <functional>
#include <random>
#include <array>
#include <numeric>

#include "Logger.hpp"
#include "Tests.hpp"
#include "System.hpp"

/* Integer Arithmetic Test Class */
IntegerArithmeticTest::IntegerArithmeticTest() 
	: BenchmarkTest("integer_arithmetic_test") {}

void IntegerArithmeticTest::Run() {
	int64_t result = 0;
	/* Loop unrolling for better pipeline utilization */
	for (size_t i = 0; i < ITERATION_COUNT; i += 4) {
		result += (static_cast<int64_t>(i) * (i + 1)) / 2;
		result += (static_cast<int64_t>(i) * (i + 2)) / 2;
		result += (static_cast<int64_t>(i) * (i + 3)) / 2;
		result += (static_cast<int64_t>(i) * (i + 4)) / 2;
		result = PerformOperations(result);
	}

	volatile int64_t prevent_optimization = result;
	if (!prevent_optimization) {
		LOG_ERROR("Integer arithmetic test result is 0.");
		throw BenchmarkException("Unexpected result in Integer Arithmetic Test");
	}
}

void IntegerArithmeticTest::RunSingleIteration() {
	static std::mt19937 gen(std::random_device{}());
	static std::uniform_int_distribution<int64_t> dist(0, RAND_MAX);

	int64_t result = 0;
	result += (dist(gen) * (dist(gen) + 1)) / 2;
	result += PerformOperations(result);
}

int64_t IntegerArithmeticTest::PerformOperations(int64_t result) {
	result = (result * MULTIPLIER + INCREMENT) ^ MASK;
	result = (result >> 4) | (result << (64 - 4));
	return result;
}


/* Floating Point Test Class */
FloatingPointTest::FloatingPointTest()
	: BenchmarkTest("floating_point_test") {}

void FloatingPointTest::Run() {
	benchmark_float_type result = 0.0;
	result += BasicArithmeticTest();
	result += TranscendentalTest();
	result += SpecialCasesTest();
	result += PrecisionTest();

	volatile benchmark_float_type check = result;
	if (std::isnan(check) || std::isinf(check) || !check) {
		LOG_ERROR("Floating point arithmetic test failed.");
		throw BenchmarkException("Invalid result in Floating Point Test");
	}
}

void FloatingPointTest::RunSingleIteration() {
	static std::mt19937 gen(std::random_device{}());
	static std::uniform_real_distribution<benchmark_float_type> dis(-1000.0, 1000.0);

	benchmark_float_type x = dis(gen);
	benchmark_float_type y = dis(gen);

	volatile benchmark_float_type result = 0.0;
	result += std::sin(x) * std::cos(y) - std::tan(x + y);
	result += std::exp(std::fmod(x, 5.0)) + std::log1p(std::abs(x));
	result += std::sqrt(std::abs(x * y)) * std::pow(x, 0.5);
}

benchmark_float_type FloatingPointTest::BasicArithmeticTest() {
	benchmark_float_type result = 0.0;
	for (int i = 0; i < m_IterationCount; i += 4) {
		benchmark_float_type x1 = i * 0.1, x2 = (i+1) * 0.1, x3 = (i+2) * 0.1, x4 = (i+3) * 0.1;
		result += x1 + x1 - x1 * x1;
		result += x2 + x2 - x2 * x2;
		result += x3 + x3 - x3 * x3;
		result += x4 + x4 - x4 * x4;
	}
	return result;
}

benchmark_float_type FloatingPointTest::TranscendentalTest() {
	benchmark_float_type result = 0.0;
	for (int i = 0; i < m_IterationCount; ++i) {
		benchmark_float_type x = static_cast<benchmark_float_type>(i) * 0.1;
		result += std::sin(x) * std::cos(x) + std::tanh(x);
		result += std::exp2(std::fmod(x, 5.0)) + std::log1p(x);
		result += std::sqrt(std::abs(x)) * std::erf(x);
	}
	return result;
}

benchmark_float_type FloatingPointTest::SpecialCasesTest() {
	std::array<benchmark_float_type, 8> specialValues = {
		0.0,
		std::numeric_limits<benchmark_float_type>::epsilon(),
		std::numeric_limits<benchmark_float_type>::min(),
		std::numeric_limits<benchmark_float_type>::denorm_min(),
		std::numeric_limits<benchmark_float_type>::max(),
		std::numeric_limits<benchmark_float_type>::infinity(),
		-std::numeric_limits<benchmark_float_type>::infinity(),
		std::numeric_limits<benchmark_float_type>::quiet_NaN()
	};

	benchmark_float_type result = 0.0;
	for (auto val : specialValues) {
		result += std::isfinite(val) ? val : 0.0;
		result += std::isnormal(val) ? 1.0 : 0.0;
		result += std::fpclassify(val) * 0.1;
		result += std::copysign(1.0, val);
	}
	return result;
}

benchmark_float_type FloatingPointTest::PrecisionTest() {
	benchmark_float_type result = 0.0;
	benchmark_float_type small = 1.0;
	for (int i = 0; i < 100; ++i) {
		small /= 10.0;
		result += 1.0 + small;
		result -= 1.0;
	}
	volatile benchmark_float_type check = result;
	return check;
}


/* Prime Test Class */
PrimeTest::PrimeTest()
	: BenchmarkTest("prime_calculation_test") {}

void PrimeTest::Run() {
	int count = 0;
	for (int i = 2; i < m_IterationCount; ++i) {
		if (isPrime(i)) ++count;
	}

	volatile int check = count;
}

void PrimeTest::RunSingleIteration() {
	isPrime(rand());
}

bool PrimeTest::isPrime(int n) {
	if (n <= 1) return false;
	if (n <= 3) return true;
	if ((n % 2) == 0 || n % 3 == 0) return false;

	for (int i = 5; i * i < n; i += 6)
		if (n % i == 0 || n % (i + 2) == 0)
			return false;
	
	return true;
}

MatrixMultiplicationTest::MatrixMultiplicationTest() 
	: BenchmarkTest("matrix_multiplication_test") {}

void MatrixMultiplicationTest::Run() {
	RunSingleIteration();
}

void MatrixMultiplicationTest::RunMultiThreaded(int numThreads) {
	const int matrix_size = 512;
	std::vector<std::vector<float>> a(matrix_size, std::vector<float>(matrix_size));
	std::vector<std::vector<float>> b(matrix_size, std::vector<float>(matrix_size));
	std::vector<std::vector<float>> c(matrix_size, std::vector<float>(matrix_size));

	_InitializeMatrix(a);
	_InitializeMatrix(b);

	if constexpr (hasAVX2ctime) {
		if (check_avx2()) {
			LOG_INFO("Using _AVX2MultiplicationMultiThread. (line 185)");
			_AVX2MultiplicationMultiThread(a, b, c);
		}
	}
	else {
		LOG_INFO("Using _BasicMultiplicationMultiThread. (line 190)");
		_BasicMultiplicationMultiThread(a, b, c);
	}
}

void MatrixMultiplicationTest::RunSingleIteration() {
	const int matrix_size = 512;
	std::vector<std::vector<float>> a(matrix_size, std::vector<float>(matrix_size));
	std::vector<std::vector<float>> b(matrix_size, std::vector<float>(matrix_size));
	std::vector<std::vector<float>> c(matrix_size, std::vector<float>(matrix_size));

	_InitializeMatrix(a);
	_InitializeMatrix(b);

	if constexpr (hasAVX2ctime) {
		if (check_avx2()) {
			LOG_INFO("Using _AVX2MultiplicationSingleThread. (line 206)");
			_AVX2MultiplicationSingleThread(a, b, c, 0, matrix_size);
		}
	}
	else {
		LOG_INFO("Using _BasicMultiplicationSingleThread. (line 211)");
		_BasicMultiplicationSingleThread(a, b, c, 0, matrix_size);
	}
}

void MatrixMultiplicationTest::_InitializeMatrix(std::vector<std::vector<float>>& m) {
	static std::mt19937 gen(std::random_device{}());
	static std::uniform_real_distribution<float> dis(0.0f, 1.0f);
	for (auto& row : m) {
		for (auto& val : row) {
			val = dis(gen);
		}
	}
}

void MatrixMultiplicationTest::_BasicMultiplicationSingleThread(
								const std::vector<std::vector<float>>& a,
								const std::vector<std::vector<float>>& b, 
								std::vector<std::vector<float>>& c,
								size_t startRow, size_t endRow)
{
	const size_t size = a.size();
	for (size_t i = startRow; i < endRow; i++) {
		for (size_t j = 0; j < size; j++) {
			float sum = 0.0f;
			for (size_t k = 0; k < size; k++) {
				sum += a[i][k] * b[k][j];
			}
			c[i][j] = sum;
		}
	}
}

void MatrixMultiplicationTest::_BasicMultiplicationMultiThread(
								const std::vector<std::vector<float>>& a,
								const std::vector<std::vector<float>>& b, 
								std::vector<std::vector<float>>& c)
{
	const size_t size = a.size();
	const int numThreads = std::thread::hardware_concurrency();
	std::vector<std::thread> threads;

	const size_t rowsPerThread = size / numThreads;
	for (int t = 0; t < numThreads; t++) {
		size_t startRow = t * rowsPerThread;
		size_t endRow = (t == numThreads - 1) ? size : startRow + rowsPerThread;
		threads.emplace_back([&, startRow, endRow]() {
			_BasicMultiplicationSingleThread(a, b, c, startRow, endRow); 
		});
	}

	for (auto& thread : threads) {
		thread.join();
	}

}

void MatrixMultiplicationTest::_AVX2MultiplicationSingleThread(
								const std::vector<std::vector<float>>& a,
								const std::vector<std::vector<float>>& b,
								std::vector<std::vector<float>>& c,
								size_t startRow, size_t endRow)
{
	const size_t size = a.size();
	for (size_t i = startRow; i < endRow; i++) {
		for (size_t j = 0; j < size; j += 8) { // Process 8 elements using AVX2
			__m256 sum = _mm256_setzero_ps();
			for (size_t k = 0; k < size; k++) {
				__m256 a_vec = _mm256_set1_ps(a[i][k]);
				__m256 b_vec = _mm256_loadu_ps(&b[k][j]);
				sum = _mm256_add_ps(sum, _mm256_mul_ps(a_vec, b_vec));
			}
			_mm256_storeu_ps(&c[i][j], sum);
		}
	}
}

void MatrixMultiplicationTest::_AVX2MultiplicationMultiThread(
								const std::vector<std::vector<float>>& a, 
								const std::vector<std::vector<float>>& b, 
								std::vector<std::vector<float>>& c)
{
	const size_t size = a.size();
	const int numThreads = std::thread::hardware_concurrency();
	std::vector<std::thread> threads;

	size_t rowsPerThread = size / numThreads;
	for (int t = 0; t < numThreads; t++) {
		size_t startRow = t * rowsPerThread;
		size_t endRow = (t == numThreads - 1) ? size : startRow + rowsPerThread;
		threads.emplace_back([&, startRow, endRow]() {
			_AVX2MultiplicationSingleThread(a, b, c, startRow, endRow);
		});
	}

	for (auto& thread : threads) {
		thread.join();
	}
}
