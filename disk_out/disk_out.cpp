#include "part_sort.h"
#include "disk_aux.h"
#include "disk_out.h"


////////////////////////////////////////////////////////////////////
// ESCRITURA DE DATOS //////////////////////////////////////////////
////////////////////////////////////////////////////////////////////



void disk_out::fix_vect_vals(std::vector<Values>::iterator start_iterator, std::vector<Values>::iterator end_iterator, int* arr_vals){
   /* CASO para INTs:

   start_iterator y end_iterator son sólo del buffer, no los totales
   de todos los datos ni los d la partición
   */

   int index = 0;

   while(start_iterator < end_iterator){
      arr_vals[index] = std::get<int>(*start_iterator);
      ++index;
      ++start_iterator;
   };
};




void disk_out::fix_vect_vals(std::vector<Values>::iterator start_iterator, std::vector<Values>::iterator end_iterator, float* arr_vals){
   /* CASO para FLOATs:

   start_iterator y end_iterator son sólo del buffer, no los totales
   de todos los datos ni los d la partición
   */

   int index = 0;
   while(start_iterator < end_iterator){
      arr_vals[index] = std::get<float>(*start_iterator);
      ++index;
      ++start_iterator;
   };
};



void disk_out::fix_vect_vals(std::vector<Values>::iterator start_iterator, std::vector<Values>::iterator end_iterator, uint8_t* arr_vals){
   /* CASO para BOOLs:

   start_iterator y end_iterator son sólo del buffer, no los totales
   de todos los datos ni los d la partición
   */

   int index = 0;

   while(start_iterator < end_iterator){
      if(std::get<bool>(*start_iterator) == 1){
         arr_vals[index] = 1;
      }else{
         arr_vals[index] = 0;
      };
      ++index;
      ++start_iterator;
   };
};


// FUNCIONES AUXILIARES DE LAS FUNCIONES DE ESCRITURA:




void disk_out::obtain_writing_indexes(auto& idx_start, auto& idx_end, std::vector<Values>*& vec_var_ptr, int written_rows_input, int& rows_to_write_input){
   idx_start = vec_var_ptr->begin() + written_rows_input;
   idx_end = vec_var_ptr->begin() + written_rows_input + rows_to_write_input;
};

int disk_out::obtain_available_size(std::filesystem::path path_var_input, std::string last_partition_str_input, int size_partition_input){

   int available_space_to_write = 0;
   std::filesystem::path write_path = path_var_input += last_partition_str_input;
   uint32_t size_file = disk_aux::obtain_file_size(write_path);
   Logger::log(LogLevel::DEBUG, "File yet to write size is: ", false, true);
   Logger::log(LogLevel::DEBUG, size_file, true, false);
   Logger::flush(LogLevel::DEBUG);
   if(size_file > 0){
      available_space_to_write = size_partition_input - size_file;
   }else{
      available_space_to_write = size_partition_input;
   };

   return available_space_to_write;
};


void disk_out::write_partition_int(auto& idx_start, auto& idx_end, std::filesystem::path write_path_input){

   std::ofstream out;

   uint32_t num_rows_buffer = size_buffer_bytes / sizeof(int);
   int* arr_vals = nullptr;
   uint32_t written_rows_num;
   try{
      arr_vals = new int[num_rows_buffer];
      out.open(write_path_input += ".dat", std::ios::out | std::ios::binary | std::ios::app);
      Logger::log(LogLevel::DEBUG, "Partition file is open");
      std::vector<Values>::iterator tmp_ini_index;
      tmp_ini_index = idx_start;
      std::vector<Values>::iterator tmp_end_index;
      while(tmp_ini_index < idx_end){
         // Calculamos el tamaño del subvector a crear:
         tmp_end_index = tmp_ini_index + num_rows_buffer;
         if(tmp_end_index > idx_end){
            tmp_end_index = idx_end;
         };
         written_rows_num = tmp_end_index - tmp_ini_index;

         ////// PROCESO DE ESCRIURA ///////////////////

         disk_out::fix_vect_vals(tmp_ini_index, tmp_end_index, arr_vals);
         Logger::log(LogLevel::DEBUG, "'fix_vect_vals' executed successfully");
         char* vals_chars = reinterpret_cast<char*>(arr_vals);
         Logger::log(LogLevel::DEBUG, "Proceeding to disk write:");
         out.write(vals_chars, written_rows_num * sizeof(int));
         Logger::log(LogLevel::DEBUG, "Disk writing carried successfully");
         out.flush();
         //////////////////////////////////////////////
         tmp_ini_index += written_rows_num;
      };
   }catch(...){
      delete[] arr_vals;
      out.close();
      throw;
   };
   delete[] arr_vals;
   out.close();
};

