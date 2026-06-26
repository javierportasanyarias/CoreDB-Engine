#ifndef LOGGING_H
#define LOGGING_H

#include <iostream>
#include <string>
#include <variant>
#include <vector>
#include <sstream>
#include <iomanip>


enum class LogLevel { DEBUG, INFO, WARN, ERROR, OUTPUT };


class Logger {
public:
    static LogLevel level;

    static void log(LogLevel msgLevel,
                    const std::string& msg,
                    bool flush_bool = true,
                    bool flag = true);

    static void log(LogLevel msgLevel,
                    const char* msg,
                    bool flush_bool = true,
                    bool flag = true);

    static void log(LogLevel msgLevel,
                    int msg,
                    bool flush_bool = true,
                    bool flag = true);

    static void log(LogLevel msgLevel,
                    std::variant<int, float, bool, std::string, std::vector<char>>  msg,
                    bool flush_bool = true,
                    bool flag = true);

    static void log_buffer(LogLevel msgLevel,
                           const char* buffer,
                           uint32_t buffer_len,
                           bool flush_bool = true,
                           bool flag = true);

    static void flush(bool endl = true);

    static void login(std::string& input);

    static void clear_in();
};

#endif
