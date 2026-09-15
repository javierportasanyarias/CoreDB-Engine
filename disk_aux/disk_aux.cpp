#include "disk_aux.h"

//====================================================
//================= Auxiliary functions ==============
//====================================================

void disk_aux::aux_vector_buffer_write_disk(char* ptr_ini, uint32_t size_buffer,
                                            std::ofstream& out) {
  if (out.is_open()) {
    // Write entire buffer:
    out.write(ptr_ini, size_buffer);
  };
};

std::vector<std::string> disk_aux::obtain_files_in_path(
    const std::filesystem::path& path, dataType data_type) {
  std::vector<std::string> files;
  /*
  Method for obtaining a vector containing the file names within a certain path
  and data type.
  */
  // 1. Path verification:
  if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) {
    return files;
  }

  // 2. Folder element iteration:
  for (const auto& entry : std::filesystem::directory_iterator(path)) {
    // We verify that it is a regular file (and not a folder)
    if (!std::filesystem::is_regular_file(entry.path())) {
      continue;
    };
    if (data_type == dataType::STRING) {
      if (entry.path().extension() == ".idx") {
        // We only keep the partition if it presents the ".idx" extension:
        files.push_back(entry.path().filename().string());
      };
    } else {
      // We keep only the file's name (e.g., "part_00001.dat"):
      files.push_back(entry.path().filename().string());
    };
  };

  return files;
};

uint32_t disk_aux::obtain_file_size(const std::filesystem::path& path) {
  /*
  Method for obtaining a file's size given its path.
  */
  Logger::log(LogLevel::DEBUG, "Inside the 'obtain_file_size' function");

  std::ifstream archivo(path, std::ios::binary | std::ios::ate);

  if (!archivo.is_open()) {
    Logger::log(LogLevel::DEBUG,
                "ERROR while opening the file for obtaining the file size");
    return 0;  // Error while opening the file
  };

  Logger::log(LogLevel::DEBUG, "File opened SUCCESSFULLY");

  long tamano = archivo.tellg();

  archivo.close();
  return tamano;
};

