

#include "data_struct.h"
#include <iostream>
#include <fstream>
#include <string>
#include "globals.h"
#include <map>
#include "logging.h"
#include "filesystem"
#include "disk_buffer.h"
#include <cstring> // Para usar std::memcpy
#include "part_sort.h"
#include "disk_aux.h"
#include "disk_out.h"



////////////////////////////////////////////////////////////////////
// ESCRITURA DE DATOS //////////////////////////////////////////////
////////////////////////////////////////////////////////////////////



/*void disk_out::write_fixed_len_columns(std::string nombre_tabla, std::string column_name, std::vector<int>& vec_vals){

   std::ofstream out;
   out.open("data/" + nombre_tabla + "/" + column_name + ".dat", std::ios::out | std::ios::binary);

   //
   uint32_t size_valores = vec_vals.size() * sizeof(int);
   char* valores_chars = reinterpret_cast<char*>(vec_vals.data());

   out.write(valores_chars, size_valores);
   out.flush();
   out.close();
};

void disk_out::write_fixed_len_columns(std::string nombre_tabla, std::string column_name, std::vector<float>& vec_vals){

   std::ofstream out;
   out.open("data/" + nombre_tabla + "/" + column_name + ".dat", std::ios::out | std::ios::binary);

   //
   uint32_t size_valores = vec_vals.size() * sizeof(float);
   char* valores_chars = reinterpret_cast<char*>(vec_vals.data());

   out.write(valores_chars, size_valores);
   out.flush();
   out.close();
};

void disk_out::write_fixed_len_columns(std::string nombre_tabla, std::string column_name, std::vector<uint8_t>& vec_vals){

   std::ofstream out;
   out.open("data/" + nombre_tabla + "/" + column_name + ".dat", std::ios::out | std::ios::binary);

   //
   uint32_t size_valores = vec_vals.size();
   // Este es un caso especial, iteramos por cadaelemento
   char* valores_chars = reinterpret_cast<char*>(vec_vals.data());

   out.write(valores_chars, size_valores);
   out.flush();
   out.close();
};*/




void disk_out::fix_vect_vals(std::vector<Values>::iterator inicio, std::vector<Values>::iterator fin, int* arr_vals){
   /* CASO para INTs:

   inicio y fin son sólo del buffer, no los totales
   de todos los datos ni los d la partición
   */

   int index = 0;

   //vec_vals.clear();
   while(inicio < fin){
      //vec_vals.push_back(std::get<int>(*inicio));
      arr_vals[index] = std::get<int>(*inicio);
      ++index;
      ++inicio;
   };
};




void disk_out::fix_vect_vals(std::vector<Values>::iterator inicio, std::vector<Values>::iterator fin, float* arr_vals){
   /* CASO para FLOATs:

   inicio y fin son sólo del buffer, no los totales
   de todos los datos ni los d la partición
   */

   int index = 0;
   //vec_vals.clear();
   while(inicio < fin){
      //vec_vals.push_back(std::get<float>(*inicio));
      arr_vals[index] = std::get<float>(*inicio);
      ++index;
      ++inicio;
   };
};



void disk_out::fix_vect_vals(std::vector<Values>::iterator inicio, std::vector<Values>::iterator fin, uint8_t* arr_vals){
   /* CASO para BOOLs:

   inicio y fin son sólo del buffer, no los totales
   de todos los datos ni los d la partición
   */

   int index = 0;

   //vec_vals.clear();
   while(inicio < fin){
      if(std::get<bool>(*inicio) == 1){
         arr_vals[index] = 1;
      }else{
         arr_vals[index] = 0;
      };
      ++index;
      ++inicio;
   };
};

/*void disk_out::fix_vect_vals(std::vector<Values>& input, std::vector<std::string>& vec_vals, std::vector<uint32_t>& vec_sizes){

   Caso especial al registrar una string, que no solo almacenamos las
   variables en sí, si no su tamaño o dónde acaba cada una.

   uint32_t size_input = input.size();
   uint32_t acc_size = 0;
   std::string tmp_string;
   for(int i = 0; i<size_input; i++){
      tmp_string = std::get<std::string>(input[i]);
      //acc_size += tmp_string.size();
      acc_size = tmp_string.size();
      vec_vals.push_back(tmp_string);
      vec_sizes.push_back(acc_size);
   };
};*/

uint32_t disk_out::fix_vect_vals(std::vector<Values>::iterator inicio, std::vector<Values>::iterator fin, std::vector<char>& vec_vals, std::vector<uint32_t>& vec_sizes){
   /* CASO para STRINGs:

   inicio y fin son sólo del buffer, no los totales
   de todos los datos ni los de la partición

   Usado en la función: escribir_particion_string
   */

   uint32_t batch_size = 0;
   std::string tmp_string;
   vec_vals.clear();
   vec_sizes.clear();
   uint32_t acc_size = 0;
   while(inicio < fin){
      tmp_string = std::get<std::string>(*inicio);
      acc_size = tmp_string.size();
      //vec_vals.push_back(tmp_string.data());
      vec_vals.insert(vec_vals.end(), tmp_string.begin(), tmp_string.end());
      vec_sizes.push_back(acc_size);
      batch_size += acc_size;
      ++inicio;
   };
   return batch_size;
};





// FUNCIONES AUXILIARES DE LAS FUNCIONES DE ESCRITURA:

void disk_out::calcular_filas_a_escribir_int(int& filas_a_escribir, int size_disponible_para_escribir, int partition_entities, int filas_totales_a_escribir){

   int filas_por_escribir_en_particion = size_disponible_para_escribir / 4;

   if(size_disponible_para_escribir == 0){
      // En caso de ser archivo nuevo:
      if(filas_totales_a_escribir > partition_entities){
         filas_a_escribir = partition_entities;
      }else{
         filas_a_escribir = filas_totales_a_escribir;
      };
      return;
   }else{
      if(filas_por_escribir_en_particion > filas_totales_a_escribir){
         filas_a_escribir = filas_totales_a_escribir;
      }else{
         filas_a_escribir = filas_por_escribir_en_particion;
      };
   };
};