void disk_out::write_partition_float(auto& idx_start, auto& idx_end, std::filesystem::path write_path_input){
   std::ofstream out;

   float* arr_vals = nullptr;

   uint32_t num_rows_buffer = size_buffer_bytes / sizeof(float);
   uint32_t written_rows_num;
   try {
      arr_vals = new float[num_rows_buffer];

      out.open(write_path_input += ".dat", std::ios::out | std::ios::binary | std::ios::app);
      Logger::log(LogLevel::DEBUG, "Partition file is open");
      std::vector<Values>::iterator tmp_ini_index;
      tmp_ini_index = idx_start;
      std::vector<Values>::iterator tmp_end_index;

      while(tmp_ini_index < idx_end){
         // Calculamos el tamaño del subvector a crear:
         tmp_end_index = tmp_ini_index + num_rows_buffer;
         if(tmp_end_index > idx_end){
            tmp_end_index = idx_end;
         };
         written_rows_num = tmp_end_index - tmp_ini_index;

         ////// PROCESO DE ESCRIURA ///////////////////

         disk_out::fix_vect_vals(tmp_ini_index, tmp_end_index, arr_vals);
         Logger::log(LogLevel::DEBUG, "'fix_vect_vals' executed successfully");
         char* vals_chars = reinterpret_cast<char*>(arr_vals);
         Logger::log(LogLevel::DEBUG, "Proceeding to disk write:");
         out.write(vals_chars, written_rows_num * sizeof(float));
         Logger::log(LogLevel::DEBUG, "Disk writing carried successfully");
         out.flush();
         //////////////////////////////////////////////
         tmp_ini_index += written_rows_num;
      };
   }catch(...){
      delete[] arr_vals;
      out.close();
      throw;
   };
   delete[] arr_vals;
   out.close();
};


void disk_out::write_partition_bool(auto& idx_start, auto& idx_end, std::filesystem::path write_path_input){
   std::ofstream out;

   uint8_t* arr_vals = nullptr;

   uint32_t num_rows_buffer = size_buffer_bytes / sizeof(uint8_t);
   uint32_t written_rows_num;

   try {
      arr_vals = new uint8_t[num_rows_buffer];
      out.open(write_path_input += ".dat", std::ios::out | std::ios::binary | std::ios::app);
      Logger::log(LogLevel::DEBUG, "Partition file is open");
      std::vector<Values>::iterator tmp_ini_index;
      tmp_ini_index = idx_start;
      std::vector<Values>::iterator tmp_end_index;
      while(tmp_ini_index < idx_end){
         // Calculamos el tamaño del subvector a crear:
         tmp_end_index = tmp_ini_index + num_rows_buffer;
         if(tmp_end_index > idx_end){
            tmp_end_index = idx_end;
         };
         written_rows_num = tmp_end_index - tmp_ini_index;

         ////// PROCESO DE ESCRIURA ///////////////////

         disk_out::fix_vect_vals(tmp_ini_index, tmp_end_index, arr_vals);
         Logger::log(LogLevel::DEBUG, "'fix_vect_vals' executed successfully");
         char* vals_chars = reinterpret_cast<char*>(arr_vals);
         Logger::log(LogLevel::DEBUG, "Proceeding to disk write:");
         out.write(vals_chars, written_rows_num);
         Logger::log(LogLevel::DEBUG, "Disk writing carried successfully");
         out.flush();
         //////////////////////////////////////////////
         tmp_ini_index += written_rows_num;
      };
   }catch(...){
      delete[] arr_vals;
      out.close();
      throw;
   };
   delete[] arr_vals;
   out.close();
};



struct byteStreamer {
   /*
   Los datos residen en un std::vector<Values>, esto significa
   que las strings que puedan albergar NO son contiguas.
   Para no complicar más el buffer de escritura, creamos este
   struct, cuyo objetivo es abstraer totalmente en la obtención
   del buffer, pudiendo así mantener un bucle de escritura muy sencillo
   */
   std::vector<Values>::iterator start_iterator;
   std::vector<Values>::iterator end_iterator;
   uint32_t char_idx = 0;
   uint32_t written_rows = 0;
   uint32_t string_accumulated_bytes = 0;
   bool is_unknown_type;

   bool is_data_left(){
      return start_iterator < end_iterator;
   };

