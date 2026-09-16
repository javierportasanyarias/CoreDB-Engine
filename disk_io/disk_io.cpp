#include "disk_io.h"

#include "disk_metadata.h"
#include "disk_out.h"
#include "disk_wal_utils.h"

////////////////////////////////////////////////////////////////////
// WRITE DUMP ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

// Function that iterates over the tables to be written:
void disk_io::write_dump() {
  // First, check if the dictionary is empty:
  if (global_table_dict.empty()) {
    // The dictionary is empty, exit
    Logger::log(LogLevel::DEBUG,
                "TABLE HASH MAP DOES NOT EXIST. PROGRAM TERMINATION");
    return;
  } else {
    // The dictionary contains content:
    for (const auto& [table_name, table_ptr] : global_table_dict) {
      int32_t n_rows;
      // Only send to write if the table exists:
      if (table_ptr != nullptr) {
        // Only allow writing if the data_ptr (not to be confused with the disk
        // buffer) is NOT empty:
        if (table_ptr->data_ptr) {
          if (!table_ptr->data_ptr->columns.empty()) {
            // First, write the metadata:
            uint32_t n_rows = 0;

            n_rows = disk_metadata::write_table_metadata(table_ptr);
            disk_out::write_table_data(table_ptr);

            // Update the table metadata to show the number of rows written to
            // disk:
            table_ptr->metadata_ptr->n_rows_disk = n_rows;
          } else {
            Logger::log(LogLevel::DEBUG,
                        "Table EXISTS within the hash map, but there is no "
                        "data in volatile memory. WRITE OPERATION CANCELED");
          };
        } else {
          Logger::log(LogLevel::DEBUG,
                      "Table EXISTS within the hash map. There is no data "
                      "entry. WRITE OPERATION CANCELED");
        };
      } else {
        Logger::log(LogLevel::DEBUG,
                    "ERROR: Table entry does not exist in global table "
                    "dictionary. Table name is: " +
                        table_name);
      };
    };
  };

  // Just before concluding the write, delete the WAL file:
  Logger::log(LogLevel::DEBUG, "WAL BACKUP FILE DELETION");
  disk_wal_utils::delete_wal_bin_file();
};

void disk_io::debug_print_mem_metadata() {
  Logger::log(LogLevel::DEBUG, "======= METADATA IN MEMORY DEBUG =======");

  if (global_table_dict.empty()) {
    Logger::log(LogLevel::ERROR, "Global table dictionary is EMPTY.");
    return;
  };

  for (auto const& [name_tmp, table_ptr] : global_table_dict) {
    Logger::log(LogLevel::DEBUG, "TABLE (Dict): " + name_tmp);

    if (!table_ptr || !table_ptr->metadata_ptr) {
      Logger::log(LogLevel::ERROR,
                  "  [!] Error: Table or metadata pointers are NULL");
      continue;
    };

    table_metadata* meta = table_ptr->metadata_ptr;
    Logger::log(LogLevel::DEBUG, "  Struct name: " + meta->name);
    Logger::log(LogLevel::DEBUG,
                "  Rows in disk:   " + std::to_string(meta->n_rows_disk));

    size_t n_cols = meta->n_cols;
    size_t n_types = meta->column_types.size();
    size_t n_pks = meta->primary_list.size();

    Logger::log(LogLevel::DEBUG, "  Data vectors:");
    Logger::log(LogLevel::DEBUG, "    - Names: " + std::to_string(n_cols));
    Logger::log(LogLevel::DEBUG, "    - Types:   " + std::to_string(n_types));
    Logger::log(LogLevel::DEBUG, "    - PKs:     " + std::to_string(n_pks));

    Logger::log(LogLevel::DEBUG, "  COLUMNS DETAIL:");
    // Iterate over the maximum found to detect mismatches
    size_t max_idx = std::max({n_cols, n_types, n_pks});

    for (size_t i = 0; i < max_idx; i++) {
      std::string col_name =
          (i < n_cols) ? meta->column_names[i] : "!!! MISSING NAME !!!";
      std::string type_str = "NOT_READ";

      if (i < n_types) {
        switch (meta->column_types[i]) {
          case dataType::INT:
            type_str = "INT";
            break;
          case dataType::FLOAT:
            type_str = "FLOAT";
            break;
          case dataType::STRING:
            type_str = "STRING";
            break;
          case dataType::BOOL:
            type_str = "BOOL";
            break;
          case dataType::UNKNOWN:
            type_str = "UNKNOWN";
            break;
        }
      }

      std::string is_pk =
          (i < n_pks && meta->primary_list[i]) ? "[PK]" : "    ";

      Logger::log(LogLevel::DEBUG, "    [" + std::to_string(i) + "] " + is_pk +
                                       " " + col_name + " (" + type_str + ")");
    }
  };
  Logger::log(LogLevel::DEBUG,
              "==============================================");
  if (Logger::level == LogLevel::DEBUG) {
    Logger::flush(LogLevel::DEBUG);
  } else {
    Logger::flush(LogLevel::DEBUG, false);
  };
};
