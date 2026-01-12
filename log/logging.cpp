#include "logging.h"
#include <iostream>

// Definición del miembro estático
LogLevel Logger::level = LogLevel::INFO;

// Sobrecarga string
void Logger::log(LogLevel msgLevel,
                 const std::string& msg,
                 bool flush_bool,
                 bool flag)
{
    if (msgLevel < level) return;

    if (flag) {
        switch (msgLevel) {
            case LogLevel::DEBUG: std::cout << "[DEBUG] "; break;
            case LogLevel::INFO:  std::cout << "[INFO] ";  break;
            case LogLevel::WARN:  std::cout << "[WARN] ";  break;
            case LogLevel::ERROR: std::cout << "[ERROR] "; break;
            case LogLevel::OUTPUT: break;
        }
    }

    std::cout << msg;
    if (flush_bool) std::cout << std::endl;
}

// Sobrecarga int
void Logger::log(LogLevel msgLevel,
                 int msg,
                 bool flush_bool,
                 bool flag)
{
    if (msgLevel < level) return;

    if (flag) {
        switch (msgLevel) {
            case LogLevel::DEBUG: std::cout << "[DEBUG] "; break;
            case LogLevel::INFO:  std::cout << "[INFO] ";  break;
            case LogLevel::WARN:  std::cout << "[WARN] ";  break;
            case LogLevel::ERROR: std::cout << "[ERROR] "; break;
            case LogLevel::OUTPUT: break;
        }
    }

    std::cout << msg;
    if (flush_bool) std::cout << std::endl;
}

void Logger::flush() {
    std::cout << std::endl;
}
