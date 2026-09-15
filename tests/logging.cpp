#include "logging.h"

#include <iostream>

// Static member definition:
TestlogLevel TestLogger::level = TestlogLevel::INFO;

// String overload:
void TestLogger::log(TestlogLevel msgLevel, const std::string& msg,
                     bool flush_bool, bool flag) {
  if (msgLevel < level) return;

  if (flag) {
    switch (msgLevel) {
      case TestlogLevel::DEBUG:
        std::cout << "[DEBUG] ";
        break;
      case TestlogLevel::INFO:
        std::cout << "[INFO] ";
        break;
      case TestlogLevel::WARN:
        std::cout << "[WARN] ";
        break;
      case TestlogLevel::ERROR:
        std::cout << "[ERROR] ";
        break;
      case TestlogLevel::OUTPUT:
        break;
    }
  }

  std::cout << msg;
  if (flush_bool) std::cout << std::endl;
}

// Int overload:
void TestLogger::log(TestlogLevel msgLevel, int msg, bool flush_bool,
                     bool flag) {
  if (msgLevel < level) return;

  if (flag) {
    switch (msgLevel) {
      case TestlogLevel::DEBUG:
        std::cout << "[DEBUG] ";
        break;
      case TestlogLevel::INFO:
        std::cout << "[INFO] ";
        break;
      case TestlogLevel::WARN:
        std::cout << "[WARN] ";
        break;
      case TestlogLevel::ERROR:
        std::cout << "[ERROR] ";
        break;
      case TestlogLevel::OUTPUT:
        break;
    }
  }

  std::cout << msg;
  if (flush_bool) std::cout << std::endl;
}

void TestLogger::flush(bool endl) {
  if (endl) {
    std::cout << std::endl;
  };
  std::cout.flush();
}
