#include "Benchmark.hpp"
#include "BenchmarkTest.hpp"
#include "Logger.hpp"
#include "Parser.hpp"
#include "System.hpp"
#include "Tests.hpp"

/* JSON Parsing */
#include "nlohmann/json.hpp"
using json = nlohmann::json;

// Global logger
std::unique_ptr<Logger> g_logger;


int main(int argc, char* argv[]) {
	const std::string config_path = "config/config.json";

	try {
		// Parse command line arguments and overwrite config file
		ArgumentParser arg_parser(argc, argv, config_path);
		
		// Define global logger instance
		LogLevel current_level = Logger::StringToLogLevel(arg_parser.log_level());
		g_logger = std::make_unique<Logger>(
			arg_parser.output_file(),
			current_level,
			MAX_FILE_SIZE,
			MAX_BUFFER_SIZE,
			std::chrono::seconds(5)
		);

		LOG_INFO("CPU Benchmark tool started");

		CPUBenchmark benchmark(arg_parser.threads());
		
		std::vector<std::string> avail_testnames = arg_parser.GetTestNames();
		for (const std::string& testname : avail_testnames) {
			std::unique_ptr<BenchmarkTest> test = benchmark.FindTest(testname);
			if (test != nullptr)
				benchmark.AddTest(std::move(test));
		}

		benchmark.RunAllTests();

		LOG_INFO("CPU Benchmark tool finished successfully");
	}
	catch (const std::exception& e) {
		std::cerr << "Fatal error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}