void disk_aux::write_aux_val_buffer_with_size_check(
    const Values& value, dataType data_type, char*& buffer_pointer,
    uint32_t& offset, uint32_t& bytes_written, uint32_t& bytes_remain,
    char*& ptr_str_ini, uint32_t& current_col) {
  /*
   * Similar to 'write_aux_val', but writes a character buffer instead.
   * Buffering batches operations into a single disk write, minimizing
   * write latency. Subsequent reads from this buffer also yield
   * significant performance gains.
   */

  char* tmp_char_ptr = nullptr;
  char*& buffer_ptr = buffer_pointer;

  switch (data_type) {
    case dataType::INT: {
      uint8_t size_int = sizeof(int);
      if (size_int <= bytes_remain) {
        int int_val;
        int_val = std::get<int>(value);
        tmp_char_ptr = reinterpret_cast<char*>(&int_val);
        std::memcpy(buffer_ptr, tmp_char_ptr, size_int);
        buffer_ptr += size_int;
        bytes_remain -= size_int;
        bytes_written += size_int;
        current_col += 1;
        return;
      };
      offset = size_int;
      return;
    };
    case dataType::FLOAT: {
      uint8_t size_float = sizeof(float);
      if (size_float <= bytes_remain) {
        float float_val;
        float_val = std::get<float>(value);
        tmp_char_ptr = reinterpret_cast<char*>(&float_val);
        std::memcpy(buffer_ptr, tmp_char_ptr, size_float);
        buffer_ptr += size_float;
        bytes_remain -= size_float;
        bytes_written += size_float;
        current_col += 1;
        return;
      };
      offset = size_float;
      return;
    };
    case dataType::BOOL: {
      uint8_t size_bool_int8 = sizeof(uint8_t);
      if (size_bool_int8 <= bytes_remain) {
        bool buffer_bool;
        uint8_t buffer_int8;
        buffer_bool = std::get<bool>(value);
        if (buffer_bool) {
          buffer_int8 = 1;
        } else {
          buffer_int8 = 0;
        };
        tmp_char_ptr = reinterpret_cast<char*>(&buffer_int8);
        std::memcpy(buffer_ptr, tmp_char_ptr, size_bool_int8);
        buffer_ptr += size_bool_int8;
        bytes_remain -= size_bool_int8;
        bytes_written += size_bool_int8;
        current_col += 1;
        return;
      };
      offset = size_bool_int8;
      return;
    };

    case dataType::STRING: {
      const std::string& string_val = std::get<std::string>(value);
      uint32_t string_size;
      string_size = string_val.size();
      uint8_t uint32_size = sizeof(uint32_t);

      Logger::log(LogLevel::DEBUG,
                  "[WRITE_STRING] >>> Auxiliary function additional entry <<<");
      Logger::log(LogLevel::DEBUG,
                  "[WRITE_STRING] Text content: " + string_val);
      Logger::log(LogLevel::DEBUG, "[WRITE_STRING] string_size: ", false, true);
      Logger::log(LogLevel::DEBUG, string_size, true, false);
      Logger::log(LogLevel::DEBUG,
                  "[WRITE_STRING] bytes_remain in buffer: ", false, true);
      Logger::log(LogLevel::DEBUG, bytes_remain, true, false);
      Logger::log(LogLevel::DEBUG,
                  "[WRITE_STRING] Base address for string_val.data(): ", false,
                  true);
      Logger::log(LogLevel::DEBUG,
                  reinterpret_cast<uintptr_t>(string_val.data()), true, false);
      Logger::log(LogLevel::DEBUG,
                  "[WRITE_STRING] ptr_str_ini initial value: ", false, true);
      Logger::log(LogLevel::DEBUG, reinterpret_cast<uintptr_t>(ptr_str_ini),
                  true, false);

      // Phase 1 -> Reading the string's size:
      if (!ptr_str_ini) {
        Logger::log(LogLevel::DEBUG,
                    "[WRITE_STRING] [Phase 1] ptr_str_ini is nullptr. Trying "
                    "to write its size...");
        tmp_char_ptr = reinterpret_cast<char*>(&string_size);
        if (uint32_size <= bytes_remain) {
          tmp_char_ptr = reinterpret_cast<char*>(&string_size);
          std::memcpy(buffer_ptr, tmp_char_ptr, uint32_size);
          buffer_ptr += uint32_size;
          bytes_remain -= uint32_size;
          bytes_written += uint32_size;
          ptr_str_ini = const_cast<char*>(string_val.data());

          Logger::log(LogLevel::DEBUG,
                      "[WRITE_STRING] [Phase 1] Size written successfully.");
          Logger::log(LogLevel::DEBUG,
                      "[WRITE_STRING] [Phase 1] New ptr_str_ini assigned to: ",
                      false, true);
          Logger::log(LogLevel::DEBUG, reinterpret_cast<uintptr_t>(ptr_str_ini),
                      true, false);

          // If there are no bytes remaining, we do not let it continue to phase
          // 2:
          if (bytes_remain == 0) {
            Logger::log(LogLevel::DEBUG,
                        "[WRITE_STRING] [Phase 1] Full buffer (bytes_remain == "
                        "0). Exiting.");
            return;
          };
        } else {
          Logger::log(LogLevel::DEBUG,
                      "[WRITE_STRING] [Phase 1] ERROR: Content does not fit "
                      "inside the buffer. Setting new value for the offset.");
          offset = uint32_size;
          return;
        };
      };

      // Phase 2 -> String writing:
      if (ptr_str_ini && bytes_remain > 0) {
        uint32_t bytes_remaining =
            (string_val.data() + string_size) - ptr_str_ini;

        Logger::log(LogLevel::DEBUG,
                    "[WRITE_STRING] [Phase 2] ptr_str_ini valid. Calculating "
                    "remaining bytes.");
        Logger::log(
            LogLevel::DEBUG,
            "[WRITE_STRING] [Phase 2] string_val.data() + string_size = ",
            false, true);
        Logger::log(
            LogLevel::DEBUG,
            reinterpret_cast<uintptr_t>(string_val.data() + string_size), true,
            false);
        Logger::log(LogLevel::DEBUG,
                    "[WRITE_STRING] [Phase 2] Current ptr_str_ini = ", false,
                    true);
        Logger::log(LogLevel::DEBUG, reinterpret_cast<uintptr_t>(ptr_str_ini),
                    true, false);
        Logger::log(LogLevel::DEBUG,
                    "[WRITE_STRING] [Phase 2] bytes_remaining: ", false, true);
        Logger::log(LogLevel::DEBUG, bytes_remaining, true, false);

        uint32_t bytes_to_write = 0;
        if (bytes_remain >= bytes_remaining) {
          bytes_to_write = bytes_remaining;
        } else {
          bytes_to_write = bytes_remain;
        };

        Logger::log(LogLevel::DEBUG,
                    "[WRITE_STRING] [Phase 2] bytes_to_write: ", false, true);
        Logger::log(LogLevel::DEBUG, bytes_to_write, true, false);

        // Only if there are bytes available for writing:
        if (bytes_to_write > 0) {
          std::memcpy(buffer_ptr, ptr_str_ini, bytes_to_write);
          buffer_ptr += bytes_to_write;
          ptr_str_ini += bytes_to_write;
          bytes_remain -= bytes_to_write;
          bytes_written += bytes_to_write;

          Logger::log(LogLevel::DEBUG,
                      "[WRITE_STRING] [Phase 2] Bytes copied to the buffer. "
                      "Updating ptr_str_ini to: ",
                      false, true);
          Logger::log(LogLevel::DEBUG, reinterpret_cast<uintptr_t>(ptr_str_ini),
                      true, false);
        };
      };

      // Phase 3 -> String writing termination:
      Logger::log(LogLevel::DEBUG,
                  "[WRITE_STRING] [Phase 3] Evaluating the character array...");
      Logger::log(LogLevel::DEBUG,
                  "[WRITE_STRING] [Phase 3] ptr_str_ini: ", false, true);
      Logger::log(LogLevel::DEBUG, reinterpret_cast<uintptr_t>(ptr_str_ini),
                  true, false);
      Logger::log(LogLevel::DEBUG,
                  "[WRITE_STRING] [Phase 3] Final limit (data+size): ", false,
                  true);
      Logger::log(LogLevel::DEBUG,
                  reinterpret_cast<uintptr_t>(string_val.data() + string_size),
                  true, false);

      if (ptr_str_ini >= string_val.data() + string_size) {
        // We have finished reading the whole string:
        Logger::log(LogLevel::DEBUG,
                    "^^^^^^^^ ptr_str_ini is now nullptr ^^^^^^^^");
        current_col += 1;
        ptr_str_ini = nullptr;
      } else {
        Logger::log(LogLevel::DEBUG,
                    "^^^^^^^^ ptr_str_ini will NOT be nullptr ^^^^^^^^");
      };
      return;
    };

    case dataType::UNKNOWN: {
      const std::vector<char>& char_vec_val =
          std::get<std::vector<char>>(value);
      uint32_t unk_size;
      unk_size = char_vec_val.size();
      uint8_t uint32_size = sizeof(uint32_t);

      Logger::log(LogLevel::DEBUG,
                  "[WRITE_STRING] >>> Auxiliary function additional entry <<<");
      Logger::log(
          LogLevel::DEBUG,
          std::string("[WRITE_STRING] Text content: ") + char_vec_val.data());
      Logger::log(LogLevel::DEBUG, "[WRITE_STRING] unk_size: ", false, true);
      Logger::log(LogLevel::DEBUG, unk_size, true, false);
      Logger::log(LogLevel::DEBUG,
                  "[WRITE_STRING] bytes_remain in buffer: ", false, true);
      Logger::log(LogLevel::DEBUG, bytes_remain, true, false);
      Logger::log(
          LogLevel::DEBUG,
          "[WRITE_STRING] Base address for char_vec_val.data(): ", false, true);
      Logger::log(LogLevel::DEBUG,
                  reinterpret_cast<uintptr_t>(char_vec_val.data()), true,
                  false);
      Logger::log(LogLevel::DEBUG,
                  "[WRITE_STRING] ptr_str_ini initial value: ", false, true);
      Logger::log(LogLevel::DEBUG, reinterpret_cast<uintptr_t>(ptr_str_ini),
                  true, false);

      // Phase 1 -> Reading the character array's size:
      if (!ptr_str_ini) {
        Logger::log(LogLevel::DEBUG,
                    "[WRITE_STRING] [Phase 1] ptr_str_ini is nullptr. Trying "
                    "to write its size...");
        tmp_char_ptr = reinterpret_cast<char*>(&unk_size);
        if (uint32_size <= bytes_remain) {
          tmp_char_ptr = reinterpret_cast<char*>(&unk_size);
          std::memcpy(buffer_ptr, tmp_char_ptr, uint32_size);
          buffer_ptr += uint32_size;
          bytes_remain -= uint32_size;
          bytes_written += uint32_size;
          ptr_str_ini = const_cast<char*>(char_vec_val.data());

          Logger::log(LogLevel::DEBUG,
                      "[WRITE_STRING] [Phase 1] Size written successfully.");
          Logger::log(LogLevel::DEBUG,
                      "[WRITE_STRING] [Phase 1] New ptr_str_ini assigned to: ",
                      false, true);
          Logger::log(LogLevel::DEBUG, reinterpret_cast<uintptr_t>(ptr_str_ini),
                      true, false);

          // If there are no bytes remaining, we do not let it continue to phase
          // 2:
          if (bytes_remain == 0) {
            Logger::log(LogLevel::DEBUG,
                        "[WRITE_STRING] [Phase 1] Full buffer (bytes_remain == "
                        "0). Exiting.");
            return;
          };
        } else {
          Logger::log(LogLevel::DEBUG,
                      "[WRITE_STRING] [Phase 1] ERROR: Content does not fit "
                      "inside the buffer. Setting new value for the offset.");
          offset = uint32_size;
          return;
        };
      };

      // Phase 2 -> Character array writing:
      if (ptr_str_ini && bytes_remain > 0) {
        uint32_t bytes_remaining =
            (char_vec_val.data() + unk_size) - ptr_str_ini;

        Logger::log(LogLevel::DEBUG,
                    "[WRITE_STRING] [Phase 2] ptr_str_ini valid. Calculating "
                    "remaining bytes.");
        Logger::log(
            LogLevel::DEBUG,
            "[WRITE_STRING] [Phase 2] char_vec_val.data() + unk_size = ", false,
            true);
        Logger::log(LogLevel::DEBUG,
                    reinterpret_cast<uintptr_t>(char_vec_val.data() + unk_size),
                    true, false);
        Logger::log(LogLevel::DEBUG,
                    "[WRITE_STRING] ptr_str_ini actual = ", false, true);
        Logger::log(LogLevel::DEBUG, reinterpret_cast<uintptr_t>(ptr_str_ini),
                    true, false);
        Logger::log(LogLevel::DEBUG,
                    "[WRITE_STRING] [Phase 2] bytes_remaining: ", false, true);
        Logger::log(LogLevel::DEBUG, bytes_remaining, true, false);

        uint32_t bytes_to_write = 0;
        if (bytes_remain >= bytes_remaining) {
          bytes_to_write = bytes_remaining;
        } else {
          bytes_to_write = bytes_remain;
        };

        Logger::log(LogLevel::DEBUG,
                    "[WRITE_STRING] [Phase 2] bytes_to_write: ", false, true);
        Logger::log(LogLevel::DEBUG, bytes_to_write, true, false);

        // Only if there are bytes available for writing:
        if (bytes_to_write > 0) {
          std::memcpy(buffer_ptr, ptr_str_ini, bytes_to_write);
          buffer_ptr += bytes_to_write;
          ptr_str_ini += bytes_to_write;
          bytes_remain -= bytes_to_write;
          bytes_written += bytes_to_write;

          Logger::log(LogLevel::DEBUG,
                      "[WRITE_STRING] [Phase 2] Bytes copied to the buffer. "
                      "Updating ptr_str_ini to: ",
                      false, true);
          Logger::log(LogLevel::DEBUG, reinterpret_cast<uintptr_t>(ptr_str_ini),
                      true, false);
        };
      };

      // Phase 3 -> Character array writing termination:
      Logger::log(LogLevel::DEBUG,
                  "[WRITE_STRING] [Phase 3] Evaluating the character array...");
      Logger::log(LogLevel::DEBUG,
                  "[WRITE_STRING] [Phase 3] ptr_str_ini: ", false, true);
      Logger::log(LogLevel::DEBUG, reinterpret_cast<uintptr_t>(ptr_str_ini),
                  true, false);
      Logger::log(LogLevel::DEBUG,
                  "[WRITE_STRING] [Phase 3] Final limit (data+size): ", false,
                  true);
      Logger::log(LogLevel::DEBUG,
                  reinterpret_cast<uintptr_t>(char_vec_val.data() + unk_size),
                  true, false);

      if (ptr_str_ini >= char_vec_val.data() + unk_size) {
        // We have finished reading the whole character array:
        Logger::log(LogLevel::DEBUG,
                    "^^^^^^^^ ptr_str_ini is now nullptr ^^^^^^^^");
        current_col += 1;
        ptr_str_ini = nullptr;
      } else {
        Logger::log(LogLevel::DEBUG,
                    "^^^^^^^^ ptr_str_ini will NOT be nullptr ^^^^^^^^");
      };
      return;
    };
  };
};

