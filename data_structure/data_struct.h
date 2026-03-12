#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H

#include <iostream>
#include <string>
#include <vector>
#include <variant>
//#include <unordered_map>
#include <map>

enum class dataType {

   INT,
   FLOAT,
   STRING,
   BOOL,
   UNKNOWN
};

struct table_metadata {
    std::string name;
    std::vector<std::string> column_names;
    std::vector<dataType> column_types;
    std::vector<bool> primary_list;
    uint32_t n_filas_disco;
    uint32_t n_filas_ram;
    table_metadata():
	  name(""), 
          n_filas_disco(0), 
          n_filas_ram(0) 
    {};
};

using Values = std::variant<int, float, bool, std::string>;

struct table_data {
	std::map<std::string, std::vector<Values>> columns;
};

struct table_data_buffer {
   std::map<std::string, std::vector<Values>> columns;
};

struct table {

   table_metadata* metadata_ptr;
   table_data* data_ptr;
   table_data_buffer* data_buffer_ptr;

   table(): metadata_ptr(nullptr), data_ptr(nullptr), data_buffer_ptr(nullptr){};
};

#endif
