#include "Parser.hpp"
#include <fstream>
#include <stdexcept>

ConfigParser::ConfigParser(const std::string& config_file) {
    std::ifstream file(config_file);
    if (!file.is_open()) {
        throw std::runtime_error("Could NOT open config file: " + config_file);
    }

    file >> m_Config;
    validate();

    if (m_Config.contains("benchmarks") && m_Config["benchmarks"].is_array()) {
        for (const auto& benchmark : m_Config["benchmarks"]) {
            if (benchmark.contains("enabled") && benchmark["enabled"].is_boolean() && benchmark["enabled"]) {
                m_TestNames.push_back(benchmark["name"]);
            }
        }
    }
}

bool ConfigParser::verbose() const {
    return get_value("verbose", false);
}

int ConfigParser::threads() const {
    return get_value("threads", BENCHMARK_THREAD_COUNT);
}

std::string ConfigParser::output_file() const {
    std::string filename = DEFAULT_FILENAME;
    return get_value("output_file", filename);
}

std::string ConfigParser::log_level() const
{
    // LogLevel::INFO is the default log level
    std::string log_level = "INFO";
    return get_value("log_level", log_level);
}

void ConfigParser::validate() const {
    if (!m_Config.contains("threads") || !m_Config["threads"].is_number()) {
        throw std::runtime_error("Invalid or missing 'threads' in config file");
    }
}

std::vector<std::string> ConfigParser::GetTestNames() const
{
    return m_TestNames;
}

template <typename T>
T ConfigParser::get_value(const std::string& key, const T& default_value) const {
    if (m_Config.contains(key)) {
        return m_Config[key].get<T>();
    }
    return default_value;
}

ArgumentParser::ArgumentParser(int argc, char** argv, const std::string& config_file)
{
    ConfigParser config(config_file);
    /* TODO: Add argument parsing support for the test names */
    m_TestNames = config.GetTestNames();  // Load available tests

    m_App.add_flag("-v, --verbose", m_Verbose, "Enable verbose output")
        ->default_val(config.verbose());

    /* TODO: Add a flag for iteration count of the tests */
    //m_App.add_flag("-i, --iterations", m_Iterations, "Number of iterations")
    //    ->check(CLI::PositiveNumber)
    //    ->default_val(config.iterations());

    m_App.add_flag("-t, --threads", m_Threads, "Number of threads")
        ->check(CLI::PositiveNumber)
        ->default_val(config.threads());

    m_App.add_flag("-o, --output", m_OutputFile, "Output file")
        ->default_val(config.output_file());

    m_App.add_flag("-l, --loglevel", m_LogLevel, "Log Level")
        ->default_val(config.log_level());

    try {
        /* Allowed for debugging purposes */
        m_App.allow_extras();
        m_App.parse(argc, argv);
    }
    catch (const CLI::ParseError& e) {
        std::exit(m_App.exit(e));
    }
}

bool ArgumentParser::verbose() const
{
    return m_Verbose;
}

int ArgumentParser::threads() const
{
    return m_Threads;
}

std::string ArgumentParser::output_file() const
{
    return m_OutputFile;
}

std::string ArgumentParser::log_level() const
{
    return m_LogLevel;
}

std::vector<std::string> ArgumentParser::GetTestNames() const
{
    return m_TestNames;
}
