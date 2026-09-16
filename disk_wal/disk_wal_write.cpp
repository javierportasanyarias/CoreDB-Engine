#include "disk_wal_write.h"

#include "disk_aux.h"
#include "disk_metadata.h"

//========================================================
//== FUNION ESCRITURA METADATOS EN EL WAL: ===============
//========================================================
void disk_wal_write::write_table_wal_metadata(table* table_ptr_input) {
  /*
  Writes metadata immediately after it is created in RAM.
  The metadata block consists of the following:
     1. Data type: 0, as it indicates metadata.
     2. Table name length.
     3. Table name.
     4. Data buffer size.
     5. Data buffer.
  The total row count is always written as zero, unlike
  standard metadata writes outside the WAL.
  */

  Logger::log(LogLevel::DEBUG,
              "Inside the metadata writing within the WAL file");
  /*
  Writes metadata to the WAL file for data persistence.
  */
  if (!table_ptr_input) return;
  // Not making metadata pointer an alias, as we do not neet to modify it
  table_metadata* metadata_pointer = table_ptr_input->metadata_ptr;
  std::string table_name = metadata_pointer->name;

  // Re-opening WAL file, this time in append mode:
  std::ofstream out(std::filesystem::path("backup_data/wal.bin"),
                    std::ios::binary | std::ios::app);

  /*
  We use a vector of char pointers to minimize the number of write calls.
  */

  uint32_t meta_byte_size =
      disk_metadata::calculate_metadata_byte_size_wal(metadata_pointer);
  char* buffer = nullptr;
  char* tmp_char_ptr = nullptr;
  try {
    buffer = new char[meta_byte_size];
    char* ptr_curr = buffer;
    /*
    Since this is a WAL write, we must first write separately beforehand:
    -> Data type (1 byte):
        * 0: metadata
        * 1: data
    -> Size (4 bytes):
        * Stores the size in bytes of the actual payload
    */
    // Writing data type:
    uint8_t data_type = 0;  // 0 porque es un metadato
    out.write(reinterpret_cast<char*>(&data_type), sizeof(uint8_t));
    // == Writing metadata: =======================================

    // -- Writing table's name ------------------------------------
    Logger::log(LogLevel::DEBUG, "Proceeding to write the table's name:");

    uint32_t table_name_size = table_name.size();
    out.write(reinterpret_cast<char*>(&table_name_size), sizeof(uint32_t));
    out.write(table_name.data(), table_name_size);

    Logger::log(LogLevel::DEBUG,
                "Table name registered within the buffer successfully");

    // -- Writing column number -----------------------------------
    uint32_t num_cols = metadata_pointer->n_cols;
    Logger::flush(LogLevel::DEBUG);
    Logger::log(LogLevel::DEBUG, "=========================================");
    Logger::log(LogLevel::DEBUG,
                "Nº of written columns in the WAl file: ", false, true);
    Logger::log(LogLevel::DEBUG, num_cols, true, false);
    Logger::log(LogLevel::DEBUG, "=========================================");
    Logger::flush(LogLevel::DEBUG);
    tmp_char_ptr = reinterpret_cast<char*>(&num_cols);
    std::memcpy(ptr_curr, tmp_char_ptr, sizeof(uint32_t));
    ptr_curr += sizeof(uint32_t);

    Logger::log(LogLevel::DEBUG, "Nº of columns registered successfully");

    for (uint32_t i = 0; i < num_cols; i++) {
      // -- Writing each column data --------------------------------

      // -- Writing column name:
      Logger::log(LogLevel::DEBUG, "Loop iteration: ", false, true);
      Logger::log(LogLevel::DEBUG, i, true, false);
      std::string column_name = (metadata_pointer->column_names)[i];
      uint32_t size_column_name = column_name.size();
      Logger::log(LogLevel::DEBUG,
                  "'column_name' and it's size recovered successfully");
      Logger::flush(LogLevel::DEBUG);

      Logger::log(LogLevel::DEBUG, "Inserting number of rows");
      tmp_char_ptr = reinterpret_cast<char*>(&size_column_name);
      std::memcpy(ptr_curr, tmp_char_ptr, sizeof(uint32_t));
      ptr_curr += sizeof(uint32_t);
      Logger::log(LogLevel::DEBUG,
                  "Nº of rows registered in the buffer successfully");

      Logger::log(LogLevel::DEBUG, "Inserting column name");
      tmp_char_ptr = column_name.data();
      std::memcpy(ptr_curr, tmp_char_ptr, size_column_name);
      ptr_curr += size_column_name;
      Logger::log(LogLevel::DEBUG,
                  "Column name registered within the buffer successfully");

      // -- Writing column data type:
      Logger::log(LogLevel::DEBUG, "Inserting data type");
      dataType col_type = (metadata_pointer->column_types)[i];
      uint32_t col_type_disk = static_cast<uint32_t>(col_type);

      tmp_char_ptr = reinterpret_cast<char*>(&col_type_disk);
      std::memcpy(ptr_curr, tmp_char_ptr, sizeof(uint32_t));
      ptr_curr += sizeof(uint32_t);
      Logger::log(LogLevel::DEBUG,
                  "Data type registered within the buffer successfully");

      // -- Writing primary key status(it is/is not primary key):
      Logger::log(LogLevel::DEBUG, "Accessing to primary key status");
      bool column_is_key = (metadata_pointer->primary_list)[i];
      Logger::log(LogLevel::DEBUG, "'column_is_key' accessed correctly");
      uint8_t key_val = column_is_key ? 1 : 0;
      Logger::log(LogLevel::DEBUG, "Boolean to integer conversion successful");
      Logger::log(LogLevel::DEBUG,
                  "Proceeding to register primary key boolean as integer in "
                  "the buffer");
      tmp_char_ptr = reinterpret_cast<char*>(&key_val);
      std::memcpy(ptr_curr, tmp_char_ptr, sizeof(uint8_t));
      ptr_curr += sizeof(uint8_t);
      Logger::log(LogLevel::DEBUG,
                  "Primary key status registered successfully");
    };
    Logger::log(LogLevel::DEBUG,
                "Metadata has already been all been written within the buffer");
    Logger::log(LogLevel::DEBUG,
                "As it is a WAL write, data type (metadata) and size must also "
                "be registered");
    /*
    The buffer is ready for writing, but since this is a WAL write, we must
    first write the size of the payload.
    */

    // Writing metadata size:
    Logger::log(LogLevel::DEBUG, "Registering metadata size");
    out.write(reinterpret_cast<char*>(&meta_byte_size), sizeof(uint32_t));
    Logger::log(LogLevel::DEBUG, "Metadata size written successfully");
    Logger::log(LogLevel::DEBUG, "Proceeding to write the metadata buffer:");
    // Writing buffer content on disk:
    disk_aux::aux_vector_buffer_write_disk(buffer, meta_byte_size, out);
    Logger::log(LogLevel::DEBUG, "Metadata buffer written SUCCESSFULLY");
    out.flush();
  } catch (...) {
    delete[] buffer;
    out.close();
    throw;
  };
  delete[] buffer;
  out.close();
  return;
};

