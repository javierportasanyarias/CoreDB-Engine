#!/usr/bin/env bash

g++ -std=c++20 \
  -Iexecution \
  -Iplanning_execution \
  -Inodes_for_trees \
  -Itext_manipulation \
  -Iglobals \
  -I"data_structure" \
  -Ilog \
  app/main.cpp \
  execution/execution.cpp \
  planning_execution/execution_planning.cpp \
  nodes_for_trees/node_for_trees.cpp \
  text_manipulation/process_tokens.cpp \
  text_manipulation/textutils.cpp \
  globals/globals.cpp \
  log/logging.cpp \
  -o app/sql_app.exe
