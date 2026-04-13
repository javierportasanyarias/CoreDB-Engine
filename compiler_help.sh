#!/usr/bin/env bash

clang++ -std=c++20 \
  -Iexecution \
  -Iplanning_execution \
  -Inodes_for_trees \
  -Itext_manipulation \
  -Iglobals \
  -I"data_structure" \
  -Ilog \
  -I"disk_io" \
  -I"disk_wal" \
  -Itests \
  -Idisk_buffer \
  app/main.cpp \
  execution/execution.cpp \
  planning_execution/execution_planning.cpp \
  nodes_for_trees/node_for_trees.cpp \
  text_manipulation/process_tokens.cpp \
  text_manipulation/textutils.cpp \
  globals/globals.cpp \
  log/logging.cpp \
  tests/data_structs.cpp \
  tests/bateria_tests.cpp \
  disk_io/disk_io.cpp \
  disk_wal/disk_wal.cpp \
  disk_buffer/disk_buffer.cpp \
  -o app/sql_app.exe
