#pragma once


#include <string>
#include <unordered_map>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/basic_file_sink.h>


class Logger {
public:
    
    Logger();
    
    Logger& defineFileLogger( const std::string& logger_name, const std::string& file_name );
    
    Logger& defineCoutLogger( const std::string& logger_name );
    Logger& defineErrorLogger( const std::string& logger_name );
    Logger& defineDailyLogger( const std::string& logger_name, const std::string& file_name, int hours, int minuets);
    
};