void disk_out::calcular_filas_a_escribir_float(int& filas_a_escribir, int size_disponible_para_escribir, int partition_entities, int filas_totales_a_escribir){

   int filas_por_escribir_en_particion = size_disponible_para_escribir / 4;

   if(size_disponible_para_escribir == 0){
      // En caso de ser archivo nuevo:
      if(filas_totales_a_escribir > partition_entities){
         filas_a_escribir = partition_entities;
      }else{
         filas_a_escribir = filas_totales_a_escribir;
      };
      return;
   }else{
      if(filas_por_escribir_en_particion > filas_totales_a_escribir){
         filas_a_escribir = filas_totales_a_escribir;
      }else{
         filas_a_escribir = filas_por_escribir_en_particion;
      };
   };
};


void disk_out::calcular_filas_a_escribir_bool(int& filas_a_escribir, int size_disponible_para_escribir, int partition_entities, int filas_totales_a_escribir){

   int filas_por_escribir_en_particion = size_disponible_para_escribir;

   if(size_disponible_para_escribir == 0){
      // En caso de ser archivo nuevo:
      if(filas_totales_a_escribir > partition_entities){
         filas_a_escribir = partition_entities;
      }else{
         filas_a_escribir = filas_totales_a_escribir;
      };
      return;
   }else{
      if(filas_por_escribir_en_particion > filas_totales_a_escribir){
         filas_a_escribir = filas_totales_a_escribir;
      }else{
         filas_a_escribir = filas_por_escribir_en_particion;
      };
   };
};


void disk_out::calcular_filas_a_escribir_string(int& filas_a_escribir, int size_disponible_para_escribir, int partition_entities, int filas_totales_a_escribir){

   int filas_por_escribir_en_particion = size_disponible_para_escribir / 4;

   if(size_disponible_para_escribir == 0){
      // En caso de ser archivo nuevo:
      if(filas_totales_a_escribir > partition_entities){
         filas_a_escribir = partition_entities;
      }else{
         filas_a_escribir = filas_totales_a_escribir;
      };
      return;
   }else{
      if(filas_por_escribir_en_particion > filas_totales_a_escribir){
         filas_a_escribir = filas_totales_a_escribir;
      }else{
         filas_a_escribir = filas_por_escribir_en_particion;
      };
   };
};


void disk_out::obtener_indices_vector_valores(auto& inicio, auto& fin, std::vector<Values>*& vec_var_ptr, int filas_escritas, int& filas_a_escribir){
   inicio = vec_var_ptr->begin() + filas_escritas;
   /*filas_a_escribir = 0;
   if(partition_entities > filas_totales_a_escribir){
      filas_a_escribir = filas_totales_a_escribir;
   }else{
      filas_a_escribir = partition_entities;
   };*/
   fin = vec_var_ptr->begin() + filas_escritas + filas_a_escribir;
};

int disk_out::obtener_size_disponible(std::string ruta_variable, std::string ultima_particion_str, int size_particion){

   int size_disponible_para_escribir = 0;
   std::string ruta_escritura = ruta_variable + ultima_particion_str;
   uint32_t size_arhivo = disk_aux::obtener_tamano_archivo(ruta_escritura);
   Logger::log(LogLevel::OUTPUT, "El tamaño del archivo a escribir es: ", false, true);
   Logger::log(LogLevel::OUTPUT, size_arhivo, true, false);
   Logger::flush();
   if(size_arhivo > 0){
      size_disponible_para_escribir = size_particion - size_arhivo;
   }else{
      size_disponible_para_escribir = size_particion;
   };

   return size_disponible_para_escribir;
};


void disk_out::escribir_particion_int(auto& inicio, auto& fin, std::string ruta_escritura){

   std::ofstream out;
   //std::vector<int> vec_vals;
   //uint32_t size_particion = size_buffer_bytes / sizeof(int);
   uint32_t num_rows_buffer = size_buffer_bytes / sizeof(int);
   int* arr_vals = nullptr;
   uint32_t num_filas_escritas;
   try{
      arr_vals = new int[num_rows_buffer];
      //vec_vals.reserve(size_particion);
      out.open(ruta_escritura + ".dat", std::ios::out | std::ios::binary | std::ios::app);
      Logger::log(LogLevel::OUTPUT, "La particion esta abierta");
      std::vector<Values>::iterator tmp_ini_index;
      tmp_ini_index = inicio;
      std::vector<Values>::iterator tmp_end_index;
      //Logger::log(LogLevel::OUTPUT, "Empezamos la escritura");
      while(tmp_ini_index < fin){
         // Calculamos el tamaño del subvector a crear:
         tmp_end_index = tmp_ini_index + num_rows_buffer;
         if(tmp_end_index > fin){
            tmp_end_index = fin;
         };
         num_filas_escritas = tmp_end_index - tmp_ini_index;
         //std::vector<Values> sub_vector(tmp_ini_index, tmp_end_index);
         ////// PROCESO DE ESCRIURA ///////////////////
         //vec_vals.clear();
         disk_out::fix_vect_vals(tmp_ini_index, tmp_end_index, arr_vals);
         Logger::log(LogLevel::OUTPUT, "Hemos aplicado 'fix_vect_vals' con exito");
         char* valores_chars = reinterpret_cast<char*>(arr_vals);
         Logger::log(LogLevel::OUTPUT, "Proceedemos a escribir en disco:");
         out.write(valores_chars, num_filas_escritas * sizeof(int));
         Logger::log(LogLevel::OUTPUT, "Escritura en disco realizada con exito");
         out.flush();
         //////////////////////////////////////////////
         tmp_ini_index += num_filas_escritas;
      };
   }catch(...){
      delete[] arr_vals;
      out.close();
      throw;
   };
   delete[] arr_vals;
   out.close();
};

