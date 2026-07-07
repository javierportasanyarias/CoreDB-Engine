#pragma once

#include "data_struct.h"

struct table;

inline std::unordered_map<std::string, table*> global_table_dict;

//extern uint32_t size_particion_bytes;
inline uint32_t size_buffer_bytes;
