#include "disk_io.h"
#include "disk_aux.h"
#include "disk_metadata.h"

////////////////////////////////////////////////////////////////////
// METADATA /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void disk_metadata::read_table_metadata(std::filesystem::path path_table_input, std::string table_name_str) {

   table_metadata*& metadata_pointer = global_table_dict.at(table_name_str)->metadata_ptr;

   // BEGIN READING:
   std::ifstream in(path_table_input, std::ios::binary);

   // We obtain the metadata file size in bytes:
   uint32_t file_size_bytes = 0;
   file_size_bytes = disk_aux::return_file_size_bytes(in);
   // We only proceed if there is information to be read:
   if (file_size_bytes == 0) {
      in.close();
      return;
   };

   // We create a character buffer to hold all the read data:
   char* buffer_meta = nullptr;
   try {
      buffer_meta = new char[file_size_bytes];
      // We proceed to read the entire file
      in.read(buffer_meta, file_size_bytes);
      in.close();
      const char* ptr_curr = buffer_meta;
      const char* ptr_end = ptr_curr + file_size_bytes;

      // Once read, we iterate over the buffer to extract the necessary values
      // Read the name size:
      uint32_t size_name = 0;
      std::memcpy(&size_name, ptr_curr, sizeof(uint32_t));
      ptr_curr += sizeof(uint32_t);

      // Read the name:
      std::string table_name(size_name, '\0');
      std::memcpy(table_name.data(), ptr_curr, size_name);
      ptr_curr += size_name;
      metadata_pointer->name = table_name;

      // Read the number of columns:
      uint32_t num_cols_reading = 0;
      std::memcpy(&num_cols_reading, ptr_curr, sizeof(uint32_t));
      metadata_pointer->n_cols = num_cols_reading;
      ptr_curr += sizeof(uint32_t);
      // We will use this value to iterate over each column

      // Read the number of rows on disk:
      uint32_t disk_reading_rows_number = 0;
      std::memcpy(&disk_reading_rows_number, ptr_curr, sizeof(uint32_t));
      metadata_pointer->n_rows_disk = disk_reading_rows_number;
      ptr_curr += sizeof(uint32_t);
      Logger::log(LogLevel::DEBUG, "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
      Logger::log(LogLevel::DEBUG, "Number of rows in 'RAM': ", false, true);
      Logger::log(LogLevel::DEBUG, disk_reading_rows_number, true, false);
      Logger::log(LogLevel::DEBUG, "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");

      // Now we iterate over each column, adding metadata for each one:
      for (uint32_t i = 0; i < num_cols_reading; i++) {
         // Read the column name:
         uint32_t size_column_name;
         std::memcpy(&size_column_name, ptr_curr, sizeof(uint32_t));
         ptr_curr += sizeof(uint32_t);
         std::string column_name_read(size_column_name, '\0');
         std::memcpy(column_name_read.data(), ptr_curr, size_column_name);
         ptr_curr += size_column_name;
         metadata_pointer->column_names.push_back(column_name_read);

         // Read the data type:
         uint32_t col_type_int;
         std::memcpy(&col_type_int, ptr_curr, sizeof(uint32_t));
         ptr_curr += sizeof(uint32_t);
         dataType col_type = static_cast<dataType>(col_type_int);
         metadata_pointer->column_types.push_back(col_type);

         // Retrieve if it is a primary key:
         uint8_t column_is_key_num;
         bool column_is_key;
         std::memcpy(&column_is_key_num, ptr_curr, sizeof(uint8_t));
         ptr_curr += sizeof(uint8_t);
         if (column_is_key_num == 1) {
            column_is_key = true;
         } else {
            column_is_key = false;
         };
         metadata_pointer->primary_list.push_back(column_is_key);

         // We skip the number of columns of size uint32_t
         // In reality, we don't need to read anything more

      };
      Logger::log(LogLevel::DEBUG, "Metadata read successfully");
      if (Logger::level == LogLevel::DEBUG) {
         Logger::flush(LogLevel::DEBUG);
      } else {
         Logger::flush(LogLevel::DEBUG, false);
      };
   } catch (...) {
      // Here we first delete the character buffer in the heap and then throw the error:
      delete[] buffer_meta;
      throw;
   };
   // In case no program error was detected, we proceed to delete the buffer:
   delete[] buffer_meta;

};

