

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
#include "disk_in.h"


////////////////////////////////////////////////////////////////////
// LECTURA DE DATOS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void disk_in::read_fixed_len_int_columns(std::string path_var, std::string partition_current, std::vector<Values>& vec_vals){

   Logger::log(LogLevel::DEBUG, "Dentro de la lectura de los INT");
   std::ifstream in;
   std::string ruta = path_var + partition_current + ".dat";
   Logger::log(LogLevel::DEBUG, "Abrimos el archivo de datos: ", false, true);
   Logger::log(LogLevel::DEBUG, ruta, true, false);
   in.open(ruta, std::ios::in | std::ios::binary);
   if (!in.is_open()) {
      Logger::log(LogLevel::ERROR, "¡No se pudo abrir el archivo! Ruta: ", false, true);
      Logger::log(LogLevel::ERROR, ruta, true, false);
      return; // Sal de la función si no hay archivo
   }else{
      Logger::log(LogLevel::DEBUG, "Archivo abierto en modo lectura");
   };








   // Hallamos el tamaño en bytes del archivo/partición:
   uint32_t file_size = disk_aux::return_file_size_bytes(in);
   uint32_t bytes_puntero = 0;
   uint32_t bytes_a_leer = 0;
   
   // Reserbvamos el buffer:
   //std::vector<char> buffer;
   char* buffer = nullptr;
   try {
      buffer = new char[size_buffer_bytes];

      Logger::log(LogLevel::DEBUG, "Se ha recuperado el tamaño del archivo");
      Logger::log(LogLevel::DEBUG, "El tamano del archivo es:", false, true);
      Logger::log(LogLevel::DEBUG, file_size, true, false);

      if(file_size > 0){
         Logger::log(LogLevel::DEBUG, "El archivo NO esta vacio");
         while(bytes_puntero < file_size){
            Logger::log(LogLevel::DEBUG, "Iteracion del bucle del buffer");
            Logger::log(LogLevel::DEBUG, "Calculamos los bytes a leer");
            // Primero calculamos los bytes a leer:
            if(bytes_puntero + size_buffer_bytes >= file_size){
               bytes_a_leer = size_buffer_bytes;
            }else{
               bytes_a_leer = file_size - bytes_puntero;
            };
            Logger::log(LogLevel::DEBUG, "Reservamos esos bytes en el buffer");
            // Ya sabemos los bytes a leer, por lo que los reservamos:
            // Realizamos la lectura:
            Logger::log(LogLevel::DEBUG, "Leemos: cargamos los datos al buffer");
            in.read(buffer, bytes_a_leer);
            Logger::log(LogLevel::DEBUG, "Lectura del buffer realizada con exito");
            Logger::log(LogLevel::DEBUG, "Nos disponemos a rellenar esos valores:");
            // Ahora los insertamos en el vector:
            disk_aux::fill_vector_int(bytes_a_leer / sizeof(int), buffer, vec_vals);
            Logger::log(LogLevel::DEBUG, "Valores añadidos a la tabla");
            // Actualizamos el puntero con los bytes leídos:
            bytes_puntero += bytes_a_leer;
         };
      }else{
         Logger::log(LogLevel::DEBUG, "El archivo SI esta vacio");
      };
   }catch(...){
      delete[] buffer;
      in.close();
      throw;
   };
   in.close();
   delete[] buffer;
};


void disk_in::read_fixed_len_float_columns(std::string path_var, std::string partition_current, std::vector<Values>& vec_vals){
   
   Logger::log(LogLevel::DEBUG, "Dentro de la lectura de los FLOAT");
   std::ifstream in;
   std::string ruta = path_var + partition_current + ".dat";
   Logger::log(LogLevel::DEBUG, "Abrimos el archivo de datos: ", false, true);
   Logger::log(LogLevel::DEBUG, ruta, true, false);
   in.open(ruta, std::ios::in | std::ios::binary);
   if (!in.is_open()) {
      Logger::log(LogLevel::ERROR, "¡No se pudo abrir el archivo! Ruta: ", false, true);
      Logger::log(LogLevel::ERROR, ruta, true, false);
      return; // Sal de la función si no hay archivo
   }else{
      Logger::log(LogLevel::DEBUG, "Archivo abierto en modo lectura");
   };

   // Hallamos el tamaño en bytes del archivo/partición:
   uint32_t file_size = disk_aux::return_file_size_bytes(in);
   uint32_t bytes_puntero = 0;
   uint32_t bytes_a_leer = 0;
   
   // Reserbvamos el buffer:
   //std::vector<char> buffer;
   char* buffer = nullptr;
   try {
      buffer = new char[size_buffer_bytes];
      Logger::log(LogLevel::DEBUG, "Se ha recuperado el tamaño del archivo");
      Logger::log(LogLevel::DEBUG, "El tamano del archivo es:", false, true);
      Logger::log(LogLevel::DEBUG, file_size, true, false);

      if(file_size > 0){
         Logger::log(LogLevel::DEBUG, "El archivo NO esta vacio");
         while(bytes_puntero < file_size){
            Logger::log(LogLevel::DEBUG, "Iteracion del bucle del buffer");
            //buffer.clear();
            Logger::log(LogLevel::DEBUG, "Calculamos los bytes a leer");
            // Primero calculamos los bytes a leer:
            if(bytes_puntero + size_buffer_bytes >= file_size){
               bytes_a_leer = size_buffer_bytes;
            }else{
               bytes_a_leer = file_size - bytes_puntero;
            };
            Logger::log(LogLevel::DEBUG, "Reservamos esos bytes en el buffer");
            // Ya sabemos los bytes a leer, por lo que los reservamos:
            //buffer.reserve(bytes_a_leer);
            // Realizamos la lectura:
            Logger::log(LogLevel::DEBUG, "Leemos: cargamos los datos al buffer");
            in.read(buffer, bytes_a_leer);
            Logger::log(LogLevel::DEBUG, "Lectura del buffer realizada con exito");
            Logger::log(LogLevel::DEBUG, "Nos disponemos a rellenar esos valores:");
            // Ahora los insertamos en el vector:
            disk_aux::fill_vector_float(bytes_a_leer / sizeof(float), buffer, vec_vals);
            Logger::log(LogLevel::DEBUG, "Valores añadidos a la tabla");
            // Actualizamos el puntero con los bytes leídos:
            bytes_puntero += bytes_a_leer;
         };
      }else{
         Logger::log(LogLevel::DEBUG, "El archivo SI esta vacio");
      };
   }catch(...){
      delete[] buffer;
      in.close();
      throw;
   };
   in.close();
   delete[] buffer;
};


