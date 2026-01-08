#ifndef LOGGING_H
#define LOGGING_H

#include <iostream>

enum class LogLevel { DEBUG, INFO, WARN, ERROR, OUTPUT };

class Logger {
public:
    //inline static LogLevel level;
    inline static LogLevel level = LogLevel::INFO; // ✅ C++17 y posteriores

    static inline void log(LogLevel msgLevel, const std::string& msg, bool flush_bool = true) {
        if (msgLevel < level) return;
        switch (msgLevel) {
            case LogLevel::DEBUG: std::cout << "[DEBUG] "; break;
            case LogLevel::INFO:  std::cout << "[INFO] "; break;
            case LogLevel::WARN:  std::cout << "[WARN] "; break;
            case LogLevel::ERROR: std::cout << "[ERROR] "; break;
        };
        std::cout << msg;
        if (flush_bool){
            std::cout << std::endl;
        };
    }
};

// Inicializar el nivel de log
// LogLevel Logger::level = LogLevel::DEBUG;

# endif
