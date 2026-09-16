#include "disk_in.h"

#include "disk_aux.h"
#include "part_sort.h"

////////////////////////////////////////////////////////////////////
// DATA READING ////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void disk_in::read_fixed_len_int_columns(std::filesystem::path path_var,
                                         std::string& partition_current,
                                         std::vector<Values>& vec_vals) {
  Logger::log(LogLevel::DEBUG, "Inside INT reading");
  std::ifstream in;
  std::filesystem::path path = path_var;
  path /= partition_current;
  path += ".dat";
  Logger::log(LogLevel::DEBUG, "Opening file data: ", false, true);
  Logger::log(LogLevel::DEBUG, path, true, false);
  in.open(path, std::ios::in | std::ios::binary);
  if (!in.is_open()) {
    Logger::log(LogLevel::ERROR, "Could not open file! Path: ", false, true);
    Logger::log(LogLevel::ERROR, path, true, false);
    return;
  } else {
    Logger::log(LogLevel::DEBUG, "File opened for reading");
  };

  // Quering partition file size:
  uint32_t file_size = disk_aux::return_file_size_bytes(in);
  uint32_t bytes_pointer = 0;
  uint32_t bytes_to_read = 0;

  // Reserving bytes within the buffer:
  char* buffer = nullptr;
  try {
    buffer = new char[size_buffer_bytes];

    Logger::log(LogLevel::DEBUG, "File size retrieved successfully");
    Logger::log(LogLevel::DEBUG, "File size:", false, true);
    Logger::log(LogLevel::DEBUG, file_size, true, false);

    if (file_size > 0) {
      Logger::log(LogLevel::DEBUG, "File is NOT empty");
      while (bytes_pointer < file_size) {
        Logger::log(LogLevel::DEBUG, "Buffer loop iteration");
        Logger::log(LogLevel::DEBUG, "Calculating bytes to read");
        // Calculating bytes to read first:
        if (bytes_pointer + size_buffer_bytes >= file_size) {
          bytes_to_read = size_buffer_bytes;
        } else {
          bytes_to_read = file_size - bytes_pointer;
        };
        Logger::log(LogLevel::DEBUG, "Reserving those bytes within the buffer");
        // Bytes to read is already known, proceeding to read them from disk:
        Logger::log(LogLevel::DEBUG, "Loading data on to buffer");
        in.read(buffer, bytes_to_read);
        Logger::log(LogLevel::DEBUG, "Buffer loades successfully");
        Logger::log(LogLevel::DEBUG, "Proceeding to fill table:");
        // Inserting bytes into the vector:
        disk_aux::fill_vector_int(bytes_to_read / sizeof(int), buffer,
                                  vec_vals);
        Logger::log(LogLevel::DEBUG, "Values added to the table successfully");
        // Updating read bytes pointer:
        bytes_pointer += bytes_to_read;
      };
    } else {
      Logger::log(LogLevel::DEBUG, "File IS empty");
    };
  } catch (...) {
    delete[] buffer;
    in.close();
    throw;
  };
  in.close();
  delete[] buffer;
};

void disk_in::read_fixed_len_float_columns(std::filesystem::path path_var,
                                           std::string& partition_current,
                                           std::vector<Values>& vec_vals) {
  Logger::log(LogLevel::DEBUG, "Inside FLOAT reading");
  std::ifstream in;
  std::filesystem::path path = path_var;
  path /= partition_current;
  path += ".dat";
  Logger::log(LogLevel::DEBUG, "Opening file data: ", false, true);
  Logger::log(LogLevel::DEBUG, path, true, false);
  in.open(path, std::ios::in | std::ios::binary);
  if (!in.is_open()) {
    Logger::log(LogLevel::ERROR, "Could not open file! Path: ", false, true);
    Logger::log(LogLevel::ERROR, path, true, false);
    return;
  } else {
    Logger::log(LogLevel::DEBUG, "File opened for reading");
  };

  // Quering partition file size:
  uint32_t file_size = disk_aux::return_file_size_bytes(in);
  uint32_t bytes_pointer = 0;
  uint32_t bytes_to_read = 0;

  // Reserving bytes within the buffer:
  char* buffer = nullptr;
  try {
    buffer = new char[size_buffer_bytes];
    Logger::log(LogLevel::DEBUG, "File size retrieved successfully");
    Logger::log(LogLevel::DEBUG, "File size:", false, true);
    Logger::log(LogLevel::DEBUG, file_size, true, false);

    if (file_size > 0) {
      Logger::log(LogLevel::DEBUG, "File is NOT empty");
      while (bytes_pointer < file_size) {
        Logger::log(LogLevel::DEBUG, "Buffer loop iteration");
        Logger::log(LogLevel::DEBUG, "Calculating bytes to read");
        // Calculating bytes to read first:
        if (bytes_pointer + size_buffer_bytes >= file_size) {
          bytes_to_read = size_buffer_bytes;
        } else {
          bytes_to_read = file_size - bytes_pointer;
        };
        Logger::log(LogLevel::DEBUG, "Reserving those bytes within the buffer");
        // Bytes to read is already known, proceeding to read them from disk:
        Logger::log(LogLevel::DEBUG, "Loading data into buffer");
        in.read(buffer, bytes_to_read);
        Logger::log(LogLevel::DEBUG, "Buffer loades successfully");
        Logger::log(LogLevel::DEBUG, "Proceeding to fill table:");
        // Inserting bytes into the vector:
        disk_aux::fill_vector_float(bytes_to_read / sizeof(float), buffer,
                                    vec_vals);
        Logger::log(LogLevel::DEBUG, "Values added to the table successfully");
        // Updating read bytes pointer:
        bytes_pointer += bytes_to_read;
      };
    } else {
      Logger::log(LogLevel::DEBUG, "File IS empty");
    };
  } catch (...) {
    delete[] buffer;
    in.close();
    throw;
  };
  in.close();
  delete[] buffer;
};