uint32_t disk_aux::return_file_size_bytes(std::ifstream& in) {
  uint32_t size_bytes = 0;

  if (in.is_open()) {
    in.seekg(0, std::ios::end);
    size_bytes = static_cast<uint32_t>(in.tellg());
    in.seekg(0, std::ios::beg);
  };
  return size_bytes;
};

void disk_aux::fill_vector_int(uint32_t elements_num, char* vec_in,
                               std::vector<Values>& vec_out) {
  int* datos_enteros = reinterpret_cast<int*>(vec_in);

  vec_out.clear();

  vec_out.reserve(elements_num);

  for (uint32_t i = 0; i < elements_num; i++) {
    vec_out.push_back(datos_enteros[i]);
  };
};

void disk_aux::fill_vector_float(uint32_t elements_num, char* vec_in,
                                 std::vector<Values>& vec_out) {
  float datos_enteros;
  std::memcpy(&datos_enteros, vec_in, sizeof(float));

  vec_out.clear();

  vec_out.reserve(elements_num);

  for (uint32_t i = 0; i < elements_num; i++) {
    vec_out.push_back(datos_enteros);
  };
};

void disk_aux::fill_vector_bool(uint32_t elements_num, char* vec_in,
                                std::vector<Values>& vec_out) {
  uint8_t* datos_enteros = reinterpret_cast<uint8_t*>(vec_in);

  vec_out.clear();

  vec_out.reserve(elements_num);

  for (uint32_t i = 0; i < elements_num; i++) {
    if (datos_enteros[i] == 1) {
      vec_out.push_back(true);
    } else {
      vec_out.push_back(false);
    };
  };
};

