#pragma once

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
                    const int msg,
                    bool flush_bool = true,
                    bool flag = true);

    static void log(LogLevel msgLevel,
                    const std::variant<int, float, bool, std::string, std::vector<char>>  msg,
                    bool flush_bool = true,
                    bool flag = true);

    static void log(LogLevel msgLevel,
                    const std::filesystem::path msg,
                    bool flush_bool = true,
                    bool flag = true);

    static void log_buffer(LogLevel msgLevel,
                           const char* buffer,
                           uint32_t buffer_len,
                           bool flush_bool = true,
                           bool flag = true);

    static void flush(LogLevel msgLevel = LogLevel::DEBUG,
                      bool endl = true);

    static void login(std::string& input);

    static void clear_in();
};