   // Función que retornará un buffer lleno
   uint32_t extract_bytes(char* buffer, uint32_t* buffer_sizes, uint32_t limit_input){
      written_rows = 0;
      uint32_t idx_sizes = 0;
      
      uint32_t bytes_copied = 0;
      uint32_t string_total_size = 0;
      uint32_t str_size = 0;
      const char* str_begin = nullptr;

      if(this->is_unknown_type){
         while(start_iterator < end_iterator && bytes_copied < limit_input){

            // Case UNKNOWN
            auto& arg = std::get<std::vector<char>>(*start_iterator);
            str_size = arg.size();
            str_begin = reinterpret_cast<const char*>(arg.data());

            uint32_t string_remaining_bytes = str_size - char_idx;
            uint32_t buffer_free_space = limit_input - bytes_copied;

            // Sólo insertamos en el buffer si cabe en el buffer:
            if(string_remaining_bytes <= buffer_free_space){
               // CASo A: La string o lo que queda de esta cabe entera:
               std::memcpy(buffer + bytes_copied, str_begin + char_idx, string_remaining_bytes);

               bytes_copied += string_remaining_bytes;

               string_total_size = string_accumulated_bytes + string_remaining_bytes;
               buffer_sizes[idx_sizes] = string_total_size;

               ++idx_sizes;
               ++written_rows;
               char_idx = 0;
               ++start_iterator;
               string_accumulated_bytes = 0;

            }else{
               // CASO B: La string no cabe entera:
               // Copiamos solo el trozo exacto de caracteres que llene el espacio libre del buffer

               std::memcpy(buffer + bytes_copied, str_begin + char_idx, buffer_free_space);
               
               // Sumamos los últimos bytes para llegar exactamente al 'limit_input' (64 KB)
               bytes_copied += buffer_free_space;
               
               // ¡ESTADO FRAGMENTADO!: Recordamos la posición exacta de la letra donde nos quedamos
               char_idx += buffer_free_space; 
               
               // Rompemos el bucle 'while'. El buffer está lleno al 100%.
               string_accumulated_bytes += buffer_free_space;
               break;
            };
         };
      }else{
         while(start_iterator < end_iterator && bytes_copied < limit_input){

            // Case STRING
            auto& arg = std::get<std::string>(*start_iterator);
            str_size = arg.size();
            str_begin = reinterpret_cast<const char*>(arg.data());

            uint32_t string_remaining_bytes = str_size - char_idx;
            uint32_t buffer_free_space = limit_input - bytes_copied;

            // Sólo insertamos en el buffer si cabe en el buffer:
            if(string_remaining_bytes <= buffer_free_space){
               // CASo A: La string o lo que queda de esta cabe entera:
               std::memcpy(buffer + bytes_copied, str_begin + char_idx, string_remaining_bytes);

               bytes_copied += string_remaining_bytes;

               string_total_size = string_accumulated_bytes + string_remaining_bytes;
               buffer_sizes[idx_sizes] = string_total_size;

               ++idx_sizes;
               ++written_rows;
               char_idx = 0;
               ++start_iterator;
               string_accumulated_bytes = 0;

            }else{
               // CASO B: La string no cabe entera:
               // Copiamos solo el trozo exacto de caracteres que llene el espacio libre del buffer

               std::memcpy(buffer + bytes_copied, str_begin + char_idx, buffer_free_space);
               
               // Sumamos los últimos bytes para llegar exactamente al 'limit_input' (64 KB)
               bytes_copied += buffer_free_space;
               
               // ¡ESTADO FRAGMENTADO!: Recordamos la posición exacta de la letra donde nos quedamos
               char_idx += buffer_free_space; 
               
               // Rompemos el bucle 'while'. El buffer está lleno al 100%.
               string_accumulated_bytes += buffer_free_space;
               break;
            };
         };
      };
      return bytes_copied;
   };
};





void disk_out::write_partition_string(std::vector<Values>::iterator start_iterator, std::vector<Values>::iterator end_iterator, std::filesystem::path write_path_input){
   
   // NO nos hace falta el argumento int available_space_to_write como input.
   // Ya que start_iterator y end_iterator son las filas a escribir en una partición.
   
   std::ofstream out1;
   std::ofstream out2;

   std::filesystem::path path_bin = write_path_input;
   path_bin += ".bin";
   std::filesystem::path path_idx = write_path_input;
   path_idx += ".idx";

   out1.open(path_bin, std::ios::out | std::ios::binary | std::ios::app);
   out2.open(path_idx, std::ios::out | std::ios::binary | std::ios::app);
   Logger::log(LogLevel::DEBUG, "Partition file is open");


   char* arr_vals = nullptr;
   uint32_t* arr_sizes = nullptr;
   uint32_t num_rows_buffer = size_buffer_bytes / sizeof(uint32_t);
   uint32_t bytes_copied = 0;
   try {
      arr_vals = new char[size_buffer_bytes];
      arr_sizes = new uint32_t[num_rows_buffer];

      // inicializamos una sinstancia de byteStreamer:
      byteStreamer byte_streamer;
      byte_streamer.start_iterator = start_iterator;
      byte_streamer.end_iterator = end_iterator;
      byte_streamer.char_idx = 0;
      byte_streamer.written_rows = 0;
      byte_streamer.is_unknown_type = false;
      
      while(byte_streamer.is_data_left()){
         bytes_copied = byte_streamer.extract_bytes(arr_vals, arr_sizes, size_buffer_bytes);

         out1.write(arr_vals, bytes_copied);
         out1.flush();
         char* buffer_sizes_chars = reinterpret_cast<char*>(arr_sizes);
         out2.write(buffer_sizes_chars, byte_streamer.written_rows * sizeof(uint32_t));
         out2.flush();
      };
   }catch(...){
      delete[] arr_vals;
      delete[] arr_sizes;
      out1.close();
      out2.close();
      throw;
   };
   delete[] arr_vals;
   delete[] arr_sizes;
   out1.close();
   out2.close();
};