void disk_in::read_fixed_len_bool_columns(std::filesystem::path path_var,
                                          std::string& partition_current,
                                          std::vector<Values>& vec_vals) {
  Logger::log(LogLevel::DEBUG, "Inside BOOL reading");
  std::ifstream in;
  std::filesystem::path path = path_var;
  path /= partition_current;
  path += ".dat";
  Logger::log(LogLevel::DEBUG, "Opening file data: ", false, true);
  Logger::log(LogLevel::DEBUG, path, true, false);
  in.open(path, std::ios::in | std::ios::binary);
  if (!in.is_open()) {
    Logger::log(LogLevel::ERROR, "Could not open file! Path: ", false, true);
    Logger::log(LogLevel::ERROR, path, true, false);
    return;
  } else {
    Logger::log(LogLevel::DEBUG, "File opened for reading");
  };

  // Quering partition file size:
  uint32_t file_size = disk_aux::return_file_size_bytes(in);
  uint32_t bytes_pointer = 0;
  uint32_t bytes_to_read = 0;

  // Reserving bytes within the buffer:
  char* buffer = nullptr;
  try {
    buffer = new char[size_buffer_bytes];
    Logger::log(LogLevel::DEBUG, "File size retrieved successfully");
    Logger::log(LogLevel::DEBUG, "File size:", false, true);
    Logger::log(LogLevel::DEBUG, file_size, true, false);

    if (file_size > 0) {
      Logger::log(LogLevel::DEBUG, "File is NOT empty");
      while (bytes_pointer < file_size) {
        Logger::log(LogLevel::DEBUG, "Buffer loop iteration");
        Logger::log(LogLevel::DEBUG, "Calculating bytes to read");
        // Calculating bytes to read first:
        if (bytes_pointer + size_buffer_bytes >= file_size) {
          bytes_to_read = size_buffer_bytes;
        } else {
          bytes_to_read = file_size - bytes_pointer;
        };
        Logger::log(LogLevel::DEBUG, "Reserving those bytes within the buffer");
        // Bytes to read is already known, proceeding to read them from disk:
        Logger::log(LogLevel::DEBUG, "Loading data into buffer");
        in.read(buffer, bytes_to_read);
        Logger::log(LogLevel::DEBUG, "Buffer loades successfully");
        Logger::log(LogLevel::DEBUG, "Proceeding to fill table:");
        // Inserting bytes into the vector:
        disk_aux::fill_vector_bool(bytes_to_read / sizeof(uint8_t), buffer,
                                   vec_vals);
        Logger::log(LogLevel::DEBUG, "Values added to the table successfully");
        // Updating read bytes pointer:
        bytes_pointer += bytes_to_read;
      };
    } else {
      Logger::log(LogLevel::DEBUG, "File IS empty");
    };
  } catch (...) {
    delete[] buffer;
    in.close();
    throw;
  };
  in.close();
  delete[] buffer;
};

///////////////////////////////////////////////////////////////////////////////////////////
// Para la lectura de las strings se realiza con un FSM

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

class stringReader_v2 {
  /*
  This class will allow total abstraction from string reading,
  Easing string or character vector reading from disk.
  */
  std::vector<Values>& vec_vals;

 public:
  // Variable auxiliar para ver si es una std::string o un vector de caracteres:
  // Auxiliary variable. Fo9r knowing if input is a string or character vector.
  bool is_unknown_type = false;
  // input and control variables:
  std::filesystem::path path_var;
  std::string partition_current;
  uint8_t state = 0;

  // File counters:
  uint32_t file_idx_ptr = 0;
  uint32_t file_buffer_idx_size = 0;
  uint32_t file_idx_size = 0;

