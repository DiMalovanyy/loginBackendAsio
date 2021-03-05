#include "logger.h"
//#include "constants.h"

Logger::Logger() {}

Logger& Logger::defineFileLogger( const std::string& logger_name, const std::string& file_name ) {
    spdlog::basic_logger_mt(logger_name, file_name);
    return *this;
}

Logger& Logger::defineCoutLogger( const std::string& logger_name ) {
    spdlog::stdout_color_mt(logger_name);
    return *this;
}

Logger& Logger::defineErrorLogger( const std::string& logger_name) {
    spdlog::stderr_color_mt(logger_name);
    return *this;
}

Logger& Logger::defineDailyLogger( const std::string& logger_name, const std::string& file_name, int hours, int minuets) {
    spdlog::daily_logger_mt(logger_name, file_name, hours, minuets);
    return *this;
}