void disk_out::write_partition_unknown(std::vector<Values>::iterator start_iterator, std::vector<Values>::iterator end_iterator, std::filesystem::path write_path_input){
   
   // NO nos hace falta el argumento int available_space_to_write como input.
   // Ya que start_iterator y end_iterator son las filas a escribir en una partición.
   
   std::ofstream out1;
   std::ofstream out2;

   std::filesystem::path path_bin = write_path_input;
   path_bin += ".bin";
   std::filesystem::path path_idx = write_path_input;
   path_idx += ".idx";

   out1.open(path_bin, std::ios::out | std::ios::binary | std::ios::app);
   out2.open(path_idx, std::ios::out | std::ios::binary | std::ios::app);
   Logger::log(LogLevel::DEBUG, "Partition file is open");


   char* arr_vals = nullptr;
   uint32_t* arr_sizes = nullptr;
   uint32_t num_rows_buffer = size_buffer_bytes / sizeof(uint32_t);
   uint32_t bytes_copied = 0;
   try {
      arr_vals = new char[size_buffer_bytes];
      arr_sizes = new uint32_t[num_rows_buffer];

      // inicializamos una sinstancia de byteStreamer:
      byteStreamer byte_streamer;
      byte_streamer.start_iterator = start_iterator;
      byte_streamer.end_iterator = end_iterator;
      byte_streamer.char_idx = 0;
      byte_streamer.written_rows = 0;
      byte_streamer.is_unknown_type = true;
      while(byte_streamer.is_data_left()){
         bytes_copied = byte_streamer.extract_bytes(arr_vals, arr_sizes, size_buffer_bytes);

         out1.write(arr_vals, bytes_copied);
         out1.flush();
         char* buffer_sizes_chars = reinterpret_cast<char*>(arr_sizes);
         out2.write(buffer_sizes_chars, byte_streamer.written_rows * sizeof(uint32_t));
         out2.flush();
      };
   }catch(...){
      delete[] arr_vals;
      delete[] arr_sizes;
      out1.close();
      out2.close();
      throw;
   };
   delete[] arr_vals;
   delete[] arr_sizes;
   out1.close();
   out2.close();
};





void disk_out::writing_loop_int(std::vector<Values>*& vec_var_ptr, int total_bytes_to_write_input, int partition_entities, std::filesystem::path path_var_input, std::string last_partition_str_input, int last_partition_int_input){

   // Estas son las variables que usaremos para trackear lo que llevamos avanzado
   int bytes_written = 0;
   int written_rows = 0;
   int total_rows_to_write = total_bytes_to_write_input / 4;
   int size_partition = partition_entities * sizeof(int);
   Logger::flush(LogLevel::DEBUG);

   Logger::log(LogLevel::DEBUG, "Entering INT writing function");
   Logger::log(LogLevel::DEBUG, "Partition file size: ", false, true);
   Logger::log(LogLevel::DEBUG, size_partition, false, true);

   // Ahora consultamos la última partición para saber realmente en que númerio estamos:
   std::vector<Values>::iterator start_iterator;
   std::vector<Values>::iterator end_iterator;
   int rows_to_write;
   int bytes_to_write;
   int available_space_to_write = 0;

   int rows_left_to_write = 0;
   int bytes_left_to_write = 0;
   int rows_that_fit_in_current_partition = 0;
   
   while(total_bytes_to_write_input > bytes_written){
      /*
      1) Consultamos el tamaño que le queda a la partición por escribir
         Si no existiera, se asigna el tmaño de una partición entera
         (del tipo concreto)
      */
      available_space_to_write = disk_out::obtain_available_size(path_var_input,
                                                                       last_partition_str_input + ".dat",
                                                                       size_partition
                                                                      );
      Logger::log(LogLevel::DEBUG, "Available space within the partition file is: ", false, true);
      Logger::log(LogLevel::DEBUG, available_space_to_write, true, false);
      // 1) Calculamos las filas a escribir:
      rows_left_to_write = total_rows_to_write - written_rows;
      bytes_left_to_write = total_bytes_to_write_input - bytes_written;
      rows_that_fit_in_current_partition = available_space_to_write / sizeof(int);
      if(rows_left_to_write > rows_that_fit_in_current_partition){
         rows_to_write = rows_that_fit_in_current_partition;
         bytes_to_write = available_space_to_write;
      }else{
         rows_to_write = rows_left_to_write;
         bytes_to_write = bytes_left_to_write;
      };
      Logger::log(LogLevel::DEBUG, "Rows left to write: ", false, true);
      Logger::log(LogLevel::DEBUG, rows_to_write, true, false);
      if(rows_to_write > 0){
         // Sacamos los índices inicial y final;
         disk_out::obtain_writing_indexes(start_iterator,
                                                end_iterator,
                                                vec_var_ptr,
                                                written_rows,
                                                rows_to_write
                                                );

         // 3) Realizamos la escritura
         std::filesystem::path write_path = path_var_input;
         write_path += last_partition_str_input;
         disk_out::write_partition_int(start_iterator, end_iterator, write_path);
         // Una vez hecha la escritura, actualizamos los contadores
         // 4) Actualización de los contadores:
         bytes_written += bytes_to_write;
         written_rows += rows_to_write;
      };
      // Actualizamos el nombre de las particiones:
      Logger::log(LogLevel::DEBUG, "'last_partition_str_input' value before: ", false, true);
      Logger::log(LogLevel::DEBUG, last_partition_str_input, true, false);
      last_partition_int_input += 1;
      last_partition_str_input = std::to_string(last_partition_int_input);
      Logger::log(LogLevel::DEBUG, "'last_partition_str_input' value after: ", false, true);
      Logger::log(LogLevel::DEBUG, last_partition_str_input, true, false);
   };
};