  uint32_t file_str_ptr = 0;
  uint32_t file_buffer_str_size = 0;
  uint32_t file_str_size = 0;

  // Buffer counters
  char* buffer_idx_ptr_ini = nullptr;
  char* buffer_idx_ptr_fin = nullptr;
  uint32_t buffer_idx_bytes_to_read_each_time =
      4;  // Always reading 4 bytes at a time
  uint32_t buffer_idx_read_bytes_tmp = 0;
  uint32_t buffer_idx_read_bytes_total = 0;

  char* buffer_str_ptr_ini = nullptr;
  char* buffer_str_ptr_fin = nullptr;
  uint32_t buffer_str_read_bytes_tmp =
      0;  // Whichever is smaller: the string length or the remaining buffer
          // capacity.
  uint32_t buffer_str_read_bytes_total = 0;

  // String reading counters:
  uint32_t size_string_tmp = 0;
  bool offset = false;

  // Reading files inputs:
  std::ifstream in_str;
  std::ifstream in_idx;

  char* buffer_str = nullptr;
  char* buffer_idx = nullptr;

  uint32_t bytes_to_read = 0;

  ;

  stringReader_v2(std::vector<Values>& vector_variable_variantes,
                  std::filesystem::path path_variable,
                  std::string& particion_actual, bool is_var_unknown)
      : vec_vals(vector_variable_variantes),
        path_var(path_variable),
        partition_current(particion_actual),
        is_unknown_type(is_var_unknown)  // Inicialización de la referencia y
                                         // strings para las rutas
  {
    this->buffer_str = new char[size_buffer_bytes];
    this->buffer_idx = new char[size_buffer_bytes];
  };

  ~stringReader_v2() {
    if (this->in_str.is_open()) {
      this->in_str.close();
    };
    if (this->in_idx.is_open()) {
      this->in_idx.close();
    };

    if (this->buffer_str) {
      delete[] this->buffer_str;
      this->buffer_str = nullptr;
    };
    if (this->buffer_idx) {
      delete[] this->buffer_idx;
      this->buffer_idx = nullptr;
    };
  };

  // DISABLE COPY AND ASSSIGNMENT OF HEAP VARIABLES:
  stringReader_v2(const stringReader_v2&) = delete;  // Copy disablement
  stringReader_v2& operator=(const stringReader_v2&) =
      delete;  // Assigment operation disablement

  bool condition_eof_str_partition() {
    return this->file_str_ptr >= this->file_str_size;
  };

  bool condition_eof_idx_partition() {
    return this->file_idx_ptr >= this->file_idx_size;
  };

  bool condition_eof_buffer_str() {
    return this->buffer_str_read_bytes_tmp >= this->file_buffer_str_size;
  };

  bool condition_eof_buffer_idx() {
    return this->buffer_idx_read_bytes_tmp >= this->file_buffer_idx_size;
  };

  // Auxiliary functions fro calculating buffer size:
  void calculate_buffer_size_str() {
    this->file_buffer_str_size = size_buffer_bytes;
    if (this->file_str_ptr + size_buffer_bytes > this->file_str_size) {
      this->file_buffer_str_size = this->file_str_size - this->file_str_ptr;
    };
  };

  void calculate_buffer_size_idx() {
    this->file_buffer_idx_size = size_buffer_bytes;
    if (this->file_idx_ptr + size_buffer_bytes > this->file_idx_size) {
      this->file_buffer_idx_size = this->file_idx_size - this->file_idx_ptr;
    };
  };

  // Auxiliary functions for reading:
  void read_str_buffer() {
    // Claculating buffer size:
    this->calculate_buffer_size_str();

    // Reading strings:
    this->in_str.read(this->buffer_str, this->file_buffer_str_size);

    // Updating reading pointer:
    this->file_str_ptr += this->file_buffer_str_size;

    // Restarting buffer reading counter:
    this->buffer_str_read_bytes_tmp = 0;

    // Setting buffer pointers:
    this->buffer_str_ptr_ini = this->buffer_str;
    this->buffer_str_ptr_fin = buffer_str_ptr_ini + this->file_buffer_str_size;
  };

  void read_idx_buffer() {
    // Claculating buffer size:
    this->calculate_buffer_size_idx();

    // Reading string indices:
    this->in_idx.read(this->buffer_idx, this->file_buffer_idx_size);

    // Updating reading pointer:
    this->file_idx_ptr += this->file_buffer_idx_size;

    // Restarting buffer reading counter:
    this->buffer_idx_read_bytes_tmp = 0;

    // Setting buffer pointers:
    this->buffer_idx_ptr_ini = this->buffer_idx;
    this->buffer_idx_ptr_fin = buffer_idx_ptr_ini + this->file_buffer_idx_size;
  };

