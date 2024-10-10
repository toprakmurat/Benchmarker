#include "BenchmarkTest.h"
#include <vector>
#include <thread>
#include <atomic>

BenchmarkTest::BenchmarkTest(const std::string& testName) {
	m_Name = testName;
	m_Score = 0.0;
}

void BenchmarkTest::RunMultiThreaded(int numThreads) {
	std::vector<std::thread> threads;
	std::atomic<int> counter(0);

	for (int i = 0; i < numThreads; ++i) {
		threads.emplace_back([this, &counter, numThreads]() {
			while (counter.fetch_add(1) < BENCHMARK_ITERATION_COUNT) {
				this->RunSingleIteration();
			}
		});
	}
	for (auto& thread : threads) {
		thread.join();
	}
}

std::string BenchmarkTest::GetName() const {
	return m_Name;
}

void BenchmarkTest::SetScore(benchmark_float_type score) {
	m_Score = score;
}

benchmark_float_type BenchmarkTest::GetScore() const {
	return m_Score;
}