//====================================================
//================= File deletion ====================
//====================================================

void disk_aux::delete_bin_metadata_file(table*& tb) {
  /*
  Given a reference to the table pointer, deletes its on-disk
  metadata if it exists.
  */

  std::string tb_name = tb->metadata_ptr->name;

  std::filesystem::path meta_path = "metadata";
  meta_path /= (tb_name + "_meta.bin");

  if (std::filesystem::exists(meta_path)) {
    // We only delete the metadata file if it exists:
    std::filesystem::remove(meta_path);
  };
};

void disk_aux::delete_bin_data_file(table*& tb) {
  /*
  Given a reference to the table pointer, deletes its on-disk
  data if it exists.
  */

  std::string tb_name = tb->metadata_ptr->name;

  std::filesystem::path data_path = "data";
  data_path /= (tb_name + "tb_name");

  if (std::filesystem::exists(data_path)) {
    // We only delete the data file if it exists:
    std::filesystem::remove_all(data_path);
  };
};

//====================================================
//================= Metadata reading =================
//====================================================

void disk_aux::scan_tables_recursive(
    const std::filesystem::path& path,
    std::vector<std::filesystem::path>& arr_tables) {
  /*
  Auxliary recursive function. Scans only metadata files for table scanning.
  */
  // Iteration for each element:
  for (const auto& entry : std::filesystem::directory_iterator(path)) {
    // If it is a folder:
    if (std::filesystem::is_directory(entry)) {
      disk_aux::scan_tables_recursive(entry.path(), arr_tables);
    } else if (std::filesystem::is_regular_file(entry) &&
               entry.path().extension().string() == ".bin" &&
               entry.path().filename().string().find("_meta") !=
                   std::string::npos) {
      // Base case (only for metadata files):
      arr_tables.push_back(entry);
    };
  };
};

std::vector<std::filesystem::path> disk_aux::scan_tables() {
  /*
  Scans metadata files using a recursive helper function
  to discover all tables present on disk.
  */

  std::filesystem::path path_tables = "metadata";
  // Vector containing all table names:
  std::vector<std::filesystem::path> arr_tables;

  disk_aux::scan_tables_recursive(path_tables, arr_tables);
  return arr_tables;
};
