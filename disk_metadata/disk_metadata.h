#pragma once

// Forward declarations:
struct table;
struct table_metadata;

namespace disk_metadata {

void read_table_metadata(std::filesystem::path path_table_input,
                         std::string table_name_str);
uint32_t calculate_metadata_byte_size(table_metadata* metadata_ptr);
uint32_t calculate_metadata_byte_size_wal(table_metadata* metadata_ptr);
uint32_t write_table_metadata(table* table_ptr_input);
void read_all_tables_metadata();

};  // namespace disk_metadata
