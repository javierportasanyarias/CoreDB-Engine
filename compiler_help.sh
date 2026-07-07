#!/usr/bin/env bash

clang++ -std=c++20 \
  -include-pch pch_app.h.pch \
  -Iexecution \
  -Iplanning_execution \
  -Inodes_for_trees \
  -Itext_manipulation \
  -I"disk_io" \
  -I"disk_in" \
  -I"disk_out" \
  -I"disk_metadata" \
  -I"disk_aux" \
  -I"disk_wal" \
  -I"disk_buffer" \
  -Ipartition_sorting \
  log/logging.cpp \
  app/main.cpp \
  execution/execution.cpp \
  planning_execution/execution_planning.cpp \
  nodes_for_trees/node_for_trees.cpp \
  text_manipulation/process_tokens.cpp \
  text_manipulation/textutils.cpp \
  disk_io/disk_io.cpp \
  disk_in/disk_in.cpp \
  disk_out/disk_out.cpp \
  disk_metadata/disk_metadata.cpp \
  disk_aux/disk_aux.cpp \
  disk_wal/disk_wal_write.cpp \
  disk_wal/disk_wal_read.cpp \
  disk_wal/disk_wal_utils.cpp \
  disk_buffer/disk_buffer.cpp \
  partition_sorting/part_sort.cpp \
  -o app/sql_app.exe
