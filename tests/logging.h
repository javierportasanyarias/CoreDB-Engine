#ifndef LOGGING_TESTS_H
#define LOGGING_TESTS_H

#include <iostream>
#include <string>

enum class TestlogLevel { DEBUG, INFO, WARN, ERROR, OUTPUT };

class TestLogger {
 public:
  static TestlogLevel level;

  static void log(TestlogLevel msgLevel, const std::string& msg,
                  bool flush_bool = true, bool flag = true);

  static void log(TestlogLevel msgLevel, int msg, bool flush_bool = true,
                  bool flag = true);

  static void flush(bool endl = true);
};

#endif