void disk_out::writing_loop_float(std::vector<Values>*& vec_var_ptr, int total_bytes_to_write_input, int partition_entities, std::filesystem::path path_var_input, std::string last_partition_str_input, int last_partition_int_input){

   // Estas son las variables que usaremos para trackear lo que llevamos avanzado
   int bytes_written = 0;
   int written_rows = 0;
   int total_rows_to_write = total_bytes_to_write_input / 4;
   int size_partition = partition_entities * sizeof(float);
   Logger::flush(LogLevel::DEBUG);

   Logger::log(LogLevel::DEBUG, "Entering FLOAT writing function");
   Logger::log(LogLevel::DEBUG, "Partition file size: ", false, true);
   Logger::log(LogLevel::DEBUG, size_partition, false, true);

   // Ahora consultamos la última partición para saber realmente en que númerio estamos:
   std::vector<Values>::iterator start_iterator;
   std::vector<Values>::iterator end_iterator;
   int rows_to_write;
   int bytes_to_write;
   int available_space_to_write = 0;

   int rows_left_to_write = 0;
   int bytes_left_to_write = 0;
   int rows_that_fit_in_current_partition = 0;
   
   while(total_bytes_to_write_input > bytes_written){
      /*
      1) Consultamos el tamaño que le queda a la partición por escribir
         Si no existiera, se asigna el tmaño de una partición entera
         (del tipo concreto)
      */
      available_space_to_write = disk_out::obtain_available_size(path_var_input,
                                                                       last_partition_str_input + ".dat",
                                                                       size_partition
                                                                      );
      Logger::log(LogLevel::DEBUG, "Available space within the partition file is: ", false, true);
      Logger::log(LogLevel::DEBUG, available_space_to_write, true, false);
      // 1) Calculamos las filas a escribir:
      rows_left_to_write = total_rows_to_write - written_rows;
      bytes_left_to_write = total_bytes_to_write_input - bytes_written;
      rows_that_fit_in_current_partition = available_space_to_write / sizeof(float);
      if(rows_left_to_write > rows_that_fit_in_current_partition){
         rows_to_write = rows_that_fit_in_current_partition;
         bytes_to_write = available_space_to_write;
      }else{
         rows_to_write = rows_left_to_write;
         bytes_to_write = bytes_left_to_write;
      };
      Logger::log(LogLevel::DEBUG, "Rows left to write: ", false, true);
      Logger::log(LogLevel::DEBUG, rows_to_write, true, false);
      if(rows_to_write > 0){
         // Sacamos los índices inicial y final;
         disk_out::obtain_writing_indexes(start_iterator,
                                                end_iterator,
                                                vec_var_ptr,
                                                written_rows,
                                                rows_to_write
                                                );

         // 3) Realizamos la escritura
         std::filesystem::path write_path = path_var_input;
         write_path += last_partition_str_input;
         disk_out::write_partition_float(start_iterator, end_iterator, write_path);
         // Una vez hecha la escritura, actualizamos los contadores
         // 4) Actualización de los contadores:
         bytes_written += bytes_to_write;
         written_rows += rows_to_write;
      };
      // Actualizamos el nombre de las particiones:
      Logger::log(LogLevel::DEBUG, "'last_partition_str_input' value before: ", false, true);
      Logger::log(LogLevel::DEBUG, last_partition_str_input, true, false);
      last_partition_int_input += 1;
      last_partition_str_input = std::to_string(last_partition_int_input);
      Logger::log(LogLevel::DEBUG, "'last_partition_str_input' value after: ", false, true);
      Logger::log(LogLevel::DEBUG, last_partition_str_input, true, false);
   };
};


