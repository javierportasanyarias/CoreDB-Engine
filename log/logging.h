#ifndef LOGGING_H
#define LOGGING_H

#include <iostream>
#include <string>
#include <variant>


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
                    std::variant<int, float, bool, std::string>  msg,
                    bool flush_bool = true,
                    bool flag = true);

    static void flush(bool endl = true);

    static void login(std::string& input);

    static void clear_in();
};

#endif