  void control_unit() {
    /*
    Control unit for reading and loading table data
    */

    switch (this->state) {
      case 0: {
        Logger::log(LogLevel::DEBUG, "State 0 stringReader_v2");
        std::filesystem::path path_str = this->path_var;
        path_str /= this->partition_current;
        path_str += ".bin";
        std::filesystem::path path_idx = this->path_var;
        path_idx /= this->partition_current;
        path_idx += ".idx";

        Logger::log(LogLevel::DEBUG, "Data path: ", false, true);
        Logger::log(LogLevel::DEBUG, path_str, true, false);
        Logger::log(LogLevel::DEBUG, "Index path: ", false, true);
        Logger::log(LogLevel::DEBUG, path_idx, true, false);

        this->in_str.open(path_str, std::ios::in | std::ios::binary);
        this->in_idx.open(path_idx, std::ios::in | std::ios::binary);

        Logger::log(LogLevel::DEBUG, "Files read successfully");

        // Retrieving file sizes:
        this->file_str_size = disk_aux::return_file_size_bytes(in_str);
        this->file_idx_size = disk_aux::return_file_size_bytes(in_idx);
        Logger::log(LogLevel::DEBUG, "  Strings file's size: ", false, true);
        Logger::log(LogLevel::DEBUG, this->file_str_size, true, false);
        Logger::log(LogLevel::DEBUG, "  Indices file's size: ", false, true);
        Logger::log(LogLevel::DEBUG, this->file_idx_size, true, false);

        if (this->file_str_size > 0 && this->file_idx_size > 0) {
          this->state = 1;
          break;
        };
        this->state = 255;
        break;
      };

      case 1: {
        Logger::log(LogLevel::DEBUG, "State 1 stringReader_v2");
        // Reading string buffer:
        this->read_str_buffer();
        // Leemos el buffer de idx:
        // Reading indices buffer:
        this->read_idx_buffer();
        this->state = 4;  // Skipping to quering string size
        break;
      };

      case 2: {
        Logger::log(LogLevel::DEBUG, "State 2 stringReader_v2");
        bool eof_buffer_str = false;
        bool eof_buffer_idx = false;
        bool eof_file_str = false;
        bool eof_file_idx = false;
        Logger::log(
            LogLevel::DEBUG,
            "<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
        Logger::log(LogLevel::DEBUG, "STOP DIAGNOSIS:");

        Logger::flush(LogLevel::DEBUG);
        Logger::log(LogLevel::DEBUG, "-> condition_eof_str_partition:");
        Logger::log(LogLevel::DEBUG, "  data pointer value: ", false, true);
        Logger::log(LogLevel::DEBUG, this->file_str_ptr, true, false);
        Logger::log(LogLevel::DEBUG, "  Data file size: ", false, true);
        Logger::log(LogLevel::DEBUG, this->file_str_size, true, false);
        Logger::flush(LogLevel::DEBUG);
        Logger::log(LogLevel::DEBUG, "-> condition_eof_idx_partition:");
        Logger::log(LogLevel::DEBUG, "  indices pointer value: ", false, true);
        Logger::log(LogLevel::DEBUG, this->file_idx_ptr, true, false);
        Logger::log(LogLevel::DEBUG, "  Indices file size: ", false, true);
        Logger::log(LogLevel::DEBUG, this->file_idx_size, true, false);
        Logger::flush(LogLevel::DEBUG);
        Logger::log(LogLevel::DEBUG, "-> condition_eof_buffer_idx:");
        Logger::log(LogLevel::DEBUG, "  Bytes read temporay: ", false, true);
        Logger::log(LogLevel::DEBUG, this->buffer_idx_read_bytes_tmp, true,
                    false);
        Logger::log(LogLevel::DEBUG, "  Buffer size: ", false, true);
        Logger::log(LogLevel::DEBUG, this->file_buffer_idx_size, true, false);
        Logger::flush(LogLevel::DEBUG);
        Logger::log(LogLevel::DEBUG, "-> condition_eof_buffer_str:");
        Logger::log(LogLevel::DEBUG, "  Bytes read temporay: ", false, true);
        Logger::log(LogLevel::DEBUG, this->buffer_str_read_bytes_tmp, true,
                    false);
        Logger::log(LogLevel::DEBUG, "  Buffer size: ", false, true);
        Logger::log(LogLevel::DEBUG, this->file_buffer_str_size, true, false);
        Logger::flush(LogLevel::DEBUG);
        Logger::log(
            LogLevel::DEBUG,
            "<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");

        // indices reading:
        if (this->condition_eof_idx_partition()) {
          eof_file_idx = true;
          if (this->condition_eof_buffer_idx()) {
            eof_buffer_idx = true;
          };
        } else {
          // Only if buffer is depleted we do read it:
          if (this->condition_eof_buffer_idx()) {
            eof_buffer_idx = true;
            this->read_idx_buffer();
          };
        };

        // Content (strings or character vectors) reading:
        if (this->condition_eof_str_partition()) {
          eof_file_str = true;
          if (this->condition_eof_buffer_str()) {
            eof_buffer_str = true;
          };
        } else {
          // Only if buffer is depleted we do read it:
          if (this->condition_eof_buffer_str()) {
            eof_buffer_str = true;
            this->read_str_buffer();
          };
        };

        // Depending on the following conditionals, the next state will be set:
        if (eof_file_idx && eof_buffer_idx && eof_file_str && eof_buffer_str) {
          this->state = 255;
          break;
        };
        // If the end has not yet been reached, we can keep reading:
        this->state = 3;  // Offset checker
        break;
      };
      case 3: {
        Logger::log(LogLevel::DEBUG, "State 3 stringReader_v2");
        if (this->offset) {
          // We are in the middle of a string reading
          this->state = 5;
          break;
        } else {
          // New string to read
          this->state = 4;
          break;
        };
      };
      case 4: {
        Logger::log(LogLevel::DEBUG, "Sate 4 stringReader_v2");

        // Reading string size:
        std::memcpy(&this->size_string_tmp,
                    this->buffer_idx_ptr_ini + this->buffer_idx_read_bytes_tmp,
                    this->buffer_idx_bytes_to_read_each_time);
        // Advanzing pointer to the next row:
        this->buffer_idx_read_bytes_total +=
            this->buffer_idx_bytes_to_read_each_time;
        this->buffer_idx_read_bytes_tmp +=
            this->buffer_idx_bytes_to_read_each_time;
        this->state = 5;  // Skipping to string reading
        break;
      };

      case 5: {
        Logger::log(LogLevel::DEBUG, "State 5 stringReader_v2");

        uint32_t bytes_ram_available =
            this->file_buffer_str_size - this->buffer_str_read_bytes_tmp;
        this->bytes_to_read = this->size_string_tmp;
        if (this->bytes_to_read > bytes_ram_available) {
          this->bytes_to_read = bytes_ram_available;
        };

        const char* origin_bytes =
            this->buffer_str_ptr_ini + this->buffer_str_read_bytes_tmp;
        if (offset) {
          if (is_unknown_type) {
            std::vector<char>& tmp_vec =
                std::get<std::vector<char>>(vec_vals.back());
            tmp_vec.insert(tmp_vec.end(), origin_bytes,
                           origin_bytes + this->bytes_to_read);
          } else {
            // Is not a new string
            std::string& tmp_str = std::get<std::string>(vec_vals.back());
            tmp_str.append(origin_bytes, this->bytes_to_read);
          };
        } else {
          if (is_unknown_type) {
            std::vector<char> tmp_vec(origin_bytes,
                                      origin_bytes + this->bytes_to_read);
            vec_vals.push_back(std::move(tmp_vec));
          } else {
            // String nueva
            std::string tmp_str;
            tmp_str.append(origin_bytes, this->bytes_to_read);
            vec_vals.push_back(std::move(tmp_str));
          };
        };

        // Updating read bytes:
        this->buffer_str_read_bytes_tmp += this->bytes_to_read;

        this->buffer_str_read_bytes_total += this->bytes_to_read;

        // Updating current string bytes left to read:
        this->size_string_tmp -= this->bytes_to_read;

        // Asserting offset:
        if (this->size_string_tmp > 0) {
          // String/character vector has not been fully read:
          this->offset = true;
        } else {
          // String/character vector has been fully read:
          this->offset = false;
        };

        // Next stage will always be string/character vector reading:
        this->state = 2;
        break;
      };
      case 255: {
        Logger::log(LogLevel::DEBUG, "State 255 stringReader_v2");
        break;
      };
        ///////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////

    };  // Switch for control unit ends
  };    // CU for FSM definition ends

  void execute_fsm() {
    Logger::log(LogLevel::DEBUG, "Inside STRING reading");
    bool aux_bool = true;
    while (this->state != 255 && aux_bool) {
      // if (this->state == 255) {
      // aux_bool = false;
      //};
      this->control_unit();
    };
  };
};

disk_in::read_table_iterator::read_table_iterator(table* table_ptr_input) {
  /*
  Constructor method for:
     * Initializing table pointers.
     * Quering total partitions number.

  */
  Logger::log(LogLevel::DEBUG, "Initializing data reading");

  this->table_ptr = table_ptr_input;
  // Retrieving table's name:
  this->table_name = this->table_ptr->metadata_ptr->name;
  // Retrieving coloumns' names:
  this->column_names = this->table_ptr->metadata_ptr->column_names;
  this->num_cols = this->table_ptr->metadata_ptr->n_cols;
  // Retrieving coloumns' types:
  this->column_types = this->table_ptr->metadata_ptr->column_types;
  // Initializing data within session object, if it has not yet been defined:
  if (this->table_ptr->data_buffer_ptr == nullptr) {
    this->table_ptr->data_buffer_ptr = new table_data_buffer();
  };
  // Associanting internal data pointer with the table's data pointer:
  this->disk_data_ptr = this->table_ptr->data_buffer_ptr;
  Logger::log(LogLevel::DEBUG,
              "Retrieving table_ptr values and initializing it's attribute for "
              "data originated from the disk");

  // Now quering total partitions number:
  this->partition_counter = 0;
  this->total_partitions = 0;
  Logger::log(LogLevel::DEBUG,
              "Obtaining partitions present within the data path:");
  Logger::log(LogLevel::DEBUG,
              "data/" + this->table_name + "/" + this->column_names[0]);
  this->partition_names = disk_aux::obtain_files_in_path(
      "data/" + this->table_name + "/" + this->column_names[0],
      this->column_types[0]);
  Logger::log(LogLevel::DEBUG, "Partition names retrieved successfully");

  if (!(this->partition_names.empty())) {
    Logger::log(LogLevel::DEBUG, "There is data within the disk");
    Logger::log(LogLevel::DEBUG,
                "Proceeding to parttion sorting in ascending order:");
    part_sort::quick_sort(this->partition_names);
    Logger::log(LogLevel::DEBUG, "Partition sorting carried successfully");
    Logger::log(LogLevel::DEBUG, "Quering number of total partitions:");
    this->total_partitions = this->partition_names.size();
    Logger::log(LogLevel::DEBUG, "Number of partitions found: ", false, true);
    Logger::log(LogLevel::DEBUG, this->total_partitions, true, false);
  };
  Logger::log(LogLevel::DEBUG,
              "Everything ready. Proceeding to enter column loop:");
};

bool disk_in::read_table_iterator::obtain_int_partition_rows(
    bool aux_bool, std::filesystem::path path_var,
    std::string& partition_current) {
  if (aux_bool) {
    Logger::log(LogLevel::DEBUG,
                "<<<<< PARTITION SIZE ASSERTION (INT CASE) >>>>>>>");
    uint32_t size_tmp = 0;
    Logger::log(LogLevel::DEBUG, "First execution. Quering partition size: ");
    std::filesystem::path tmp_path = path_var;
    tmp_path /= (partition_current + ".dat");
    Logger::log(LogLevel::DEBUG, tmp_path);
    size_tmp = disk_aux::obtain_file_size(tmp_path);
    Logger::log(LogLevel::DEBUG, "Partition file size: ", false, true);
    Logger::log(LogLevel::DEBUG, size_tmp, true, false);
    Logger::log(LogLevel::DEBUG, "Registering partition file size");
    this->current_partition_n_rows = size_tmp / sizeof(int);
    Logger::log(LogLevel::DEBUG,
                "Current partition rows registered successfully: ", false,
                true);
    Logger::log(LogLevel::DEBUG, this->current_partition_n_rows, true, false);
    aux_bool = false;
  };
  return aux_bool;
};

bool disk_in::read_table_iterator::obtain_float_partition_rows(
    bool aux_bool, std::filesystem::path path_var,
    std::string& partition_current) {
  if (aux_bool) {
    Logger::log(LogLevel::DEBUG,
                "<<<<< PARTITION SIZE ASSERTION (FLOAT CASE) >>>>>>>");
    uint32_t size_tmp = 0;
    Logger::log(LogLevel::DEBUG, "First execution. Quering partition size: ");
    std::filesystem::path tmp_path = path_var;
    tmp_path /= (partition_current + ".dat");
    Logger::log(LogLevel::DEBUG, tmp_path);
    size_tmp = disk_aux::obtain_file_size(tmp_path);
    Logger::log(LogLevel::DEBUG, "Partition file size: ", false, true);
    Logger::log(LogLevel::DEBUG, size_tmp, true, false);
    Logger::log(LogLevel::DEBUG, "Registering partition file size");
    this->current_partition_n_rows = size_tmp / sizeof(float);
    Logger::log(LogLevel::DEBUG,
                "Current partition rows registered successfully: ", false,
                true);
    Logger::log(LogLevel::DEBUG, this->current_partition_n_rows, true, false);
    aux_bool = false;
  };
  return aux_bool;
};

bool disk_in::read_table_iterator::obtain_bool_partition_rows(
    bool aux_bool, std::filesystem::path path_var,
    std::string& partition_current) {
  if (aux_bool) {
    uint32_t size_tmp = 0;
    Logger::log(LogLevel::DEBUG,
                "<<<<< PARTITION SIZE ASSERTION (BOOL CASE) >>>>>>>");
    Logger::log(LogLevel::DEBUG, "First execution. Quering partition size: ");
    std::filesystem::path tmp_path = path_var;
    tmp_path /= (partition_current + ".dat");
    Logger::log(LogLevel::DEBUG, tmp_path);
    size_tmp = disk_aux::obtain_file_size(tmp_path);
    Logger::log(LogLevel::DEBUG, "Partition file size: ", false, true);
    Logger::log(LogLevel::DEBUG, size_tmp, true, false);
    Logger::log(LogLevel::DEBUG, "Registering partition file size");
    this->current_partition_n_rows = size_tmp;
    Logger::log(LogLevel::DEBUG,
                "Current partition rows registered successfully: ", false,
                true);
    Logger::log(LogLevel::DEBUG, this->current_partition_n_rows, true, false);
    aux_bool = false;
  };
  return aux_bool;
};

bool disk_in::read_table_iterator::obtain_string_partition_rows(
    bool aux_bool, std::filesystem::path path_var,
    std::string& partition_current) {
  if (aux_bool) {
    uint32_t size_tmp = 0;
    Logger::log(LogLevel::DEBUG,
                "<<<<< PARTITION SIZE ASSERTION (STRING CASE) >>>>>>>");
    Logger::log(LogLevel::DEBUG, "First execution. Quering partition size: ");
    std::filesystem::path tmp_path = path_var;
    tmp_path /= (partition_current + ".idx");
    Logger::log(LogLevel::DEBUG, tmp_path);
    size_tmp = disk_aux::obtain_file_size(tmp_path);
    Logger::log(LogLevel::DEBUG, "Partition file size: ", false, true);
    Logger::log(LogLevel::DEBUG, size_tmp, true, false);
    Logger::log(LogLevel::DEBUG, "Registering partition file size");
    this->current_partition_n_rows = size_tmp / sizeof(uint32_t);
    Logger::log(LogLevel::DEBUG,
                "Current partition rows registered successfully: ", false,
                true);
    Logger::log(LogLevel::DEBUG, this->current_partition_n_rows, true, false);
    aux_bool = false;
  };
  return aux_bool;
};

bool disk_in::read_table_iterator::read_table() {
  Logger::flush(LogLevel::DEBUG);
  Logger::log(LogLevel::DEBUG, "inside 'read_table'");

  if (this->partition_counter < this->total_partitions) {
    Logger::log(LogLevel::DEBUG, "Read partitions: ", false, true);
    Logger::log(LogLevel::DEBUG, this->partition_counter, true, false);
    Logger::log(LogLevel::DEBUG, "Total partitions: ", false, true);
    Logger::log(LogLevel::DEBUG, this->total_partitions, true, false);
    // En este caso preparamos dónde se almacenarán los datos:
    Logger::log(LogLevel::DEBUG, "Accessing disk's std::map:");
    std::map<std::string, std::vector<Values>>& buffer_ram_disk =
        this->disk_data_ptr->columns;
    Logger::log(LogLevel::DEBUG, "disk's std::map retrieved successfully");
    Logger::log(LogLevel::DEBUG, "Proceeding to clean disk's std::map:");
    buffer_ram_disk.clear();
    Logger::log(LogLevel::DEBUG, "disk's std::map cleaned successfully");
    bool aux_bool = true;

    // Can be performed outside the reading loop:
    std::string partition_current =
        this->partition_names[this->partition_counter];
    // Erasing extension flag from partition name:
    partition_current = part_sort::erase_file_extension(partition_current);
    Logger::log(LogLevel::DEBUG, partition_current, true, false);

    for (int i = 0; i < this->num_cols; i++) {
      dataType data_type = column_types[i];
      std::string column_name = this->column_names[i];
      Logger::log(LogLevel::DEBUG, "Processing the column: ", false, true);
      Logger::log(LogLevel::DEBUG, column_name, true, false);
      Logger::log(LogLevel::DEBUG, "Obtaining path: ", false, true);
      std::filesystem::path path_var =
          std::filesystem::path("data") / this->table_name / column_name;
      Logger::log(LogLevel::DEBUG, path_var, true, false);
      Logger::log(LogLevel::DEBUG, "Accessing current partition: ", false,
                  true);
      std::vector<Values> vec_vals;

      switch (data_type) {
        case dataType::INT: {
          Logger::log(LogLevel::DEBUG, "INT Case");
          Logger::log(LogLevel::DEBUG, "++++++ SWITCH INT CASE ++++++");
          aux_bool = this->obtain_int_partition_rows(aux_bool, path_var,
                                                     partition_current);
          Logger::log(LogLevel::DEBUG, "Proceeding current INT partition:");
          disk_in::read_fixed_len_int_columns(path_var, partition_current,
                                              vec_vals);
          // Adding data to the table (section for data defined within current
          // program session/execution):
          Logger::log(LogLevel::DEBUG,
                      "Proceeding to add INT vector to the table");
          buffer_ram_disk[column_name] = std::move(vec_vals);
          Logger::log(LogLevel::DEBUG,
                      "Vector added to the table successfully");
          break;
        };
        case dataType::FLOAT: {
          Logger::log(LogLevel::DEBUG, "++++++ SWITCH FLOAT CASE ++++++");
          aux_bool = this->obtain_float_partition_rows(aux_bool, path_var,
                                                       partition_current);
          Logger::log(LogLevel::DEBUG, "FLOAT case");
          Logger::log(LogLevel::DEBUG,
                      "Proceeding to add FLOAT vector to the table");
          disk_in::read_fixed_len_float_columns(path_var, partition_current,
                                                vec_vals);
          // Adding data to the table (section for data defined within current
          // program session/execution):
          buffer_ram_disk[column_name] = std::move(vec_vals);
          Logger::log(LogLevel::DEBUG,
                      "Vector added to the table successfully");
          break;
        };
        case dataType::BOOL: {
          Logger::log(LogLevel::DEBUG, "++++++ SWITCH BOOL CASE ++++++");
          aux_bool = this->obtain_bool_partition_rows(aux_bool, path_var,
                                                      partition_current);
          Logger::log(LogLevel::DEBUG,
                      "Proceeding to add BOOL vector to the table");
          disk_in::read_fixed_len_bool_columns(path_var, partition_current,
                                               vec_vals);
          // Adding data to the table (section for data defined within current
          // program session/execution):
          buffer_ram_disk[column_name] = std::move(vec_vals);
          Logger::log(LogLevel::DEBUG,
                      "Vector added to the table successfully");
          break;
        };
        case dataType::STRING: {
          Logger::log(LogLevel::DEBUG, "++++++ SWITCH STRING CASE ++++++");
          aux_bool = this->obtain_string_partition_rows(aux_bool, path_var,
                                                        partition_current);
          Logger::log(LogLevel::DEBUG,
                      "Proceeding to add STRING vector to the table");

          // Creating string/character array object:
          Logger::flush(LogLevel::DEBUG);
          Logger::flush(LogLevel::DEBUG);
          Logger::flush(LogLevel::DEBUG);
          Logger::log(LogLevel::DEBUG,
                      "||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
                      "||||||||||||");
          stringReader_v2 string_reader =
              stringReader_v2(vec_vals, path_var, partition_current, false);
          // Executing FSM partition reading:
          string_reader.execute_fsm();

          Logger::flush(LogLevel::DEBUG);
          Logger::flush(LogLevel::DEBUG);
          Logger::flush(LogLevel::DEBUG);
          Logger::log(LogLevel::DEBUG,
                      "||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
                      "||||||||||||");
          // Adding data to the table (section for data defined within current
          // program session/execution):
          buffer_ram_disk[column_name] = std::move(vec_vals);
          Logger::log(LogLevel::DEBUG,
                      "Vector added to the table successfully");
          break;
        };
        case dataType::UNKNOWN: {
          Logger::log(LogLevel::DEBUG, "++++++ SWITCH UNKNOWN CASE ++++++");
          aux_bool = this->obtain_string_partition_rows(aux_bool, path_var,
                                                        partition_current);
          Logger::log(LogLevel::DEBUG,
                      "Proceeding to add UNKNOWN vector to the table");

          // Creating string/character array object:
          Logger::flush(LogLevel::DEBUG);
          Logger::flush(LogLevel::DEBUG);
          Logger::flush(LogLevel::DEBUG);
          Logger::log(LogLevel::DEBUG,
                      "||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
                      "||||||||||||");
          stringReader_v2 string_reader =
              stringReader_v2(vec_vals, path_var, partition_current, true);
          // Executing FSM partition reading:
          string_reader.execute_fsm();

          Logger::flush(LogLevel::DEBUG);
          Logger::flush(LogLevel::DEBUG);
          Logger::flush(LogLevel::DEBUG);
          Logger::log(LogLevel::DEBUG,
                      "||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
                      "||||||||||||");
          // Adding data to the table (section for data defined within current
          // program session/execution):
          buffer_ram_disk[column_name] = std::move(vec_vals);
          Logger::log(LogLevel::DEBUG,
                      "Vector added to the table successfully");
          break;
        };
      };
    };
    Logger::log(LogLevel::DEBUG, "Adding one parttition !!!!!!!");
    this->partition_counter += 1;
    return false;
  };
  Logger::log(LogLevel::DEBUG, "There are no partitions left to read");
  // Erasing table's data section assciated with disk retrievals, as there are
  // no more partitions left to read:
  std::map<std::string, std::vector<Values>>& buffer_ram_disk =
      this->disk_data_ptr->columns;
  buffer_ram_disk.clear();
  return true;  // Reading is finished
};
