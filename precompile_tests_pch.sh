#!/usr/bin/env bash

clang++ -std=c++20 \
  -gdwarf-4 -O0 \
  -x c++-header \
  tests/tests_pch.h \
  -o tests/tests_pch.h.pch