void disk_out::writing_loop_bool(std::vector<Values>*& vec_var_ptr, int total_bytes_to_write_input, int partition_entities, std::filesystem::path path_var_input, std::string last_partition_str_input, int last_partition_int_input){

   // Estas son las variables que usaremos para trackear lo que llevamos avanzado
   int bytes_written = 0;
   int written_rows = 0;
   int total_rows_to_write = total_bytes_to_write_input;
   int size_partition = partition_entities;
   Logger::flush(LogLevel::DEBUG);

   Logger::log(LogLevel::DEBUG, "Entering BOOL writing function");
   Logger::log(LogLevel::DEBUG, "Partition file size: ", false, true);
   Logger::log(LogLevel::DEBUG, size_partition, false, true);

   // Ahora consultamos la última partición para saber realmente en que númerio estamos:
   std::vector<Values>::iterator start_iterator;
   std::vector<Values>::iterator end_iterator;
   int rows_to_write;
   int bytes_to_write;
   int available_space_to_write = 0;

   int rows_left_to_write = 0;
   int bytes_left_to_write = 0;
   int rows_that_fit_in_current_partition = 0;
   
   while(total_bytes_to_write_input > bytes_written){
      /*
      1) Consultamos el tamaño que le queda a la partición por escribir
         Si no existiera, se asigna el tmaño de una partición entera
         (del tipo concreto)
      */
      available_space_to_write = disk_out::obtain_available_size(path_var_input,
                                                                       last_partition_str_input + ".dat",
                                                                       size_partition
                                                                      );
      Logger::log(LogLevel::DEBUG, "Available space within the partition file is: ", false, true);
      Logger::log(LogLevel::DEBUG, available_space_to_write, true, false);
      // 1) Calculamos las filas a escribir:
      rows_left_to_write = total_rows_to_write - written_rows;
      bytes_left_to_write = total_bytes_to_write_input - bytes_written;
      rows_that_fit_in_current_partition = available_space_to_write / sizeof(uint8_t);
      if(rows_left_to_write > rows_that_fit_in_current_partition){
         rows_to_write = rows_that_fit_in_current_partition;
         bytes_to_write = available_space_to_write;
      }else{
         rows_to_write = rows_left_to_write;
         bytes_to_write = bytes_left_to_write;
      };
      Logger::log(LogLevel::DEBUG, "Rows left to write: ", false, true);
      Logger::log(LogLevel::DEBUG, rows_to_write, true, false);
      if(rows_to_write > 0){
         // Sacamos los índices inicial y final;
         disk_out::obtain_writing_indexes(start_iterator,
                                                end_iterator,
                                                vec_var_ptr,
                                                written_rows,
                                                rows_to_write
                                                );

         // 3) Realizamos la escritura
         std::filesystem::path write_path = path_var_input;
         write_path += last_partition_str_input;
         disk_out::write_partition_bool(start_iterator, end_iterator, write_path);
         // Una vez hecha la escritura, actualizamos los contadores
         // 4) Actualización de los contadores:
         bytes_written += bytes_to_write;
         written_rows += rows_to_write;
      };
      // Actualizamos el nombre de las particiones:
      Logger::log(LogLevel::DEBUG, "'last_partition_str_input' value before: ", false, true);
      Logger::log(LogLevel::DEBUG, last_partition_str_input, true, false);
      last_partition_int_input += 1;
      last_partition_str_input = std::to_string(last_partition_int_input);
      Logger::log(LogLevel::DEBUG, "'last_partition_str_input' value after: ", false, true);
      Logger::log(LogLevel::DEBUG, last_partition_str_input, true, false);
   };
};


void disk_out::writing_loop_tring(std::vector<Values>*& vec_var_ptr, int total_bytes_to_write_input, int partition_entities, std::filesystem::path path_var_input, std::string last_partition_str_input, int last_partition_int_input){

   // Estas son las variables que usaremos para trackear lo que llevamos avanzado
   int bytes_written = 0;
   int written_rows = 0;
   int total_rows_to_write = total_bytes_to_write_input / 4;
   int size_partition = partition_entities * sizeof(uint32_t);
   Logger::flush(LogLevel::DEBUG);

   Logger::log(LogLevel::DEBUG, "Entering STRING writing function");
   Logger::log(LogLevel::DEBUG, "Partition file size: ", false, true);
   Logger::log(LogLevel::DEBUG, size_partition, false, true);

   // Ahora consultamos la última partición para saber realmente en que númerio estamos:
   std::vector<Values>::iterator start_iterator;
   std::vector<Values>::iterator end_iterator;
   int rows_to_write;
   int bytes_to_write;
   int available_space_to_write = 0;

   int rows_left_to_write = 0;
   int bytes_left_to_write = 0;
   int rows_that_fit_in_current_partition = 0;
   
   while(total_bytes_to_write_input > bytes_written){
      /*
      1) Consultamos el tamaño que le queda a la partición por escribir
         Si no existiera, se asigna el tmaño de una partición entera
         (del tipo concreto)
      */
      available_space_to_write = disk_out::obtain_available_size(path_var_input,
                                                                       last_partition_str_input + ".idx",
                                                                       size_partition
                                                                      );
      Logger::log(LogLevel::DEBUG, "Available space within the partition file is: ", false, true);
      Logger::log(LogLevel::DEBUG, available_space_to_write, true, false);
      // 1) Calculamos las filas a escribir:
      rows_left_to_write = total_rows_to_write - written_rows;
      bytes_left_to_write = total_bytes_to_write_input - bytes_written;
      rows_that_fit_in_current_partition = available_space_to_write / sizeof(uint32_t);
      if(rows_left_to_write > rows_that_fit_in_current_partition){
         rows_to_write = rows_that_fit_in_current_partition;
         bytes_to_write = available_space_to_write;
      }else{
         rows_to_write = rows_left_to_write;
         bytes_to_write = bytes_left_to_write;
      };
      Logger::log(LogLevel::DEBUG, "Rows left to write: ", false, true);
      Logger::log(LogLevel::DEBUG, rows_to_write, true, false);
      if(rows_to_write > 0){
         // Sacamos los índices inicial y final;
         disk_out::obtain_writing_indexes(start_iterator,
                                                end_iterator,
                                                vec_var_ptr,
                                                written_rows,
                                                rows_to_write
                                                );

         // 3) Realizamos la escritura
         std::filesystem::path write_path = path_var_input;
         write_path += last_partition_str_input;
         disk_out::write_partition_string(start_iterator, end_iterator, write_path);
         // Una vez hecha la escritura, actualizamos los contadores
         // 4) Actualización de los contadores:
         bytes_written += bytes_to_write;
         written_rows += rows_to_write;
      };
      // Actualizamos el nombre de las particiones:
      Logger::log(LogLevel::DEBUG, "'last_partition_str_input' value before: ", false, true);
      Logger::log(LogLevel::DEBUG, last_partition_str_input, true, false);
      last_partition_int_input += 1;
      last_partition_str_input = std::to_string(last_partition_int_input);
      Logger::log(LogLevel::DEBUG, "'last_partition_str_input' value after: ", false, true);
      Logger::log(LogLevel::DEBUG, last_partition_str_input, true, false);
   };
};



