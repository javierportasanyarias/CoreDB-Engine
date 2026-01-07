#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H

#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include <unordered_map>

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

    table_metadata() 
        : name(""),
          column_names(),
          column_types(),
          primary_list()
    {}
};

using Values = std::variant<int, float, bool, std::string>;
struct table_data {

   std::unordered_map<std::string, std::vector<Values>> columns;
};

struct table {

   table_metadata* metadata_ptr;
   table_data* data_ptr;

   table(): metadata_ptr(nullptr), data_ptr(nullptr){};
};



#endif
