#!/usr/bin/env bash

clang++ -std=c++20 \
  -Iexperiments \
  experiments/main_2.cpp \
  -o experiments/main.exe