void disk_out::writing_loop_unknown(std::vector<Values>*& vec_var_ptr, int total_bytes_to_write_input, int partition_entities, std::filesystem::path path_var_input, std::string last_partition_str_input, int last_partition_int_input){

   // Estas son las variables que usaremos para trackear lo que llevamos avanzado
   int bytes_written = 0;
   int written_rows = 0;
   int total_rows_to_write = total_bytes_to_write_input / 4;
   int size_partition = partition_entities * sizeof(uint32_t);
   Logger::flush(LogLevel::DEBUG);

   Logger::log(LogLevel::DEBUG, "Entering UNKNOWNs writing function");
   Logger::log(LogLevel::DEBUG, "Partition file size: ", false, true);
   Logger::log(LogLevel::DEBUG, size_partition, false, true);

   // Ahora consultamos la última partición para saber realmente en que númerio estamos:
   std::vector<Values>::iterator start_iterator;
   std::vector<Values>::iterator end_iterator;
   int rows_to_write;
   int bytes_to_write;
   int available_space_to_write = 0;

   int rows_left_to_write = 0;
   int bytes_left_to_write = 0;
   int rows_that_fit_in_current_partition = 0;
   
   while(total_bytes_to_write_input > bytes_written){
      /*
      1) Consultamos el tamaño que le queda a la partición por escribir
         Si no existiera, se asigna el tmaño de una partición entera
         (del tipo concreto)
      */
      available_space_to_write = disk_out::obtain_available_size(path_var_input,
                                                                       last_partition_str_input + ".idx",
                                                                       size_partition
                                                                      );
      Logger::log(LogLevel::DEBUG, "Available space within the partition file is: ", false, true);
      Logger::log(LogLevel::DEBUG, available_space_to_write, true, false);
      // 1) Calculamos las filas a escribir:
      rows_left_to_write = total_rows_to_write - written_rows;
      bytes_left_to_write = total_bytes_to_write_input - bytes_written;
      rows_that_fit_in_current_partition = available_space_to_write / sizeof(uint32_t);
      if(rows_left_to_write > rows_that_fit_in_current_partition){
         rows_to_write = rows_that_fit_in_current_partition;
         bytes_to_write = available_space_to_write;
      }else{
         rows_to_write = rows_left_to_write;
         bytes_to_write = bytes_left_to_write;
      };
      Logger::log(LogLevel::DEBUG, "Rows left to write: ", false, true);
      Logger::log(LogLevel::DEBUG, rows_to_write, true, false);
      if(rows_to_write > 0){
         // Sacamos los índices inicial y final;
         disk_out::obtain_writing_indexes(start_iterator,
                                                end_iterator,
                                                vec_var_ptr,
                                                written_rows,
                                                rows_to_write
                                                );

         // 3) Realizamos la escritura
         std::filesystem::path write_path = path_var_input;
         write_path += last_partition_str_input;
         disk_out::write_partition_unknown(start_iterator, end_iterator, write_path);
         // Una vez hecha la escritura, actualizamos los contadores
         // 4) Actualización de los contadores:
         bytes_written += bytes_to_write;
         written_rows += rows_to_write;
      };
      // Actualizamos el nombre de las particiones:
      Logger::log(LogLevel::DEBUG, "'last_partition_str_input' value before: ", false, true);
      Logger::log(LogLevel::DEBUG, last_partition_str_input, true, false);
      last_partition_int_input += 1;
      last_partition_str_input = std::to_string(last_partition_int_input);
      Logger::log(LogLevel::DEBUG, "'last_partition_str_input' value after: ", false, true);
      Logger::log(LogLevel::DEBUG, last_partition_str_input, true, false);
   };
};