void disk_out::escribir_particion_float(auto& inicio, auto& fin, std::string ruta_escritura){
   std::ofstream out;
   //std::vector<Values> sub_vector(inicio, fin);
   //std::vector<float> vec_vals;
   float* arr_vals = nullptr;
   //uint32_t size_particion = size_buffer_bytes / sizeof(float);
   uint32_t num_rows_buffer = size_buffer_bytes / sizeof(float);
   uint32_t num_filas_escritas;
   try {
      arr_vals = new float[num_rows_buffer];
      //vec_vals.reserve(size_particion);
      out.open(ruta_escritura + ".dat", std::ios::out | std::ios::binary | std::ios::app);
      Logger::log(LogLevel::OUTPUT, "La particion esta abierta");
      std::vector<Values>::iterator tmp_ini_index;
      tmp_ini_index = inicio;
      std::vector<Values>::iterator tmp_end_index;
      //Logger::log(LogLevel::OUTPUT, "Empezamos la escritura");
      while(tmp_ini_index < fin){
         // Calculamos el tamaño del subvector a crear:
         tmp_end_index = tmp_ini_index + num_rows_buffer;
         if(tmp_end_index > fin){
            tmp_end_index = fin;
         };
         num_filas_escritas = tmp_end_index - tmp_ini_index;
         //std::vector<Values> sub_vector(tmp_ini_index, tmp_end_index);
         ////// PROCESO DE ESCRIURA ///////////////////
         //vec_vals.clear();
         disk_out::fix_vect_vals(tmp_ini_index, tmp_end_index, arr_vals);
         Logger::log(LogLevel::OUTPUT, "Hemos aplicado 'fix_vect_vals' con exito");
         char* valores_chars = reinterpret_cast<char*>(arr_vals);
         Logger::log(LogLevel::OUTPUT, "Proceedemos a escribir en disco:");
         out.write(valores_chars, num_filas_escritas * sizeof(float));
         Logger::log(LogLevel::OUTPUT, "Escritura en disco realizada con exito");
         out.flush();
         //////////////////////////////////////////////
         tmp_ini_index += num_filas_escritas;
      };
   }catch(...){
      delete[] arr_vals;
      out.close();
      throw;
   };
   delete[] arr_vals;
   out.close();
};


