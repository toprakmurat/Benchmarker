#include <vector>
#include <thread>
#include <atomic>

#include "BenchmarkTest.hpp"
#include "Logger.hpp"

BenchmarkTest::BenchmarkTest(const std::string& testName) {
	m_Name = testName;
	m_Score = 0.0;
}

void BenchmarkTest::RunMultiThreaded(int numThreads) {
	g_logger->Info("Starting multi-threaded run of " + m_Name + " with " + std::to_string(numThreads) + " threads");

	std::vector<std::thread> threads;
	std::atomic<int> counter(0);

	for (int i = 0; i < numThreads; ++i) {
		threads.emplace_back([this, &counter, numThreads, i]() {
			g_logger->Debug("Thread " + std::to_string(i) + " started for " + m_Name);
			while (counter.fetch_add(1) < BENCHMARK_ITERATION_COUNT) {
				this->RunSingleIteration();
			}
			g_logger->Debug("Thread " + std::to_string(i) + " finished for " + m_Name);
		});
	}
	for (auto& thread : threads) {
		thread.join();
	}
	
	g_logger->Info("Completed multi-threaded run of " + m_Name);
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
