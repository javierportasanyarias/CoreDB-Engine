#pragma once

enum class dataType {
  /*
  All data types considered in this SQL database engine:
     -> INT: Equivalent to 'int' in C++.
     -> FLOAT: Equivalent to 'float' in C++.
     -> STRING: Equivalent to 'std::string' in C++.
     -> BOOL: Equivalent to 'bool' in C++.
     -> UNKNOWN: unknown data type. It will be interpeted as a character array
  in C++.
  */
  INT,
  FLOAT,
  STRING,
  BOOL,
  UNKNOWN
};

struct table_metadata {
  /*
  Table metada struct. It's attributes are the following:
     -> name: nombre of the table.
     -> column_names: Column names. The order is defined by their insertion.
     -> column_types: Column types. The order is also determied by insertion
  order.
     -> primary_list: Boolean array indicationg if the filed is a primary key or
  not. The order is also determied by insertion order.
     -> n_rows_disk: Nº of rows in disk.
     -> n_rows_ram: Nº of rows defined within the volatile memory associated
  with a current program execution.

  Inicialization:
     -> name as an empty string
     -> n_rows_disk as zero
     -> n_rows_ram as zero

  */
  std::string name;
  std::vector<std::string> column_names;
  std::vector<dataType> column_types;
  std::vector<bool> primary_list;
  uint32_t n_cols;
  uint32_t n_rows_disk;
  uint32_t n_rows_ram;
  table_metadata() : name(""), n_cols(0), n_rows_disk(0), n_rows_ram(0){};
};

// Variant variable holding all the table possible types of data within the
// program:
using Values = std::variant<int, float, bool, std::string, std::vector<char>>;

struct table_data {
  /*
  Data associated with the information added within the same session/execution
  of the engine. The data is presented as a hash map in which:
     -> The key id the name of the column.
     -> The value is an std::vector containing it's values.
  */
  std::map<std::string, std::vector<Values>> columns;
};

struct table_data_buffer {
  /*
  Data associated with the information retrieved for the disk.
  The data is presented as a hash map in which:
     -> The key id the name of the column.
     -> The value is an std::vector containing it's values.
  */
  std::map<std::string, std::vector<Values>> columns;
};

struct table {
  /*
  Table's struct. It holds the following pointers:
     -> metadata_ptr: metadata pointer.
     -> data_ptr: pointer to the data defined in the current execution (within
  volatile memory).
     -> data_buffer_ptr: pointer to the data retrieved form disk.

  These three poniters are initialized as null when a table instance is created,
  for greater memory security.
  */
  table_metadata* metadata_ptr;
  table_data* data_ptr;
  table_data_buffer* data_buffer_ptr;

  table()
      : metadata_ptr(nullptr), data_ptr(nullptr), data_buffer_ptr(nullptr){};

  ~table() {
    delete metadata_ptr;
    delete data_ptr;
    delete data_buffer_ptr;
  }
};