//========================================================
//==== FUNION ESCRITURA DATOS EN EL WAL: =================
//========================================================

void disk_wal_write::walDataWriter::control_unit() {
  switch (this->state) {
    case 0: {
      Logger::log(LogLevel::DEBUG, "State 0");
      if (!this->table_obj) {
        this->state = 255;
        break;
      };
      table_metadata* metadata = this->table_obj->metadata_ptr;
      std::string table_name = metadata->name;

      this->out.open(std::filesystem::path("backup_data/wal.bin"),
                     std::ios::binary | std::ios::app);
      Logger::log(LogLevel::DEBUG, "WAL file has been opened");

      this->data_types = metadata->column_types;
      this->column_names = metadata->column_names;
      this->num_cols = metadata->n_cols;

      this->row_iterator = new disk_buffer::tableRowIterator_only_ram_for_wal(
          table_name, this->n_rows_to_write);

      uint8_t data_type = 1;
      this->out.write(reinterpret_cast<char*>(&data_type), sizeof(uint8_t));

      uint32_t table_name_size = table_name.size();
      this->out.write(reinterpret_cast<char*>(&table_name_size),
                      sizeof(uint32_t));
      this->out.write(table_name.data(), table_name_size);

      Logger::log(LogLevel::DEBUG,
                  "Inserted rows to write in WAL file: ", false, true);
      Logger::log(LogLevel::DEBUG, this->n_rows_to_write, true, false);
      this->out.write(reinterpret_cast<char*>(&this->n_rows_to_write),
                      sizeof(uint32_t));
      // Writitng buffer size:
      this->out.write(reinterpret_cast<char*>(&size_buffer_bytes),
                      sizeof(uint32_t));
      this->out.flush();

      this->state = 3;
      break;
    };

    case 1: {
      Logger::log(LogLevel::DEBUG, "State 1");
      if (this->row_iterator->is_eof()) {
        Logger::log(LogLevel::DEBUG, "General EOF");
        this->state = 7;
        break;
      } else {
        this->state = 2;
        break;
      };
    };

    case 2: {
      Logger::log(LogLevel::DEBUG, "State 2");
      Logger::log(LogLevel::DEBUG, "Current column: ", false, true);
      Logger::log(LogLevel::DEBUG, this->current_col, true, false);
      Logger::log(LogLevel::DEBUG, "Total columns: ", false, true);
      Logger::log(LogLevel::DEBUG, this->num_cols, true, false);
      if (this->eof_row()) {
        Logger::log(LogLevel::DEBUG, "Row EOF");
        this->state = 3;
        this->current_col = 0;
        break;
      } else {
        this->state = 4;
        break;
      };
    };

    case 3: {
      Logger::log(LogLevel::DEBUG, "State 3");
      this->row_to_write = this->row_iterator->get_next_row_ram();
      this->state = 4;
      break;
    };

    case 4: {
      Logger::log(LogLevel::DEBUG, "State 4");
      Logger::log(LogLevel::DEBUG, "Current column: ", false, true);
      Logger::log(LogLevel::DEBUG, this->current_col, true, false);
      Logger::log(LogLevel::DEBUG, "Column name: ", false, true);
      Logger::log(LogLevel::DEBUG, this->column_names[this->current_col], true,
                  false);
      Logger::log(LogLevel::DEBUG, "Column type: ", true, true);
      disk_aux::write_aux_val_buffer_with_size_check(
          this->row_to_write[this->column_names[this->current_col]],
          this->data_types[this->current_col], this->buffer_pointer,
          this->offset, this->bytes_written, this->bytes_remain,
          this->ptr_str_ini, this->current_col);
      this->state = 5;
      break;
    };

    case 5: {
      Logger::log(LogLevel::DEBUG, "State 5");
      Logger::log(LogLevel::DEBUG, "Written bytes: ", false, true);
      Logger::log(LogLevel::DEBUG, this->bytes_written, true, false);
      Logger::log(LogLevel::DEBUG, "Offset: ", false, true);
      Logger::log(LogLevel::DEBUG, this->offset, true, false);
      if (this->bytes_written + this->offset >= size_buffer_bytes) {
        Logger::log(LogLevel::DEBUG, "Buffer EOF");
        this->state = 6;
        break;
      } else {
        // If offset condition has not yet been reached
        this->state = 1;
        break;
      };
    };

    case 6: {
      Logger::log(LogLevel::DEBUG, "State 6");
      Logger::log(LogLevel::DEBUG, "State reached when buffer EOF happens");

      // LOG ENABLING USER TO SEE BUFFER SIZE:
      Logger::log(
          LogLevel::DEBUG,
          "?????????????????????????????????????????????????????????????");
      Logger::log_buffer(LogLevel::DEBUG, this->buffer, size_buffer_bytes, true,
                         true);
      Logger::flush(LogLevel::DEBUG);
      Logger::log(
          LogLevel::DEBUG,
          "?????????????????????????????????????????????????????????????");

      Logger::log(LogLevel::DEBUG, "Writing: ", false, true);
      Logger::log(LogLevel::DEBUG, size_buffer_bytes, false, false);
      Logger::log(LogLevel::DEBUG, " bytes", true, false);
      this->out.write(this->buffer, size_buffer_bytes);
      this->out.flush();
      this->offset = 0;
      this->bytes_written = 0;
      this->buffer_pointer = this->buffer;
      this->bytes_remain = size_buffer_bytes;
      this->state = 1;
      // Buffer reset to all zeros:
      std::memset(this->buffer, 0, size_buffer_bytes);
      break;
    };

    case 7: {
      Logger::log(LogLevel::DEBUG, "State 7");
      Logger::log(LogLevel::DEBUG, "Current column: ", false, true);
      Logger::log(LogLevel::DEBUG, this->current_col, true, false);
      Logger::log(LogLevel::DEBUG, "Total columns: ", false, true);
      Logger::log(LogLevel::DEBUG, this->num_cols, true, false);
      if (this->eof_row()) {
        Logger::log(LogLevel::DEBUG, "Rows EOF");
        this->state = 8;
        break;
      } else {
        this->state = 4;
        break;
      };
    };

    case 8: {
      Logger::log(LogLevel::DEBUG, "State 8");
      Logger::log(LogLevel::DEBUG, "'bytes_written': ", false, true);
      Logger::log(LogLevel::DEBUG, this->bytes_written, true, false);
      Logger::log(LogLevel::DEBUG, "Writing: ", false, true);
      Logger::log(LogLevel::DEBUG, size_buffer_bytes, false, false);
      Logger::log(LogLevel::DEBUG, " bytes", true, false);

      // We only write if there is something left to write:
      if (this->bytes_written > 0) {
        Logger::log(LogLevel::DEBUG,
                    "As 'bytes_written' > 0, leftover space will be filled "
                    "with padding bytes");
        this->out.write(this->buffer, size_buffer_bytes);
        this->out.flush();
      };
      this->state = 255;
      break;
    };

    case 255: {
      this->out.flush();
      Logger::log(LogLevel::DEBUG, "State 255");
      break;
    };
  };
};

void disk_wal_write::walDataWriter::execute_fsm() {
  bool aux_bool = true;
  while (aux_bool) {
    if (this->state == 255) {
      aux_bool = false;
    };
    this->control_unit();
  }
}

void disk_wal_write::write_table_data_wal(table* table_ptr_input,
                                          uint32_t n_rows_to_write_input) {
  // We create the data writer object:
  disk_wal_write::walDataWriter data_writer_obj(table_ptr_input,
                                                n_rows_to_write_input);
  // We execute the data writing FSM within:
  data_writer_obj.execute_fsm();
};