uint32_t disk_metadata::calculate_metadata_byte_size(table_metadata* metadata_ptr) {
   /*
   Function designed to calculate the total byte size
   of the data stored within the table's metadata
   */
   uint32_t meta_byte_size = 0;
   if (!metadata_ptr) {
      return meta_byte_size;
   };

   meta_byte_size += metadata_ptr->name.size();
   meta_byte_size += sizeof(uint32_t);

   uint32_t num_cols = metadata_ptr->n_cols;

   // We add up the size of: n_cols, n_rows_disk and n_rows_ram:
   // In reality, we only write n_cols to disk:
   meta_byte_size += sizeof(uint32_t) * 1;

   // We add up the size of each element contained in: column_names, column_types and primary_list (as 8-bit integer)
   const std::vector<std::string>& col_name_list = metadata_ptr->column_names;
   for (uint32_t i = 0; i < num_cols; i++) {
      meta_byte_size += sizeof(uint32_t);
      meta_byte_size += col_name_list[i].size();

      meta_byte_size += sizeof(uint32_t);

      meta_byte_size += sizeof(uint8_t);
   };
   return meta_byte_size;
};

uint32_t disk_metadata::calculate_metadata_byte_size_wal(table_metadata* metadata_ptr) {
   /*
   Function designed to calculate the total byte size
   of the data stored within the table's metadata
   */

   uint32_t meta_byte_size = 0;

   if (!metadata_ptr) {
      return meta_byte_size;
   };

   uint32_t num_cols = metadata_ptr->n_cols;

   // We add up the size of: n_cols, n_rows_disk and n_rows_ram:
   // In reality, we only write n_cols to disk:
   meta_byte_size += sizeof(uint32_t) * 1;

   // We add up the size of each element contained in: column_names, column_types and primary_list (as 8-bit integer)
   const std::vector<std::string>& col_name_list = metadata_ptr->column_names;
   for (uint32_t i = 0; i < num_cols; i++) {

      meta_byte_size += sizeof(uint32_t);
      meta_byte_size += col_name_list[i].size();

      meta_byte_size += sizeof(uint32_t);

      meta_byte_size += sizeof(uint8_t);
   };
   return meta_byte_size;
};