void disk_out::write_table_data(table* table_ptr_input){

   Logger::log(LogLevel::DEBUG, "<<< ENTERING PARTITION WRITING FUNCTION >>>>");

   // Recuperamos los tipos de datos:
   std::vector<dataType> column_types = table_ptr_input->metadata_ptr->column_types;
   // Recuperamos el nombre de la columna:
   std::vector<std::string> column_names = table_ptr_input->metadata_ptr->column_names;
   // Recuperamos el vector de vectores de los datos:
   std::map<std::string, std::vector<Values>>& vec_de_vec_vals = table_ptr_input->data_ptr->columns;
   // Recuperamos el nombre de la table_ptr_input:
   std::string table_name_str = table_ptr_input->metadata_ptr->name;
   Logger::log(LogLevel::DEBUG, "Table necessary values retrieved successfully");

   // Antes de nada, creamos el directorio de la tabla:
   std::filesystem::path table_path = "data/" + table_name_str;
   std::filesystem::create_directories(table_path);
   Logger::log(LogLevel::DEBUG, "Table data directory has been created");

   // Ahora, examinamos cuantas particiones hay:
   std::vector<std::string> partition_names_str;
   partition_names_str = disk_aux::obtain_files_in_path(table_path / column_names[0], column_types[0]);
   std::string last_partition = "";
   //////////////////////////////////////////////////////////////////////
   // Obtenemos la última partición:
   std::string last_partition_str = "0";
   int last_partition_int = 0;


   if(!partition_names_str.empty()){
      Logger::log(LogLevel::DEBUG, "Performing bubble sort algorithm");
      part_sort::bubble_sort(partition_names_str);
      Logger::log(LogLevel::DEBUG, "Sorting carried successfully");
      int parts = partition_names_str.size();
      last_partition = partition_names_str[parts-1];
      if(last_partition != ""){
         Logger::log(LogLevel::DEBUG, "Last partition is not '' ");
         Logger::log(LogLevel::DEBUG, "'last_partition' value is: ", false, true);
         Logger::log(LogLevel::DEBUG, last_partition, true, false);
         last_partition_str = part_sort::process_partition_number_as_string(last_partition);
         Logger::log(LogLevel::DEBUG, "'last_partition_str' value is: ", false, true);
         Logger::log(LogLevel::DEBUG, last_partition_str, false, true);
         last_partition_int = std::stoi(last_partition_str);
      };
   };

   Logger::log(LogLevel::DEBUG, "Las partition has been found: ", false, true);
   Logger::log(LogLevel::DEBUG, last_partition_str, true, false);
   // Ahora obtenemos el ultimo numero en forma de string y entero, para hacerlo sólo una vez:


   // VARIABLES NECESARIAS ANTES DEL BUCLE:
   uint32_t num_cols = table_ptr_input->metadata_ptr->n_cols;
   int total_bytes_to_write = 0; // Cantidad que nos servirá poara ver cuánto tenemos que escribir:
   int partition_entities = 3;

   // Iteramos por cada elemento
   for(int i = 0; i<num_cols; i++){
      // Iteramos por cada columna:
      dataType& data_type = column_types[i];
      std::string& column_name = column_names[i];
      std::vector<Values>* vec_var_ptr = &(vec_de_vec_vals.at(column_name));
      // Creamos el directorio de cada varuiable de la tabla:
      std::filesystem::path column_path = std::filesystem::path("data") / table_name_str / column_name;
      std::filesystem::create_directories(column_path);
      std::filesystem::path path_var = column_path / "part_";

      switch(data_type){
         case dataType::INT: {
            total_bytes_to_write = vec_var_ptr->size() * 4; //  Tamaño total de lo que hay que escribir

            disk_out::writing_loop_int(vec_var_ptr, total_bytes_to_write, partition_entities, path_var, last_partition_str, last_partition_int);
            break;
         };
         case dataType::FLOAT: {
            total_bytes_to_write = vec_var_ptr->size() * 4; //  Tamaño total de lo que hay que escribir

            disk_out::writing_loop_float(vec_var_ptr, total_bytes_to_write, partition_entities, path_var, last_partition_str, last_partition_int);
            break;
         };
         case dataType::BOOL: {
            total_bytes_to_write = vec_var_ptr->size();
            disk_out::writing_loop_bool(vec_var_ptr, total_bytes_to_write, partition_entities, path_var, last_partition_str, last_partition_int);
            break;
         };
         case dataType::STRING: {
            total_bytes_to_write = vec_var_ptr->size() * 4;
            disk_out::writing_loop_tring(vec_var_ptr, total_bytes_to_write, partition_entities, path_var, last_partition_str, last_partition_int);
            break;
         };
         case dataType::UNKNOWN: {
            total_bytes_to_write = vec_var_ptr->size() * 4;
            disk_out::writing_loop_unknown(vec_var_ptr, total_bytes_to_write, partition_entities, path_var, last_partition_str, last_partition_int);
            break;
         };

      };
   };
};