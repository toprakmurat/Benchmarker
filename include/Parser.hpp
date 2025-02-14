#pragma once

#include <string>

/* JSON parsing */
#include <nlohmann/json.hpp>
using json = nlohmann::json;

/* Command line parsing */
#include <CLI11/CLI11.hpp>

#include "BenchmarkTest.hpp"
#include "Logger.hpp"

class ConfigParser {
public:
    explicit ConfigParser(const std::string& config_file);

    bool verbose() const;
    int threads() const;
    std::string output_file() const;
    std::string log_level() const;
    void validate() const;

    std::vector<std::string> GetTestNames() const;

private:
    json m_Config;
    std::vector<std::string> m_TestNames;

    template <typename T>
    T get_value(const std::string& key, const T& default_value = T{}) const;
};

class ArgumentParser {
public:
    ArgumentParser(int argc, char** argv, const std::string& config_file);

    bool verbose() const;
    int threads() const;
    std::string output_file() const;
    std::string log_level() const;
    std::vector<std::string> GetTestNames() const;

private:
    CLI::App m_App;
    bool m_Verbose = false;
    int m_Threads = BENCHMARK_THREAD_COUNT;
    std::string m_OutputFile = DEFAULT_FILENAME;
    std::string m_LogLevel = "INFO";
    std::vector<std::string> m_TestNames;
};