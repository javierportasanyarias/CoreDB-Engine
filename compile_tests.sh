#!/usr/bin/env bash

clang++ -std=c++20 \
  -include-pch tests/tests_pch.h.pch \
  -Itests \
  tests/data_structs.cpp \
  tests/bateria_tests.cpp \
  tests/tests_3.cpp \
  tests/logging.cpp \
  -o tests/test_exe.exe
