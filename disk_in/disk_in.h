#pragma once

namespace disk_in {

void read_fixed_len_int_columns(std::filesystem::path path_var,
                                std::string& partition_current,
                                std::vector<Values>& vec_vals);
void read_fixed_len_float_columns(std::filesystem::path path_var,
                                  std::string& partition_current,
                                  std::vector<Values>& vec_vals);
void read_fixed_len_bool_columns(std::filesystem::path path_var,
                                 std::string& partition_current,
                                 std::vector<Values>& vec_vals);

class read_table_iterator {
  /*
  Class that will read and insert the contents of the partitions into the
  table's memory one by one.
  */
 public:
  uint32_t partition_counter;
  uint32_t total_partitions;
  std::vector<std::string> partition_names;
  table* table_ptr;
  bool partition_eof;  // Indicates if there are no more partitions left

  std::string table_name;
  std::vector<dataType> column_types;
  uint32_t num_cols;
  std::vector<std::string> column_names;
  table_data_buffer* disk_data_ptr;

  uint32_t current_partition_n_rows;

  // Constructor method
  read_table_iterator(table* table_ptr_input);

  bool obtain_int_partition_rows(bool aux_bool, std::filesystem::path path_var,
                                 std::string& partition_current);
  bool obtain_float_partition_rows(bool aux_bool,
                                   std::filesystem::path path_var,
                                   std::string& partition_current);
  bool obtain_bool_partition_rows(bool aux_bool, std::filesystem::path path_var,
                                  std::string& partition_current);
  bool obtain_string_partition_rows(bool aux_bool,
                                    std::filesystem::path path_var,
                                    std::string& partition_current);

  // Interactive reading through partitions
  bool read_table();
};

};  // namespace disk_in