uint32_t disk_metadata::write_table_metadata(table* table_ptr_input) {
   /*
   Function to write metadata to disk.
   */
   if (!table_ptr_input) return 0;
   // We do not make metadata pointer an alias as we do not need to modify any values
   table_metadata* metadata_pointer = table_ptr_input->metadata_ptr;

   // First we calculate the metadata's size in bytes:
   uint32_t meta_byte_size = disk_metadata::calculate_metadata_byte_size(metadata_pointer);
   if (meta_byte_size == 0) {
      return 0;
   };

   std::string table_name = metadata_pointer->name;
   std::filesystem::path path_table = std::filesystem::path("metadata");
   path_table /= table_name;
   path_table += "_meta.bin";

   // Open for writing:
   std::ofstream out(path_table, std::ios::binary | std::ios::out);

   /*
   We use a vector of character pointers to avoid
   making too many write calls
   */

   char* buffer = nullptr;
   try {
      buffer = new char[meta_byte_size];

      char* ptr_curr = buffer;

      char* tmp_char_ptr = nullptr;

      // == Write metadata: ===============================

      // -- Write name -----------------------------------------
      uint32_t size_name = table_name.size();
      tmp_char_ptr = reinterpret_cast<char*>(&size_name);
      std::memcpy(ptr_curr, tmp_char_ptr, sizeof(uint32_t));
      ptr_curr += sizeof(uint32_t);

      tmp_char_ptr = table_name.data();
      std::memcpy(ptr_curr, tmp_char_ptr, size_name);
      ptr_curr += size_name;

      // -- Write number of columns -----------------------------
      uint32_t num_cols = metadata_pointer->n_cols;
      tmp_char_ptr = reinterpret_cast<char*>(&num_cols);
      std::memcpy(ptr_curr, tmp_char_ptr, sizeof(uint32_t));

      ptr_curr += sizeof(uint32_t);

      std::map<std::string, std::vector<Values>> columnas = table_ptr_input->data_ptr->columns;

      uint32_t num_rows_mem = metadata_pointer->n_rows_ram;
      uint32_t num_rows_disk = metadata_pointer->n_rows_disk;
      uint32_t n_rows_total = num_rows_mem + num_rows_disk;
      Logger::log(LogLevel::DEBUG, "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
      Logger::log(LogLevel::DEBUG, "Metadata writing");
      Logger::log(LogLevel::DEBUG, "Rows in-memory: ", false, true);
      Logger::log(LogLevel::DEBUG, num_rows_mem, true, false);
      Logger::log(LogLevel::DEBUG, "Disk rows: ", false, true);
      Logger::log(LogLevel::DEBUG, num_rows_disk, true, false);
      Logger::log(LogLevel::DEBUG, "Total rows: ", false, false);
      Logger::log(LogLevel::DEBUG, n_rows_total, true, false);
      Logger::log(LogLevel::DEBUG, "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
      tmp_char_ptr = reinterpret_cast<char*>(&n_rows_total);
      std::memcpy(ptr_curr, tmp_char_ptr, sizeof(uint32_t));
      ptr_curr += sizeof(uint32_t);

      for (uint32_t i = 0; i < num_cols; i++) {
         // -- Write column data ---------------------

         // -- Write name:
         std::string column_name = (metadata_pointer->column_names)[i];
         uint32_t size_column_name = column_name.size();
         tmp_char_ptr = reinterpret_cast<char*>(&size_column_name);
         std::memcpy(ptr_curr, tmp_char_ptr, sizeof(uint32_t));
         ptr_curr += sizeof(uint32_t);

         tmp_char_ptr = column_name.data();
         std::memcpy(ptr_curr, tmp_char_ptr, size_column_name);
         ptr_curr += size_column_name;

         // -- Write data type:
         dataType col_type = (metadata_pointer->column_types)[i];
         uint32_t col_type_disk = static_cast<uint32_t>(col_type);
         tmp_char_ptr = reinterpret_cast<char*>(&col_type_disk);
         std::memcpy(ptr_curr, tmp_char_ptr, sizeof(uint32_t));
         ptr_curr += sizeof(uint32_t);

         // -- Write if primary key:
         bool column_is_key = (metadata_pointer->primary_list)[i];
         uint8_t key_val = column_is_key ? 1 : 0;
         tmp_char_ptr = reinterpret_cast<char*>(&key_val);
         std::memcpy(ptr_curr, tmp_char_ptr, sizeof(uint8_t));
         ptr_curr += sizeof(uint8_t);

      };
      // Now we iterate over the vectors and perform the actual write
      disk_aux::aux_vector_buffer_write_disk(buffer,
                                             meta_byte_size,
                                             out
                                             );
      out.flush();
      out.close();
      delete[] buffer;
      return num_rows_mem;
   } catch (...) {
      out.close();
      delete[] buffer;
      throw;
   };
};

////////////////////////////////////////////////////////////////////
// READ ALL METADATA //////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void disk_metadata::read_all_tables_metadata() {

   std::vector<std::filesystem::path> tables_arr;
   tables_arr = disk_aux::scan_tables();
   for (int i = 0; i < tables_arr.size(); i++) {
      // Create the table object and its entry in the global dictionary:
      // Initialize the global dictionary and its elements:
      table* table_ptr;
      // REGISTER TABLE NAME WITHOUT THE '_meta' SUFFIX:
      std::string table_name_str = tables_arr[i].stem().string();
      if (table_name_str.ends_with("_meta")) {
         table_name_str.erase(table_name_str.size() - 5);
      };
      global_table_dict[table_name_str] = new table;
      table_ptr = global_table_dict.at(table_name_str);
      table_ptr->metadata_ptr = new table_metadata();
      table_ptr->data_ptr = new table_data();
      disk_metadata::read_table_metadata(tables_arr[i], table_name_str);
   };
   disk_io::debug_print_mem_metadata();
};
