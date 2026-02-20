#ifndef LOGGING_TESTS_H
#define LOGGING_TESTS_H

#include <iostream>
#include <string>

enum class LogLevel { DEBUG, INFO, WARN, ERROR, OUTPUT };

class Logger {
public:
    static LogLevel level;

    static void log(LogLevel msgLevel,
                    const std::string& msg,
                    bool flush_bool = true,
                    bool flag = true);

    static void log(LogLevel msgLevel,
                    int msg,
                    bool flush_bool = true,
                    bool flag = true);

    static void flush(bool endl = true);
};

#endif
