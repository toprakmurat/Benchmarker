#pragma once
#include "BenchmarkTest.h"

class IntegerArithmeticTest : public BenchmarkTest {
public:
	IntegerArithmeticTest();

	void Run() override;
	void RunSingleIteration() override;
};

class FloatingPointTest : public BenchmarkTest {
public:
	FloatingPointTest();
	void Run() override;
	void RunSingleIteration() override;
};

class PrimeTest : public BenchmarkTest {
public:
	PrimeTest();
	void Run() override;
	void RunSingleIteration() override;

private:
	bool isPrime(int n);
};