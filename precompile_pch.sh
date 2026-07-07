#!/usr/bin/env bash

clang++ -std=c++20 \
  -x c++-header \
  -Idata_structure \
  -Ilog \
  -Iglobals \
  pch_app.h \
  -o pch_app.h.pch