void disk_out::escribir_particion_bool(auto& inicio, auto& fin, std::string ruta_escritura){
   std::ofstream out;
   //std::vector<Values> sub_vector(inicio, fin);
   //std::vector<uint8_t> vec_vals;
   uint8_t* arr_vals = nullptr;

   //uint32_t size_particion = size_buffer_bytes / sizeof(uint8_t);
   uint32_t num_rows_buffer = size_buffer_bytes / sizeof(uint8_t);
   uint32_t num_filas_escritas;

   try {
      //vec_vals.reserve(size_particion);
      arr_vals = new uint8_t[num_rows_buffer];
      out.open(ruta_escritura + ".dat", std::ios::out | std::ios::binary | std::ios::app);
      Logger::log(LogLevel::OUTPUT, "La particion esta abierta");
      std::vector<Values>::iterator tmp_ini_index;
      tmp_ini_index = inicio;
      std::vector<Values>::iterator tmp_end_index;
      //Logger::log(LogLevel::OUTPUT, "Empezamos la escritura");
      while(tmp_ini_index < fin){
         // Calculamos el tamaño del subvector a crear:
         tmp_end_index = tmp_ini_index + num_rows_buffer;
         if(tmp_end_index > fin){
            tmp_end_index = fin;
         };
         num_filas_escritas = tmp_end_index - tmp_ini_index;
         //std::vector<Values> sub_vector(tmp_ini_index, tmp_end_index);
         ////// PROCESO DE ESCRIURA ///////////////////
         //vec_vals.clear();
         disk_out::fix_vect_vals(tmp_ini_index, tmp_end_index, arr_vals);
         Logger::log(LogLevel::OUTPUT, "Hemos aplicado 'fix_vect_vals' con exito");
         char* valores_chars = reinterpret_cast<char*>(arr_vals);
         Logger::log(LogLevel::OUTPUT, "Proceedemos a escribir en disco:");
         out.write(valores_chars, num_filas_escritas);
         Logger::log(LogLevel::OUTPUT, "Escritura en disco realizada con exito");
         out.flush();
         //////////////////////////////////////////////
         tmp_ini_index += num_filas_escritas;
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
   std::vector<Values>::iterator inicio;
   std::vector<Values>::iterator fin;
   uint32_t char_idx = 0;
   uint32_t filas_escritas = 0;
   uint32_t string_acumulada_bytes = 0;
   bool is_unknown_type;

   bool is_data_left(){
      return inicio < fin;
   };

   // Función que retornará un buffer lleno
   uint32_t extraer_bytes(char* buffer, uint32_t* buffer_sizes, uint32_t limite){
      filas_escritas = 0;
      uint32_t idx_sizes = 0;
      //buffer.clear();
      //buffer_sizes.clear();
      
      uint32_t bytes_copiados = 0;
      uint32_t tamano_total_string = 0;
      uint32_t str_size = 0;
      const char* str_begin = nullptr;
      // Se para la copia cuando se haya alcanzado el fin de las filas  o no quede más especio en la partición:
      while(inicio < fin && bytes_copiados < limite){
         std::visit([&str_size, &str_begin](auto&& arg) {
            using type = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<type, std::string> || std::is_same_v<type, std::vector<char>>) {
               str_size = arg.size();
               str_begin = reinterpret_cast<const char*>(arg.data());
            }
         }, *inicio);

         uint32_t bytes_disponibles_en_str = str_size - char_idx;

         uint32_t espacio_libre_en_buffer = limite - bytes_copiados;

         // Sólo insertamos en el buffer si cabe en el buffer:
         if(bytes_disponibles_en_str <= espacio_libre_en_buffer){
            // CASo A: La string o lo que queda de esta cabe entera:
            //buffer.insert(buffer.end(), str_begin + char_idx, str_begin + str_size);
            std::memcpy(buffer + bytes_copiados, str_begin + char_idx, bytes_disponibles_en_str);

            bytes_copiados += bytes_disponibles_en_str;

            tamano_total_string = string_acumulada_bytes + bytes_disponibles_en_str;
            //buffer_sizes.push_back(tamano_total_string);
            buffer_sizes[idx_sizes] = tamano_total_string;

            ++idx_sizes;
            ++filas_escritas;
            char_idx = 0;
            ++inicio;
            string_acumulada_bytes = 0;

         }else{
            // CASO B: La string no cabe entera:
            // Copiamos solo el trozo exacto de caracteres que llene el espacio libre del buffer
            //buffer.insert(buffer.end(), 
                          //str_begin + char_idx, 
                          //str_begin + char_idx + espacio_libre_en_buffer);
            std::memcpy(buffer + bytes_copiados, str_begin + char_idx, espacio_libre_en_buffer);
            
            // Sumamos los últimos bytes para llegar exactamente al 'limite' (64 KB)
            bytes_copiados += espacio_libre_en_buffer;
            
            // ¡ESTADO FRAGMENTADO!: Recordamos la posición exacta de la letra donde nos quedamos
            char_idx += espacio_libre_en_buffer; 
            
            // Rompemos el bucle 'while'. El buffer está lleno al 100%.
            // OJO: NO avanzamos '++actual'. Nos quedamos apuntando a la misma string.
            //Logger::log(LogLevel::DEBUG, "El tamaño disponible en la particion es: ", false, true);
            /*if(filas_escritas == 0){
               buffer_sizes.push_back(espacio_libre_en_buffer);
            }else{
               buffer_sizes.back() += espacio_libre_en_buffer;
            };*/
            string_acumulada_bytes += espacio_libre_en_buffer;
            break;
         };
      };
      return bytes_copiados;
   };
};





void disk_out::escribir_particion_string(std::vector<Values>::iterator inicio, std::vector<Values>::iterator fin, std::string ruta_escritura){
   
   // NO nos hace falta el argumento int size_disponible_para_escribir como input.
   // Ya que inicio y fin son las filas a escribir en una partición.
   
   std::ofstream out1;
   std::ofstream out2;
   //std::vector<uint32_t> vec_sizes;

   out1.open(ruta_escritura + ".bin", std::ios::out | std::ios::binary | std::ios::app);
   out2.open(ruta_escritura + ".idx", std::ios::out | std::ios::binary | std::ios::app);
   Logger::log(LogLevel::OUTPUT, "La particion esta abierta");



   //std::vector<char> buffer_vals;
   //std::vector<uint32_t> buffer_sizes;
   char* arr_vals = nullptr;
   uint32_t* arr_sizes = nullptr;
   uint32_t num_rows_buffer = size_buffer_bytes / sizeof(uint32_t);
   uint32_t bytes_copiados = 0;
   try {
      arr_vals = new char[size_buffer_bytes];
      arr_sizes = new uint32_t[num_rows_buffer];

      // inicializamos una sinstancia de byteStreamer:
      byteStreamer byte_streamer;
      byte_streamer.inicio = inicio;
      byte_streamer.fin = fin;
      byte_streamer.char_idx = 0;
      byte_streamer.filas_escritas = 0;
      byte_streamer.is_unknown_type = false;
      
      while(byte_streamer.is_data_left()){
         bytes_copiados = byte_streamer.extraer_bytes(arr_vals, arr_sizes, size_buffer_bytes);

         out1.write(arr_vals, bytes_copiados);
         out1.flush();
         char* buffer_sizes_chars = reinterpret_cast<char*>(arr_sizes);
         out2.write(buffer_sizes_chars, byte_streamer.filas_escritas * sizeof(uint32_t));
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


void disk_out::escribir_particion_unknown(std::vector<Values>::iterator inicio, std::vector<Values>::iterator fin, std::string ruta_escritura){
   
   // NO nos hace falta el argumento int size_disponible_para_escribir como input.
   // Ya que inicio y fin son las filas a escribir en una partición.
   
   std::ofstream out1;
   std::ofstream out2;
   //std::vector<uint32_t> vec_sizes;

   out1.open(ruta_escritura + ".bin", std::ios::out | std::ios::binary | std::ios::app);
   out2.open(ruta_escritura + ".idx", std::ios::out | std::ios::binary | std::ios::app);
   Logger::log(LogLevel::OUTPUT, "La particion esta abierta");



   //std::vector<char> buffer_vals;
   //std::vector<uint32_t> buffer_sizes;
   char* arr_vals = nullptr;
   uint32_t* arr_sizes = nullptr;
   uint32_t num_rows_buffer = size_buffer_bytes / sizeof(uint32_t);
   uint32_t bytes_copiados = 0;
   try {
      arr_vals = new char[size_buffer_bytes];
      arr_sizes = new uint32_t[num_rows_buffer];

      // inicializamos una sinstancia de byteStreamer:
      byteStreamer byte_streamer;
      byte_streamer.inicio = inicio;
      byte_streamer.fin = fin;
      byte_streamer.char_idx = 0;
      byte_streamer.filas_escritas = 0;
      byte_streamer.is_unknown_type = true;
      while(byte_streamer.is_data_left()){
         bytes_copiados = byte_streamer.extraer_bytes(arr_vals, arr_sizes, size_buffer_bytes);

         out1.write(arr_vals, bytes_copiados);
         out1.flush();
         char* buffer_sizes_chars = reinterpret_cast<char*>(arr_sizes);
         out2.write(buffer_sizes_chars, byte_streamer.filas_escritas * sizeof(uint32_t));
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





void disk_out::bucle_escritura_int(std::vector<Values>*& vec_var_ptr, int bytes_totales_a_escribir, int partition_entities, std::string ruta_variable, std::string ultima_particion_str, int ultima_particion_int){

   // Estas son las variables que usaremos para trackear lo que llevamos avanzado
   int bytes_escritos = 0;
   int filas_escritas = 0;
   int filas_totales_a_escribir = bytes_totales_a_escribir / 4;
   int size_particion = partition_entities * sizeof(int);
   Logger::flush();

   Logger::log(LogLevel::DEBUG, "Entramos en la funcion de escritura de INTs");
   Logger::log(LogLevel::DEBUG, "Tamano de la particion: ", false, true);
   Logger::log(LogLevel::DEBUG, size_particion, false, true);

   // Ahora consultamos la última partición para saber realmente en que númerio estamos:
   std::vector<Values>::iterator inicio;
   std::vector<Values>::iterator fin;
   int filas_a_escribir;
   int bytes_a_escribir;
   int size_disponible_para_escribir = 0;

   int filas_que_quedan_por_escribir = 0;
   int bytes_que_quedan_por_escribir = 0;
   int filas_que_caben_en_particion_actual = 0;
   
   while(bytes_totales_a_escribir > bytes_escritos){
      /*
      1) Consultamos el tamaño que le queda a la partición por escribir
         Si no existiera, se asigna el tmaño de una partición entera
         (del tipo concreto)
      */
      size_disponible_para_escribir = disk_out::obtener_size_disponible(ruta_variable,
                                                                       ultima_particion_str + ".dat",
                                                                       size_particion
                                                                      );
      Logger::log(LogLevel::DEBUG, "El tamaño disponible en la particion es: ", false, true);
      Logger::log(LogLevel::DEBUG, size_disponible_para_escribir, true, false);
      // 1) Calculamos las filas a escribir:
      filas_que_quedan_por_escribir = filas_totales_a_escribir - filas_escritas;
      bytes_que_quedan_por_escribir = bytes_totales_a_escribir - bytes_escritos;
      filas_que_caben_en_particion_actual = size_disponible_para_escribir / sizeof(int);
      if(filas_que_quedan_por_escribir > filas_que_caben_en_particion_actual){
         filas_a_escribir = filas_que_caben_en_particion_actual;
         bytes_a_escribir = size_disponible_para_escribir;
      }else{
         filas_a_escribir = filas_que_quedan_por_escribir;
         bytes_a_escribir = bytes_que_quedan_por_escribir;
      };
      Logger::log(LogLevel::DEBUG, "Filas que quedan pro escribir: ", false, true);
      Logger::log(LogLevel::DEBUG, filas_a_escribir, true, false);
      if(filas_a_escribir > 0){
         // Sacamos los índices inicial y final;
         disk_out::obtener_indices_vector_valores(inicio,
                                                fin,
                                                vec_var_ptr,
                                                filas_escritas,
                                                filas_a_escribir
                                                );

         // 3) Realizamos la escritura
         std::string ruta_escritura = ruta_variable + ultima_particion_str;
         disk_out::escribir_particion_int(inicio, fin, ruta_escritura);
            // Una vez hecha la escritura, actualizamos los contadores
            // 4) Actualización de los contadores:
            bytes_escritos += bytes_a_escribir;
            filas_escritas += filas_a_escribir;
         };
         // Actualizamos el nombre de las particiones:
         Logger::log(LogLevel::DEBUG, "Valor de 'ultima_particion_str' antes: ", false, true);
         Logger::log(LogLevel::DEBUG, ultima_particion_str, true, false);
         ultima_particion_int += 1;
         ultima_particion_str = std::to_string(ultima_particion_int);
         Logger::log(LogLevel::DEBUG, "Valor de 'ultima_particion_str' despues: ", false, true);
         Logger::log(LogLevel::DEBUG, ultima_particion_str, true, false);
   };
};

void disk_out::bucle_escritura_float(std::vector<Values>*& vec_var_ptr, int bytes_totales_a_escribir, int partition_entities, std::string ruta_variable, std::string ultima_particion_str, int ultima_particion_int){

   // Estas son las variables que usaremos para trackear lo que llevamos avanzado
   int bytes_escritos = 0;
   int filas_escritas = 0;
   int filas_totales_a_escribir = bytes_totales_a_escribir / 4;
   int size_particion = partition_entities * sizeof(float);
   Logger::flush();

   Logger::log(LogLevel::DEBUG, "Entramos en la funcion de escritura de FLOATs");
   Logger::log(LogLevel::DEBUG, "Tamano de la particion: ", false, true);
   Logger::log(LogLevel::DEBUG, size_particion, false, true);

   // Ahora consultamos la última partición para saber realmente en que númerio estamos:
   std::vector<Values>::iterator inicio;
   std::vector<Values>::iterator fin;
   int filas_a_escribir;
   int bytes_a_escribir;
   int size_disponible_para_escribir = 0;

   int filas_que_quedan_por_escribir = 0;
   int bytes_que_quedan_por_escribir = 0;
   int filas_que_caben_en_particion_actual = 0;
   
   while(bytes_totales_a_escribir > bytes_escritos){
      /*
      1) Consultamos el tamaño que le queda a la partición por escribir
         Si no existiera, se asigna el tmaño de una partición entera
         (del tipo concreto)
      */
      size_disponible_para_escribir = disk_out::obtener_size_disponible(ruta_variable,
                                                                       ultima_particion_str + ".dat",
                                                                       size_particion
                                                                      );
      Logger::log(LogLevel::DEBUG, "El tamaño disponible en la particion es: ", false, true);
      Logger::log(LogLevel::DEBUG, size_disponible_para_escribir, true, false);
      // 1) Calculamos las filas a escribir:
      filas_que_quedan_por_escribir = filas_totales_a_escribir - filas_escritas;
      bytes_que_quedan_por_escribir = bytes_totales_a_escribir - bytes_escritos;
      filas_que_caben_en_particion_actual = size_disponible_para_escribir / sizeof(float);
      if(filas_que_quedan_por_escribir > filas_que_caben_en_particion_actual){
         filas_a_escribir = filas_que_caben_en_particion_actual;
         bytes_a_escribir = size_disponible_para_escribir;
      }else{
         filas_a_escribir = filas_que_quedan_por_escribir;
         bytes_a_escribir = bytes_que_quedan_por_escribir;
      };
      Logger::log(LogLevel::DEBUG, "Filas que quedan pro escribir: ", false, true);
      Logger::log(LogLevel::DEBUG, filas_a_escribir, true, false);
      if(filas_a_escribir > 0){
         // Sacamos los índices inicial y final;
         disk_out::obtener_indices_vector_valores(inicio,
                                                fin,
                                                vec_var_ptr,
                                                filas_escritas,
                                                filas_a_escribir
                                                );

         // 3) Realizamos la escritura
         std::string ruta_escritura = ruta_variable + ultima_particion_str;
         disk_out::escribir_particion_float(inicio, fin, ruta_escritura);
         // Una vez hecha la escritura, actualizamos los contadores
         // 4) Actualización de los contadores:
         bytes_escritos += bytes_a_escribir;
         filas_escritas += filas_a_escribir;
      };
      // Actualizamos el nombre de las particiones:
      Logger::log(LogLevel::DEBUG, "Valor de 'ultima_particion_str' antes: ", false, true);
      Logger::log(LogLevel::DEBUG, ultima_particion_str, true, false);
      ultima_particion_int += 1;
      ultima_particion_str = std::to_string(ultima_particion_int);
      Logger::log(LogLevel::DEBUG, "Valor de 'ultima_particion_str' despues: ", false, true);
      Logger::log(LogLevel::DEBUG, ultima_particion_str, true, false);
   };
};


void disk_out::bucle_escritura_bool(std::vector<Values>*& vec_var_ptr, int bytes_totales_a_escribir, int partition_entities, std::string ruta_variable, std::string ultima_particion_str, int ultima_particion_int){

   // Estas son las variables que usaremos para trackear lo que llevamos avanzado
   int bytes_escritos = 0;
   int filas_escritas = 0;
   int filas_totales_a_escribir = bytes_totales_a_escribir;
   int size_particion = partition_entities;
   Logger::flush();

   Logger::log(LogLevel::DEBUG, "Entramos en la funcion de escritura de BOOLs");
   Logger::log(LogLevel::DEBUG, "Tamano de la particion: ", false, true);
   Logger::log(LogLevel::DEBUG, size_particion, false, true);

   // Ahora consultamos la última partición para saber realmente en que númerio estamos:
   std::vector<Values>::iterator inicio;
   std::vector<Values>::iterator fin;
   int filas_a_escribir;
   int bytes_a_escribir;
   int size_disponible_para_escribir = 0;

   int filas_que_quedan_por_escribir = 0;
   int bytes_que_quedan_por_escribir = 0;
   int filas_que_caben_en_particion_actual = 0;
   
   while(bytes_totales_a_escribir > bytes_escritos){
      /*
      1) Consultamos el tamaño que le queda a la partición por escribir
         Si no existiera, se asigna el tmaño de una partición entera
         (del tipo concreto)
      */
      size_disponible_para_escribir = disk_out::obtener_size_disponible(ruta_variable,
                                                                       ultima_particion_str + ".dat",
                                                                       size_particion
                                                                      );
      Logger::log(LogLevel::DEBUG, "El tamaño disponible en la particion es: ", false, true);
      Logger::log(LogLevel::DEBUG, size_disponible_para_escribir, true, false);
      // 1) Calculamos las filas a escribir:
      filas_que_quedan_por_escribir = filas_totales_a_escribir - filas_escritas;
      bytes_que_quedan_por_escribir = bytes_totales_a_escribir - bytes_escritos;
      filas_que_caben_en_particion_actual = size_disponible_para_escribir / sizeof(uint8_t);
      if(filas_que_quedan_por_escribir > filas_que_caben_en_particion_actual){
         filas_a_escribir = filas_que_caben_en_particion_actual;
         bytes_a_escribir = size_disponible_para_escribir;
      }else{
         filas_a_escribir = filas_que_quedan_por_escribir;
         bytes_a_escribir = bytes_que_quedan_por_escribir;
      };
      Logger::log(LogLevel::DEBUG, "Filas que quedan pro escribir: ", false, true);
      Logger::log(LogLevel::DEBUG, filas_a_escribir, true, false);
      if(filas_a_escribir > 0){
         // Sacamos los índices inicial y final;
         disk_out::obtener_indices_vector_valores(inicio,
                                                fin,
                                                vec_var_ptr,
                                                filas_escritas,
                                                filas_a_escribir
                                                );

         // 3) Realizamos la escritura
         std::string ruta_escritura = ruta_variable + ultima_particion_str;
         disk_out::escribir_particion_bool(inicio, fin, ruta_escritura);
         // Una vez hecha la escritura, actualizamos los contadores
         // 4) Actualización de los contadores:
         bytes_escritos += bytes_a_escribir;
         filas_escritas += filas_a_escribir;
      };
      // Actualizamos el nombre de las particiones:
      Logger::log(LogLevel::DEBUG, "Valor de 'ultima_particion_str' antes: ", false, true);
      Logger::log(LogLevel::DEBUG, ultima_particion_str, true, false);
      ultima_particion_int += 1;
      ultima_particion_str = std::to_string(ultima_particion_int);
      Logger::log(LogLevel::DEBUG, "Valor de 'ultima_particion_str' despues: ", false, true);
      Logger::log(LogLevel::DEBUG, ultima_particion_str, true, false);
   };
};


void disk_out::bucle_escritura_string(std::vector<Values>*& vec_var_ptr, int bytes_totales_a_escribir, int partition_entities, std::string ruta_variable, std::string ultima_particion_str, int ultima_particion_int){

   // Estas son las variables que usaremos para trackear lo que llevamos avanzado
   int bytes_escritos = 0;
   int filas_escritas = 0;
   int filas_totales_a_escribir = bytes_totales_a_escribir / 4;
   int size_particion = partition_entities * sizeof(uint32_t);
   Logger::flush();

   Logger::log(LogLevel::DEBUG, "Entramos en la funcion de escritura de STRINGs");
   Logger::log(LogLevel::DEBUG, "Tamano de la particion: ", false, true);
   Logger::log(LogLevel::DEBUG, size_particion, false, true);

   // Ahora consultamos la última partición para saber realmente en que númerio estamos:
   std::vector<Values>::iterator inicio;
   std::vector<Values>::iterator fin;
   int filas_a_escribir;
   int bytes_a_escribir;
   int size_disponible_para_escribir = 0;

   int filas_que_quedan_por_escribir = 0;
   int bytes_que_quedan_por_escribir = 0;
   int filas_que_caben_en_particion_actual = 0;
   
   while(bytes_totales_a_escribir > bytes_escritos){
      /*
      1) Consultamos el tamaño que le queda a la partición por escribir
         Si no existiera, se asigna el tmaño de una partición entera
         (del tipo concreto)
      */
      size_disponible_para_escribir = disk_out::obtener_size_disponible(ruta_variable,
                                                                       ultima_particion_str + ".idx",
                                                                       size_particion
                                                                      );
      Logger::log(LogLevel::DEBUG, "El tamaño disponible en la particion es: ", false, true);
      Logger::log(LogLevel::DEBUG, size_disponible_para_escribir, true, false);
      // 1) Calculamos las filas a escribir:
      filas_que_quedan_por_escribir = filas_totales_a_escribir - filas_escritas;
      bytes_que_quedan_por_escribir = bytes_totales_a_escribir - bytes_escritos;
      filas_que_caben_en_particion_actual = size_disponible_para_escribir / sizeof(uint32_t);
      if(filas_que_quedan_por_escribir > filas_que_caben_en_particion_actual){
         filas_a_escribir = filas_que_caben_en_particion_actual;
         bytes_a_escribir = size_disponible_para_escribir;
      }else{
         filas_a_escribir = filas_que_quedan_por_escribir;
         bytes_a_escribir = bytes_que_quedan_por_escribir;
      };
      Logger::log(LogLevel::DEBUG, "Filas que quedan pro escribir: ", false, true);
      Logger::log(LogLevel::DEBUG, filas_a_escribir, true, false);
      if(filas_a_escribir > 0){
         // Sacamos los índices inicial y final;
         disk_out::obtener_indices_vector_valores(inicio,
                                                fin,
                                                vec_var_ptr,
                                                filas_escritas,
                                                filas_a_escribir
                                                );

         // 3) Realizamos la escritura
         std::string ruta_escritura = ruta_variable + ultima_particion_str;
         disk_out::escribir_particion_string(inicio, fin, ruta_escritura);
         // Una vez hecha la escritura, actualizamos los contadores
         // 4) Actualización de los contadores:
         bytes_escritos += bytes_a_escribir;
         filas_escritas += filas_a_escribir;
      };
      // Actualizamos el nombre de las particiones:
      Logger::log(LogLevel::DEBUG, "Valor de 'ultima_particion_str' antes: ", false, true);
      Logger::log(LogLevel::DEBUG, ultima_particion_str, true, false);
      ultima_particion_int += 1;
      ultima_particion_str = std::to_string(ultima_particion_int);
      Logger::log(LogLevel::DEBUG, "Valor de 'ultima_particion_str' despues: ", false, true);
      Logger::log(LogLevel::DEBUG, ultima_particion_str, true, false);
   };
};



void disk_out::bucle_escritura_unknown(std::vector<Values>*& vec_var_ptr, int bytes_totales_a_escribir, int partition_entities, std::string ruta_variable, std::string ultima_particion_str, int ultima_particion_int){

   // Estas son las variables que usaremos para trackear lo que llevamos avanzado
   int bytes_escritos = 0;
   int filas_escritas = 0;
   int filas_totales_a_escribir = bytes_totales_a_escribir / 4;
   int size_particion = partition_entities * sizeof(uint32_t);
   Logger::flush();

   Logger::log(LogLevel::DEBUG, "Entramos en la funcion de escritura de UNKNOWNs");
   Logger::log(LogLevel::DEBUG, "Tamano de la particion: ", false, true);
   Logger::log(LogLevel::DEBUG, size_particion, false, true);

   // Ahora consultamos la última partición para saber realmente en que númerio estamos:
   std::vector<Values>::iterator inicio;
   std::vector<Values>::iterator fin;
   int filas_a_escribir;
   int bytes_a_escribir;
   int size_disponible_para_escribir = 0;

   int filas_que_quedan_por_escribir = 0;
   int bytes_que_quedan_por_escribir = 0;
   int filas_que_caben_en_particion_actual = 0;
   
   while(bytes_totales_a_escribir > bytes_escritos){
      /*
      1) Consultamos el tamaño que le queda a la partición por escribir
         Si no existiera, se asigna el tmaño de una partición entera
         (del tipo concreto)
      */
      size_disponible_para_escribir = disk_out::obtener_size_disponible(ruta_variable,
                                                                       ultima_particion_str + ".idx",
                                                                       size_particion
                                                                      );
      Logger::log(LogLevel::DEBUG, "El tamaño disponible en la particion es: ", false, true);
      Logger::log(LogLevel::DEBUG, size_disponible_para_escribir, true, false);
      // 1) Calculamos las filas a escribir:
      filas_que_quedan_por_escribir = filas_totales_a_escribir - filas_escritas;
      bytes_que_quedan_por_escribir = bytes_totales_a_escribir - bytes_escritos;
      filas_que_caben_en_particion_actual = size_disponible_para_escribir / sizeof(uint32_t);
      if(filas_que_quedan_por_escribir > filas_que_caben_en_particion_actual){
         filas_a_escribir = filas_que_caben_en_particion_actual;
         bytes_a_escribir = size_disponible_para_escribir;
      }else{
         filas_a_escribir = filas_que_quedan_por_escribir;
         bytes_a_escribir = bytes_que_quedan_por_escribir;
      };
      Logger::log(LogLevel::DEBUG, "Filas que quedan pro escribir: ", false, true);
      Logger::log(LogLevel::DEBUG, filas_a_escribir, true, false);
      if(filas_a_escribir > 0){
         // Sacamos los índices inicial y final;
         disk_out::obtener_indices_vector_valores(inicio,
                                                fin,
                                                vec_var_ptr,
                                                filas_escritas,
                                                filas_a_escribir
                                                );

         // 3) Realizamos la escritura
         std::string ruta_escritura = ruta_variable + ultima_particion_str;
         disk_out::escribir_particion_unknown(inicio, fin, ruta_escritura);
         // Una vez hecha la escritura, actualizamos los contadores
         // 4) Actualización de los contadores:
         bytes_escritos += bytes_a_escribir;
         filas_escritas += filas_a_escribir;
      };
      // Actualizamos el nombre de las particiones:
      Logger::log(LogLevel::DEBUG, "Valor de 'ultima_particion_str' antes: ", false, true);
      Logger::log(LogLevel::DEBUG, ultima_particion_str, true, false);
      ultima_particion_int += 1;
      ultima_particion_str = std::to_string(ultima_particion_int);
      Logger::log(LogLevel::DEBUG, "Valor de 'ultima_particion_str' despues: ", false, true);
      Logger::log(LogLevel::DEBUG, ultima_particion_str, true, false);
   };
};




void disk_out::write_table_data(table* tabla){

   Logger::log(LogLevel::DEBUG, "<<<ENTRAMOS EN LA FUNCION DE ESCRITURA POR PARTICIONES>>>>");

   // Recuperamos los tipos de datos:
   std::vector<dataType> tipo_columnas = tabla->metadata_ptr->column_types;
   // Recuperamos el nombre de la columna:
   std::vector<std::string> nombre_columnas = tabla->metadata_ptr->column_names;
   // Recuperamos el vector de vectores de los datos:
   std::map<std::string, std::vector<Values>> vec_de_vec_vals = tabla->data_ptr->columns;
   // Recuperamos el nombre de la tabla:
   std::string nombre_tabla = tabla->metadata_ptr->name;
   Logger::log(LogLevel::DEBUG, "Valores preliminares recuperados");

   // Antes de nada, creamos el directorio de la tabla:
   std::filesystem::path ruta_tabla = "data/" + nombre_tabla;
   std::filesystem::create_directories(ruta_tabla);
   Logger::log(LogLevel::DEBUG, "Directorio de la tabla creado");

   // Ahora, examinamos cuantas particiones hay:
   std::vector<std::string> particiones_nombres;
   particiones_nombres = disk_aux::obtener_archivos_en_ruta("data/" + nombre_tabla + "/" + nombre_columnas[0], tipo_columnas[0]);
   std::string last_partition = "";
   //////////////////////////////////////////////////////////////////////
   // Obtenemos la última partición:
   std::string ultima_particion_str = "0";
   int ultima_particion_int = 0;


   if(!particiones_nombres.empty()){
      Logger::log(LogLevel::DEBUG, "Hacemos bubble sort");
      part_sort::bubble_sort(particiones_nombres);
      Logger::log(LogLevel::DEBUG, "Bubble sort realizado con exito");
      int parts = particiones_nombres.size();
      last_partition = particiones_nombres[parts-1];
      if(last_partition != ""){
         Logger::log(LogLevel::DEBUG, "La ultima particion no es '' ");
         Logger::log(LogLevel::DEBUG, "El valor de 'last_partition' es: ", false, true);
         Logger::log(LogLevel::DEBUG, last_partition, true, false);
         ultima_particion_str = part_sort::procesar_numero_particion_como_str(last_partition);
         Logger::log(LogLevel::DEBUG, "Valor de 'ultima_particion_str': ", false, true);
         Logger::log(LogLevel::DEBUG, ultima_particion_str, false, true);
         ultima_particion_int = std::stoi(ultima_particion_str);
      };
   };

   Logger::log(LogLevel::OUTPUT, "Encontrada la ultima particion: ", false, true);
   Logger::log(LogLevel::OUTPUT, ultima_particion_str, true, false);
   // Ahora obtenemos el ultimo numero en forma de string y entero, para hacerlo sólo una vez:


   // VARIABLES NECESARIAS ANTES DEL BUCLE:
   uint32_t num_cols = tabla->metadata_ptr->n_cols;
   int bytes_totales_a_escribir = 0; // Cantidad que nos servirá poara ver cuánto tenemos que escribir:
   int partition_entities = 3;

   // Iteramos por cada elemento
   for(int i = 0; i<num_cols; i++){
      // Iteramos por cada columna:
      dataType& tipo_dato = tipo_columnas[i];
      std::string& column_name = nombre_columnas[i];
      std::vector<Values>* vec_var_ptr = &(vec_de_vec_vals.at(column_name));
      // Creamos el directorio de cada varuiable de la tabla:
      std::string ruta_columna = "data/" + nombre_tabla + "/" + column_name;
      std::filesystem::create_directories(ruta_columna);
      std::string ruta_variable = "data/" + nombre_tabla + "/" + column_name + "/" + "part_";

      switch(tipo_dato){
         case dataType::INT: {
            bytes_totales_a_escribir = vec_var_ptr->size() * 4; //  Tamaño total de lo que hay que escribir

            disk_out::bucle_escritura_int(vec_var_ptr, bytes_totales_a_escribir, partition_entities, ruta_variable, ultima_particion_str, ultima_particion_int);
            break;
         };
         case dataType::FLOAT: {
            bytes_totales_a_escribir = vec_var_ptr->size() * 4; //  Tamaño total de lo que hay que escribir

            disk_out::bucle_escritura_float(vec_var_ptr, bytes_totales_a_escribir, partition_entities, ruta_variable, ultima_particion_str, ultima_particion_int);
            break;
         };
         case dataType::BOOL: {
            bytes_totales_a_escribir = vec_var_ptr->size();
            disk_out::bucle_escritura_bool(vec_var_ptr, bytes_totales_a_escribir, partition_entities, ruta_variable, ultima_particion_str, ultima_particion_int);
            break;
         };
         case dataType::STRING: {
            bytes_totales_a_escribir = vec_var_ptr->size() * 4;
            disk_out::bucle_escritura_string(vec_var_ptr, bytes_totales_a_escribir, partition_entities, ruta_variable, ultima_particion_str, ultima_particion_int);
            break;
         };
         case dataType::UNKNOWN: {
            bytes_totales_a_escribir = vec_var_ptr->size() * 4;
            disk_out::bucle_escritura_unknown(vec_var_ptr, bytes_totales_a_escribir, partition_entities, ruta_variable, ultima_particion_str, ultima_particion_int);
            break;
         };

      };
   };
};