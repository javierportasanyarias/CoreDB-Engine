#ifndef GLOBALS_H
#define GLOBALS_H

#include <unordered_map>
#include <string>
// #include "data_structure/data_struct.h"
#include "data_struct.h"

extern std::unordered_map<std::string, table*> global_table_dict;

//extern uint32_t size_particion_bytes;
extern uint32_t size_buffer_bytes;

#endif