void disk_in::read_fixed_len_bool_columns(std::string path_var, std::string partition_current, std::vector<Values>& vec_vals){
   
   Logger::log(LogLevel::DEBUG, "Dentro de la lectura de los BOOLs");
   std::ifstream in;
   std::string ruta = path_var + partition_current + ".dat";
   Logger::log(LogLevel::DEBUG, "Abrimos el archivo de datos: ", false, true);
   Logger::log(LogLevel::DEBUG, ruta, true, false);
   in.open(ruta, std::ios::in | std::ios::binary);
   if (!in.is_open()) {
      Logger::log(LogLevel::ERROR, "¡No se pudo abrir el archivo! Ruta: ", false, true);
      Logger::log(LogLevel::ERROR, ruta, true, false);
      return; // Sal de la función si no hay archivo
   }else{
      Logger::log(LogLevel::DEBUG, "Archivo abierto en modo lectura");
   };

   // Hallamos el tamaño en bytes del archivo/partición:
   uint32_t file_size = disk_aux::return_file_size_bytes(in);
   uint32_t bytes_puntero = 0;
   uint32_t bytes_a_leer = 0;
   
   // Reserbvamos el buffer:
   //std::vector<char> buffer;
   char* buffer = nullptr;
   try {
      buffer = new char[size_buffer_bytes];
      Logger::log(LogLevel::DEBUG, "Se ha recuperado el tamaño del archivo");
      Logger::log(LogLevel::DEBUG, "El tamano del archivo es:", false, true);
      Logger::log(LogLevel::DEBUG, file_size, true, false);

      if(file_size > 0){
         Logger::log(LogLevel::DEBUG, "El archivo NO esta vacio");
         while(bytes_puntero < file_size){
            Logger::log(LogLevel::DEBUG, "Iteracion del bucle del buffer");
            //buffer.clear();
            Logger::log(LogLevel::DEBUG, "Calculamos los bytes a leer");
            // Primero calculamos los bytes a leer:
            if(bytes_puntero + size_buffer_bytes >= file_size){
               bytes_a_leer = size_buffer_bytes;
            }else{
               bytes_a_leer = file_size - bytes_puntero;
            };
            Logger::log(LogLevel::DEBUG, "Reservamos esos bytes en el buffer");
            // Ya sabemos los bytes a leer, por lo que los reservamos:
            //buffer.reserve(bytes_a_leer);
            // Realizamos la lectura:
            Logger::log(LogLevel::DEBUG, "Leemos: cargamos los datos al buffer");
            in.read(buffer, bytes_a_leer);
            Logger::log(LogLevel::DEBUG, "Lectura del buffer realizada con exito");
            Logger::log(LogLevel::DEBUG, "Nos disponemos a rellenar esos valores:");
            // Ahora los insertamos en el vector:
            disk_aux::fill_vector_bool(bytes_a_leer / sizeof(uint8_t), buffer, vec_vals);
            Logger::log(LogLevel::DEBUG, "Valores añadidos a la tabla");
            // Actualizamos el puntero con los bytes leídos:
            bytes_puntero += bytes_a_leer;
         };
      }else{
         Logger::log(LogLevel::DEBUG, "El archivo SI esta vacio");
      };
   }catch(...){
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

class stringReader_v2{
   /*
   Clase que nos abstraerá por completo de la lectura de strings.
   */
   std::vector<Values>& vec_vals;
   public:
      // Variable auxiliar para ver si es una std::string o un vector de caracteres:
      bool is_unknown_type = false; // false = std::string, true = std::vector<char>
      // Variables como input y de control:
      std::string path_var;
      std::string partition_current;
      uint8_t state = 0;

      // Valores predefindos:

      // Contadores de archivo:
      uint32_t file_idx_ptr = 0;
      uint32_t file_buffer_idx_size = 0;
      uint32_t file_idx_size = 0;

      uint32_t file_str_ptr = 0;
      uint32_t file_buffer_str_size = 0;
      uint32_t file_str_size = 0;

      // Contadores de buffer:
      char* buffer_idx_ptr_ini;
      char* buffer_idx_ptr_fin;
      uint32_t buffer_idx_bytes_to_read_each_time = 4; // Siempre se leera de 4 en 4 bytes
      uint32_t buffer_idx_read_bytes_tmp = 0;
      uint32_t buffer_idx_read_bytes_total = 0;

      char* buffer_str_ptr_ini;
      char* buffer_str_ptr_fin;
      uint32_t buffer_str_read_bytes_tmp = 0; // Esta valor dependera del tamano de la string o lo que quede por leer del buffer. Lo que sea más pequeño de las dos
      uint32_t buffer_str_read_bytes_total = 0;

      // Contadores de la lectura de strings:
      uint32_t tam_string_tmp = 0;
      bool offset = false;


      // Lectura:
      std::ifstream in_str;
      std::ifstream in_idx;

      //std::vector<char> buffer_str;
      char* buffer_str = nullptr;
      //std::vector<char> buffer_idx;
      char* buffer_idx = nullptr;


      uint32_t bytes_a_leer = 0;

;

      stringReader_v2(std::vector<Values>& vector_variable_variantes, std::string path_variable, std::string particion_actual, bool is_var_unknown) 
        : vec_vals(vector_variable_variantes), path_var(path_variable),  partition_current(particion_actual), is_unknown_type(is_var_unknown)// Inicialización de la referencia y strings para las rutas
      {
         this->buffer_str = new char[size_buffer_bytes];
         this->buffer_idx = new char[size_buffer_bytes];
      };

      ~stringReader_v2() {
         if(this->in_str.is_open()){
            this->in_str.close();
         };
         if(this->in_idx.is_open()){
            this->in_idx.close();
         };

         if(this->buffer_str){
               delete[] this->buffer_str;
               this->buffer_str = nullptr;
         };
            if(this->buffer_idx){
               delete[] this->buffer_idx;
               this->buffer_idx = nullptr;
         };
      };

      bool condition_eof_str_partition(){
         //return this->file_str_ptr +  this->file_buffer_str_size > this->file_str_size;
         return this->file_str_ptr > this->file_str_size;
      };

      bool condition_eof_idx_partition(){
         // return this->file_idx_ptr + this->file_buffer_idx_size > this->file_idx_size;
         return this->file_idx_ptr > this->file_idx_size;
      };

      bool condicion_eof_buffer_str(){
         //return this-> buffer_str_ptr_ini + this->buffer_str_read_bytes_total > this->buffer_str_ptr_fin;
         //return this-> buffer_str_ptr_ini + this->buffer_str_read_bytes_tmp > this->buffer_str_ptr_fin;
         return this->buffer_str_read_bytes_tmp >= this->file_buffer_str_size;

      };

      bool condicion_eof_buffer_idx(){
         //return this-> buffer_idx_ptr_ini + this->buffer_idx_read_bytes_total > this->buffer_idx_ptr_fin;
         //return this-> buffer_idx_ptr_ini + this->buffer_idx_read_bytes_tmp > this->buffer_idx_ptr_fin;
         return this->buffer_idx_read_bytes_tmp >= this->file_buffer_idx_size;
      };

      // Funciones auxiliares de calculo del tamño del buffer:
      void calcular_tamano_buffer_str(){
         this->file_buffer_str_size = size_buffer_bytes;
         if(this->file_str_ptr + size_buffer_bytes > this->file_str_size){
            this->file_buffer_str_size = this->file_str_size - this->file_str_ptr;
         };
      };

      void calcular_tamano_buffer_idx(){
         this->file_buffer_idx_size = size_buffer_bytes;
         if(this->file_idx_ptr + size_buffer_bytes > this->file_idx_size){
            this->file_buffer_idx_size = this->file_idx_size - this->file_idx_ptr;
         };
      };

      // Funciones axuliares de lectra de datos del disco al buffer:
      void read_str_buffer(){
         // Limpiamos la memoria:
         //this->buffer_str.clear();

         // Calculamos el tamano del buffer:
         this->calcular_tamano_buffer_str();

         // reservamos memoria:
         //this->buffer_str.resize(this->file_buffer_str_size);

         // Leemos las strings:
         this->in_str.read(this->buffer_str, this->file_buffer_str_size);

         // Actualizamos el puntero de lectura:
         this->file_str_ptr += this->file_buffer_str_size;

         // Reinicamos contadores:
         this->buffer_str_read_bytes_tmp = 0;
   
         // Fijamos los punteros:
         this->buffer_str_ptr_ini = this->buffer_str;
         //this->buffer_str_ptr_fin = buffer_str_ptr_ini + this->buffer_str.size();
         this->buffer_str_ptr_fin = buffer_str_ptr_ini + this->file_buffer_str_size;
      };

      void read_idx_buffer(){
         // Limpiamos la memoria:
         //this->buffer_idx.clear();

         // Calculamos el tamano del buffer:
         this->calcular_tamano_buffer_idx();

         // reservamos memoria:
         //this->buffer_idx.resize(this->file_buffer_idx_size);

         // Leemos las strings:
         this->in_idx.read(this->buffer_idx, this->file_buffer_idx_size);

         // Actualizamos el puntero de lectura:
         this->file_idx_ptr += this->file_buffer_idx_size;

         // Reiniciamos contadores:
         this->buffer_idx_read_bytes_tmp = 0;

         // Fijamos los punteros:
         this->buffer_idx_ptr_ini = this->buffer_idx;
         //this->buffer_idx_ptr_fin = buffer_idx_ptr_ini + this->buffer_idx.size();
         this->buffer_idx_ptr_fin = buffer_idx_ptr_ini + this->file_buffer_idx_size;
      };


      void control_unit(){
         /*
         Unidad de control para la lectura y
         cargado de los datos en la tabla
         */

         switch(this->state){
            case 0:{
               // CASO ESPECIAL
               //Logger::log(LogLevel::DEBUG, "ESTADO 0: (setup inicial)");
               // Este estado no se volverá a repetir:
               std::string ruta_str = this->path_var + this->partition_current + ".bin";
               std::string ruta_idx = this->path_var + this->partition_current + ".idx";
               this->in_str.open(ruta_str, std::ios::in | std::ios::binary);
               this->in_idx.open(ruta_idx, std::ios::in | std::ios::binary);

               // Recuperamos el tamano de los archivos:
               this->file_str_size = disk_aux::return_file_size_bytes(in_str);
               this->file_idx_size = disk_aux::return_file_size_bytes(in_idx);
               //Logger::log(LogLevel::DEBUG, "  Eñl archivo de strings pesa: ", false, true);
               //Logger::log(LogLevel::DEBUG, this->file_str_size, true, false);
               //Logger::log(LogLevel::DEBUG, "  Eñl archivo de indices pesa: ", false, true);
               //Logger::log(LogLevel::DEBUG, this->file_idx_size, true, false);

               if(this->file_str_size > 0 && this->file_idx_size > 0){
                  this->state = 1;
                  break;
               };
               this->state = 255;
               break;
            };
            case 1:{
               // CASO ESPECIAL
               // Este estad otambiñén es espacial. Solo se leerán las dos a la vez sólo la primera vez:
               //Logger::log(LogLevel::DEBUG, "ESTADO 1: (Lectura buffers inicial)");
               //Logger::log(LogLevel::DEBUG, "  Lectura por primera vez del archivo de strings");
               // Leemos el buffer de strings:
               this->read_str_buffer();
               // Leemos el buffer de idx:
               //Logger::log(LogLevel::DEBUG, "  Lectura por primera vez del archivo de indices");
               this->read_idx_buffer();
               this->state = 4; // Saltamos a ver el tamano de la string
               break;
            };
            case 2:{
               //Logger::log(LogLevel::DEBUG, "ESTADO 2: (LECTURA INTELIGENTE)");
               //Logger::log(LogLevel::DEBUG, "  Recarga de buffers con comprobaciones:");
               bool eof_buffer_str = false;
               bool eof_buffer_idx = false;
               bool eof_file_str = false;
               bool eof_file_idx = false;

               // Ahora realizamos la lectura de los indices:
               if(this->condition_eof_idx_partition()){
                  eof_file_idx = true;
                  if(this->condicion_eof_buffer_idx()){
                     eof_buffer_idx = true;
                  };
               }else{
                  // Solo si  hemos agotado el buffer leemos:
                  if(this->condicion_eof_buffer_idx()){
                     eof_buffer_idx = true;
                     this->read_idx_buffer();
                  };
               };

               // Ahora realizamos la lectura de las cadenas de texto
               if(this->condition_eof_str_partition()){
                  eof_file_str = true;
                  if(this->condicion_eof_buffer_str()){
                     eof_buffer_str = true;
                  };
               }else{
                  // Solo si  hemos agotado el buffer leemos:
                  if(this->condicion_eof_buffer_str()){
                     eof_buffer_str = true;
                     this->read_str_buffer();
                  };
               };

               // Ahora en función de los condicionales decidimos a dónde ir:
               if(eof_file_idx && eof_buffer_idx && eof_file_str && eof_buffer_str){
                  this->state = 255;
                  break;
               };
               // Si no hemos llegado al final, podemos seguir leyendo:
               this->state = 3; // Offset checker
               break;
            };
            case 3: {
               //Logger::log(LogLevel::DEBUG, "ESTADO 3: (Offset checker)");
               if(this->offset){
                  // La string la arrastramos de antes
                  this->state = 5;
                  break;
               }else{
                  // La string es nueva
                  this->state = 4;
                  break;
               };
            };
            case 4:{
               // LECTURA TAMONO STRING
               //Logger::log(LogLevel::DEBUG, "ESTADO 4: (Lectura tamano string)");
               //Logger::log(LogLevel::DEBUG, "  Lectura del tamano de la string");
               // Leemos el tamano de la string:
               
               std::memcpy(&this->tam_string_tmp,
                  this->buffer_idx_ptr_ini + this->buffer_idx_read_bytes_tmp,
                  this->buffer_idx_bytes_to_read_each_time
               );
               //Logger::log(LogLevel::DEBUG, "  Tamano de la string: ", false, true);
               //Logger::log(LogLevel::DEBUG, this->tam_string_tmp, true, false);
               // Avanzamos el puntero al la siguiente fila:
               this->buffer_idx_read_bytes_total += this->buffer_idx_bytes_to_read_each_time;
               this->buffer_idx_read_bytes_tmp += this->buffer_idx_bytes_to_read_each_time;
               this->state = 5; // Saltamos a leer la string
               break;
            };
            case 5:{
               //Logger::log(LogLevel::DEBUG, "ESTADO 5: (Lectura de la STRING)");
               // Antes evaluamos si se ha llegado al EOF de los buffers:
               // Ahora calculamos los bytes a leer de la string:
               //this->bytes_a_leer = this->file_buffer_str_size;
               uint32_t bytes_disponibles_ram = this->file_buffer_str_size - this->buffer_str_read_bytes_tmp;
               this->bytes_a_leer = this->tam_string_tmp;
               if(this->bytes_a_leer > bytes_disponibles_ram){
                  this->bytes_a_leer = bytes_disponibles_ram;
               };
               //Logger::log(LogLevel::DEBUG, " Los bytes a leer son: ", false, true);
               //Logger::log(LogLevel::DEBUG, this->bytes_a_leer, true, false);
               // Ya hemos leído lo necesario
               /*
               Este estado es de una lectura de la string.
               Si la string es mas larga, es un intercambio entre el estado 2 y 5
               */
               // std::string& tmp_str; // String tempral que vendrá de una ya existente u otra nueva:

               const char* origen_bytes = this->buffer_str_ptr_ini + this->buffer_str_read_bytes_tmp;
               if(offset){
                  if(is_unknown_type){
                     std::vector<char>& tmp_vec = std::get<std::vector<char>>(vec_vals.back());
                     tmp_vec.insert(tmp_vec.end(),
                     origen_bytes,
                     origen_bytes + this->bytes_a_leer);
                  }else{
                     // No es una string nueva
                     std::string& tmp_str = std::get<std::string>(vec_vals.back());
                     tmp_str.append(
                        origen_bytes,
                        this->bytes_a_leer
                     );
                     //Logger::log(LogLevel::DEBUG, " La string leida es: ", false, true);
                     //Logger::log(LogLevel::DEBUG, tmp_str, true, false);
                  };
               }else{
                  if(is_unknown_type){
                     std::vector<char> tmp_vec(origen_bytes, origen_bytes + this->bytes_a_leer);
                     vec_vals.push_back(std::move(tmp_vec));
                  }else{
                     // String nueva
                     std::string tmp_str;
                     tmp_str.append(
                        origen_bytes,
                        this->bytes_a_leer
                     );
                     vec_vals.push_back(std::move(tmp_str));
                     //Logger::log(LogLevel::DEBUG, " La string leida es: ", false, true);
                     //Logger::log(LogLevel::DEBUG, tmp_str, true, false);
                  };
               };

               // Actualizamos los bytes leidos:
               //Logger::log(LogLevel::DEBUG, " 'buffer_str_read_bytes_tmp' antes: ", false, true);
               //Logger::log(LogLevel::DEBUG, this->buffer_str_read_bytes_tmp, true, false);
               this->buffer_str_read_bytes_tmp += this->bytes_a_leer;
               //Logger::log(LogLevel::DEBUG, " 'buffer_str_read_bytes_tmp' despues: ", false, true);
               //Logger::log(LogLevel::DEBUG, this->buffer_str_read_bytes_tmp, true, false);   

               //Logger::log(LogLevel::DEBUG, " 'buffer_str_read_bytes_total' antes: ", false, true);
               //Logger::log(LogLevel::DEBUG, this->buffer_str_read_bytes_total, true, false);   
               this->buffer_str_read_bytes_total += this->bytes_a_leer;
               //Logger::log(LogLevel::DEBUG, " 'buffer_str_read_bytes_total' despues: ", false, true);
               //Logger::log(LogLevel::DEBUG, this->buffer_str_read_bytes_total, true, false);  

               //Logger::log(LogLevel::DEBUG, " 'tam_string_tmp' antes: ", false, true);
               //Logger::log(LogLevel::DEBUG, this->tam_string_tmp, true, false);   
               // Actualizamos el tamano de la string que queda por leer:
               this->tam_string_tmp -= this->bytes_a_leer;
               //Logger::log(LogLevel::DEBUG, " 'tam_string_tmp' despues: ", false, true);
               //Logger::log(LogLevel::DEBUG, this->tam_string_tmp, true, false);   

               //Logger::log(LogLevel::DEBUG, " 'offset' antes: ", false, true);
               //Logger::log(LogLevel::DEBUG, this->offset, true, false);   
               // Ahroa vemos si ay offset o no:
               if(this->tam_string_tmp > 0){
                  // No se ha leido entera:
                  this->offset = true;
               }else{
                  // Se ha leido entera:
                  this->offset = false;
               };
               //Logger::log(LogLevel::DEBUG, " 'offset' despues: ", false, true);
               //Logger::log(LogLevel::DEBUG, this->offset, true, false);   
               
               // SIempre vamos a la lectura inteligente:
               this->state = 2;
               break;
            };
            case 255: {
               //Logger::log(LogLevel::DEBUG, "ESTADO 2555: (ESTADO FINAL)");
               break;
            };
         ///////////////////////////////////////////////////////////////
         ///////////////////////////////////////////////////////////////
         ///////////////////////////////////////////////////////////////



         }; // Switch for control unit ends
      }; // Termina la CU de la FSM

      void execute_fsm(){
         Logger::log(LogLevel::DEBUG, "Dentro de la lectura de las STRINGs");
         bool aux_bool = true;
         //uint32_t contador_aux = 0;
         while(this->state != 255 && aux_bool){
            //if(contador_aux == 1000000){
               //break;
            //};
            if(this->state == 255){
               aux_bool = false;
            };
            this->control_unit();
            //++contador_aux;
         };
      };
};






disk_in::read_table_iterator::read_table_iterator(table* tabla){
   /*
   Método constructor en el cual:
   Leemos y registramos lo necesario para realizar las sucesivas lecturas
   */
   Logger::log(LogLevel::DEBUG, "INICIAMOS LA LECTURA DE LOS DATOS DE LA TABLA");

   this->tabla = tabla;
   // Recuperamos el nombre:
   this->nombre_tabla = this->tabla->metadata_ptr->name;
   // El nombre de las columnas:
   this->nombre_columnas = this->tabla->metadata_ptr->column_names;
   this->num_cols = this->tabla->metadata_ptr->n_cols;
   // Recuperamos el tipo de las columnas:
   this->tipo_columnas = this->tabla->metadata_ptr->column_types;
   // Inicializamos la variable por la cual añadiremos los datos venidos del disco:
   if(this->tabla->data_buffer_ptr == nullptr){
      this->tabla->data_buffer_ptr = new table_data_buffer();
   }
   // Ahora apuntamos nuestro puntero interno al que ya tiene (o acaba de recibir) la tabla
   this->ptr_datos_disco = this->tabla->data_buffer_ptr;
   Logger::log(LogLevel::DEBUG, "Recuperdos valores de la tabla y inicializado el apartado del disco en la tabla");
   //std::map<std::string, std::vector<Values>>& buffer_ram_del_disco = ptr_datos_disco->columns;

   // Ahora vemos que particiones hay:
   this->partition_counter = 0;
   this->total_partitions = 0;
   Logger::log(LogLevel::DEBUG, "Pasamos a obtener los archivos een la ruta:");
   Logger::log(LogLevel::DEBUG, "data/" + this->nombre_tabla + "/" + this->nombre_columnas[0]);
   // Ahora vemos si la columa primera o con indice 0 es de tamaño fijo o variable:

   this->particiones_nombres = disk_aux::obtener_archivos_en_ruta("data/" + this->nombre_tabla + "/" + this->nombre_columnas[0], this->tipo_columnas[0]);
   Logger::log(LogLevel::DEBUG, "Nombres de las particiones obtenidos con exito");

   if(!(this->particiones_nombres.empty())){
      Logger::log(LogLevel::DEBUG, "SI que hay archivos en DISCO");
      Logger::log(LogLevel::DEBUG, "Procedemos a ordenarlas de menor a mayor:");
      part_sort::bubble_sort(this->particiones_nombres);
      Logger::log(LogLevel::DEBUG, "Ordenamoento de las particiones realizado con exito");
      Logger::log(LogLevel::DEBUG, "Hallamos el número de particiones:");
      this->total_partitions = this->particiones_nombres.size();
      /*if(this->tipo_columnas[0] == dataType::STRING){
         this->total_partitions = this->total_partitions / 2;
      };*/
      Logger::log(LogLevel::DEBUG, "Numero de particones encontradas: ", false, true);
      Logger::log(LogLevel::DEBUG, this->total_partitions, true, false);
   };


   ////////////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////////////
   Logger::log(LogLevel::DEBUG, "Todo listo, procedemos a entrar en el bucle de las columnas:");

};

bool disk_in::read_table_iterator::obtain_int_partition_rows(bool aux_bool, std::string path_var, std::string partition_current){
   
   if(aux_bool){
      Logger::log(LogLevel::DEBUG, "<<<<<CASO INT VEMOS TAMANO PARTICION>>>>>>>");
      uint32_t size_tmp = 0;
      Logger::log(LogLevel::DEBUG, "Primera ejecucion. Vemos el tamano de la particion: ");
      Logger::log(LogLevel::DEBUG, path_var + partition_current);
      size_tmp = disk_aux::obtener_tamano_archivo(path_var + partition_current + ".dat");
      Logger::log(LogLevel::DEBUG, "Tamano del archivo de particion: ", false, true);
      Logger::log(LogLevel::DEBUG, size_tmp, true, false);
      Logger::log(LogLevel::DEBUG, "Tamano de la particion recuperado, lo registramos");
      this->numero_de_filas_current_partition =  size_tmp / sizeof(int);
      Logger::log(LogLevel::DEBUG, "Queda registrado el numero de filas de la particion: ", false, true);
      Logger::log(LogLevel::DEBUG, this->numero_de_filas_current_partition, true, false);
      aux_bool = false;
   };
   return aux_bool;
};


bool disk_in::read_table_iterator::obtain_float_partition_rows(bool aux_bool, std::string path_var, std::string partition_current){
   
   if(aux_bool){
      Logger::log(LogLevel::DEBUG, "<<<<<CASO FLOAT VEMOS TAMANO PARTICION>>>>>>>");
      uint32_t size_tmp = 0;
      Logger::log(LogLevel::DEBUG, "Primera ejecucion. Vemos el tamano de la particion: ");
      Logger::log(LogLevel::DEBUG, path_var + partition_current);
      size_tmp = disk_aux::obtener_tamano_archivo(path_var + partition_current + ".dat");
      Logger::log(LogLevel::DEBUG, "Tamano del archivo de particion: ", false, true);
      Logger::log(LogLevel::DEBUG, size_tmp, true, false);
      Logger::log(LogLevel::DEBUG, "Tamano de la particion recuperado, lo registramos");
      this->numero_de_filas_current_partition =  size_tmp / sizeof(float);
      Logger::log(LogLevel::DEBUG, "Queda registrado el numero de filas de la particion: ", false, true);
      Logger::log(LogLevel::DEBUG, this->numero_de_filas_current_partition, true, false);
      aux_bool = false;
   };
   return aux_bool;
};


bool disk_in::read_table_iterator::obtain_bool_partition_rows(bool aux_bool, std::string path_var, std::string partition_current){
   
   if(aux_bool){
      uint32_t size_tmp = 0;
      Logger::log(LogLevel::DEBUG, "<<<<<CASO BOOL VEMOS TAMANO PARTICION>>>>>>>");
      Logger::log(LogLevel::DEBUG, "Primera ejecucion. Vemos el tamano de la particion: ");
      Logger::log(LogLevel::DEBUG, path_var + partition_current);
      size_tmp = disk_aux::obtener_tamano_archivo(path_var + partition_current + ".dat");
      Logger::log(LogLevel::DEBUG, "Tamano del archivo de particion: ", false, true);
      Logger::log(LogLevel::DEBUG, size_tmp, true, false);
      Logger::log(LogLevel::DEBUG, "Tamano de la particion recuperado, lo registramos");
      this->numero_de_filas_current_partition =  size_tmp;
      Logger::log(LogLevel::DEBUG, "Queda registrado el numero de filas de la particion: ", false, true);
      Logger::log(LogLevel::DEBUG, this->numero_de_filas_current_partition, true, false);
      aux_bool = false;
   };
   return aux_bool;
};


bool disk_in::read_table_iterator::obtain_string_partition_rows(bool aux_bool, std::string path_var, std::string partition_current){
   
   if(aux_bool){
      uint32_t size_tmp = 0;
      Logger::log(LogLevel::DEBUG, "<<<<<CASO STRING VEMOS TAMANO PARTICION>>>>>>>");
      Logger::log(LogLevel::DEBUG, "Primera ejecucion. Vemos el tamano de la particion: ");
      Logger::log(LogLevel::DEBUG, path_var + partition_current);
      size_tmp = disk_aux::obtener_tamano_archivo(path_var + partition_current + ".idx");
      Logger::log(LogLevel::DEBUG, "Tamano del archivo de particion: ", false, true);
      Logger::log(LogLevel::DEBUG, size_tmp, true, false);
      Logger::log(LogLevel::DEBUG, "Tamano de la particion recuperado, lo registramos");
      this->numero_de_filas_current_partition =  size_tmp / sizeof(uint32_t);
      Logger::log(LogLevel::DEBUG, "Queda registrado el numero de filas de la particion: ", false, true);
      Logger::log(LogLevel::DEBUG, this->numero_de_filas_current_partition, true, false);
      aux_bool = false;
   };
   return aux_bool;
};

bool disk_in::read_table_iterator::read_table(){

   Logger::flush(LogLevel::DEBUG);
   Logger::log(LogLevel::DEBUG, "Dentro de 'read_table'");

   if(this->partition_counter < this->total_partitions){
      Logger::log(LogLevel::DEBUG, "Particiones leidas: ", false, true);
      Logger::log(LogLevel::DEBUG, this->partition_counter, true, false);
      Logger::log(LogLevel::DEBUG, "Particiones totales: ", false, true);
      Logger::log(LogLevel::DEBUG, this->total_partitions, true, false);
      // En este caso preparamos dónde se almacenarán los datos:
      Logger::log(LogLevel::DEBUG, "Accedemos al vector de std::map del disco:");
      std::map<std::string, std::vector<Values>>& buffer_ram_del_disco = this->ptr_datos_disco->columns;
      Logger::log(LogLevel::DEBUG, "std::map del disco hallado");
      Logger::log(LogLevel::DEBUG, "Limpiamos el std::map del disco:");
      buffer_ram_del_disco.clear();
      Logger::log(LogLevel::DEBUG, "std::map del disco limpiado con exito");
      bool aux_bool = true;


      // Se puede hacer fuera del bucle:
      std::string partition_current = this->particiones_nombres[this->partition_counter];
      // Le quitamos la extension al nombre:
      partition_current = part_sort::quitar_extension_archivo(partition_current);
      Logger::log(LogLevel::DEBUG, partition_current, true, false);
   
      for(int i = 0; i< this->num_cols; i++){

         dataType tipo_dato = tipo_columnas[i];
         std::string column_name = this->nombre_columnas[i];
         Logger::log(LogLevel::DEBUG, "Procesamos la columna: ", false, true);
         Logger::log(LogLevel::DEBUG, column_name, true, false);
         Logger::log(LogLevel::DEBUG, "Obtenemos la ruta: ", false, true);
         std::string path_var = "data/" + this->nombre_tabla + "/" + column_name + "/";
         Logger::log(LogLevel::DEBUG, path_var, true, false);
         Logger::log(LogLevel::DEBUG, "Accdemos a la particion actual: ", false, true);
         std::vector<Values> vec_vals;

         switch(tipo_dato){
            case dataType::INT: {
               Logger::log(LogLevel::DEBUG, "Caso INT");
               Logger::log(LogLevel::DEBUG, "++++++ CASE INT DEL SWITCH ++++++");
               aux_bool = this->obtain_int_partition_rows(aux_bool, path_var, partition_current);
               Logger::log(LogLevel::DEBUG, "Pasamos a leer esta particion de INTs:");
               disk_in::read_fixed_len_int_columns(path_var, partition_current, vec_vals);
               // Adicionamos estos datos a la tabla, en la region de datos del disco:
               Logger::log(LogLevel::DEBUG, "Procedemos a adicionar el vector de INTs a la TABLA");
               buffer_ram_del_disco[column_name] = std::move(vec_vals);
               Logger::log(LogLevel::DEBUG, "Vector adicionado a la TABLA con exito");
               break;
            };
            case dataType::FLOAT: {
               Logger::log(LogLevel::DEBUG, "++++++ CASE FLOAT DEL SWITCH ++++++");
               aux_bool = this->obtain_float_partition_rows(aux_bool, path_var, partition_current);
               Logger::log(LogLevel::DEBUG, "Caso FLOAT");
               //std::vector<float> vec_vals;
               Logger::log(LogLevel::DEBUG, "Procedemos a adicionar el vector de FLOATs a la TABLA");
               disk_in::read_fixed_len_float_columns(path_var, partition_current, vec_vals);
               // Adicionamos estos datos a la tabla, en la region de datos del disco:
               buffer_ram_del_disco[column_name] = std::move(vec_vals);
               Logger::log(LogLevel::DEBUG, "Vector adicionado a la TABLA con exito");
               break;
            };
            case dataType::BOOL: {
               Logger::log(LogLevel::DEBUG, "++++++ CASE BOOL DEL SWITCH ++++++");
               aux_bool = this->obtain_bool_partition_rows(aux_bool, path_var, partition_current);
               //std::vector<uint8_t> vec_vals;
               Logger::log(LogLevel::DEBUG, "Procedemos a adicionar el vector de BOOLs a la TABLA");
               disk_in::read_fixed_len_bool_columns(path_var, partition_current, vec_vals);
               buffer_ram_del_disco[column_name] = std::move(vec_vals);
               Logger::log(LogLevel::DEBUG, "Vector adicionado a la TABLA con exito");
               break;
            };
            case dataType::STRING: {
               Logger::log(LogLevel::DEBUG, "++++++ CASE STRING DEL SWITCH ++++++");
               aux_bool = this->obtain_string_partition_rows(aux_bool, path_var, partition_current);
               Logger::log(LogLevel::DEBUG, "Procedemos a adicionar el vector de STRINGs a la TABLA");
               //disk_in::read_fixed_len_string_columns(path_var, partition_current, vec_vals);
               // Creamos el objeto:
               Logger::flush(LogLevel::DEBUG);
               Logger::flush(LogLevel::DEBUG);
               Logger::flush(LogLevel::DEBUG);
               Logger::log(LogLevel::DEBUG, "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||");
               stringReader_v2 string_reader = stringReader_v2(vec_vals, path_var, partition_current, false);
               // Realizamos la lectura de la partición:
               string_reader.execute_fsm();

               Logger::flush(LogLevel::DEBUG);
               Logger::flush(LogLevel::DEBUG);
               Logger::flush(LogLevel::DEBUG);
               Logger::log(LogLevel::DEBUG, "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||");

               buffer_ram_del_disco[column_name] = std::move(vec_vals);
               Logger::log(LogLevel::DEBUG, "Vector adicionado a la TABLA con exito");
               break;
            };
            case dataType::UNKNOWN: {
               Logger::log(LogLevel::DEBUG, "++++++ CASE UNKNOWN DEL SWITCH ++++++");
               aux_bool = this->obtain_string_partition_rows(aux_bool, path_var, partition_current);
               Logger::log(LogLevel::DEBUG, "Procedemos a adicionar el vector de UNKNOWNs a la TABLA");
               //disk_in::read_fixed_len_string_columns(path_var, partition_current, vec_vals);
               // Creamos el objeto:
               Logger::flush(LogLevel::DEBUG);
               Logger::flush(LogLevel::DEBUG);
               Logger::flush(LogLevel::DEBUG);
               Logger::log(LogLevel::DEBUG, "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||");
               stringReader_v2 string_reader = stringReader_v2(vec_vals, path_var, partition_current, true);
               // Realizamos la lectura de la partición:
               string_reader.execute_fsm();

               Logger::flush(LogLevel::DEBUG);
               Logger::flush(LogLevel::DEBUG);
               Logger::flush(LogLevel::DEBUG);
               Logger::log(LogLevel::DEBUG, "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||");

               buffer_ram_del_disco[column_name] = std::move(vec_vals);
               Logger::log(LogLevel::DEBUG, "Vector adicionado a la TABLA con exito");
               break;
            };
         };
      };
      Logger::log(LogLevel::DEBUG, "!!!!!!! Sumamos una a la particion !!!!!!!");
      this->partition_counter += 1;
      return false;
   };
   Logger::log(LogLevel::DEBUG, "No quedan particiones por leer");
   // En el caso de ya haberse quedado sin particiones que leer, limpiamos la region de la tabla para datos leidos del disco:
   std::map<std::string, std::vector<Values>>& buffer_ram_del_disco = this->ptr_datos_disco->columns;
   buffer_ram_del_disco.clear();
   return true; // En caso de no haber leído naada, porque ya no hay datos
   // Ya habríamos aacabado la lectura, avanzamos en uno la partición:
};
