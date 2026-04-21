

#include "data_struct.h"
#include <iostream>
#include <fstream>
#include <string>
#include "globals.h"
#include <map>
#include "logging.h"
#include "filesystem"
#include "disk_io.h"
#include "disk_wal.h"
#include "disk_buffer.h"
#include <cstring> // Para usar std::memcpy


// LECTURA CON BUFFER:
bool disk_io::is_eof(std::ifstream& in, std::streampos end_pos){

   // Calculamos el tamaño del archivo:
   std::streampos current_pos = in.tellg();
   return current_pos > end_pos;
};


bool disk_io::is_eof_2(uint32_t puntero_lectura, uint32_t size_archivo){

   // Calculamos el tamaño del archivo con los punteros manuales:
   return puntero_lectura > size_archivo;
};


void disk_io::write_in_memory_with_data_buffer(dataType tipo_dato, std::vector<std::string> col_names, uint32_t& size_disponible_buffer, uint32_t& contador_bytes_buffer, std::map<std::string, std::vector<Values>>& columnas, int contador_cols, std::array<char, 128>& buffer, uint32_t& offset){

   /*
   Dado el buffer de datos y el tipo de dato, se asigna el valor a la memoria RAM de la tabla.
   Siempre en caso de no superar el 
   */
   Logger::log(LogLevel::DEBUG, "Entramos en la asignacion de memoria");

   if(offset == 0){

      switch(tipo_dato){

         case dataType::INT: {
            Logger::log(LogLevel::DEBUG, "CASO ENTERO");
            if(sizeof(int) > size_disponible_buffer){
               //offset = sizeof(int);
               offset = size_disponible_buffer; 
               Logger::log(LogLevel::DEBUG, "Nos hemos quedado a medias leyendo el INT");
               size_disponible_buffer -= sizeof(int);
               break;
            };
            size_disponible_buffer -= sizeof(int);
            // Podemos proceder a la lectura:
            int valor_tmp;
            std::memcpy(&valor_tmp, buffer.data() + contador_bytes_buffer, sizeof(int));
            contador_bytes_buffer += sizeof(int);
            columnas[col_names[contador_cols]].push_back(valor_tmp);
            break;
         };
         case dataType::FLOAT: {
            Logger::log(LogLevel::DEBUG, "CASO FLOAT");
            if(sizeof(float) > size_disponible_buffer){
               //offset = sizeof(float);
               offset = size_disponible_buffer;
               Logger::log(LogLevel::DEBUG, "Nos hemos quedado a medias leyendo el FLOAT");
               size_disponible_buffer -= sizeof(float);
               break;
            };
            size_disponible_buffer -= sizeof(float);
            // Podemos proceder a la lectura:
            float valor_tmp;
            std::memcpy(&valor_tmp, buffer.data() + contador_bytes_buffer, sizeof(float));
            contador_bytes_buffer += sizeof(float);
            columnas[col_names[contador_cols]].push_back(valor_tmp);
            break;
         };
         case dataType::BOOL: {
            Logger::log(LogLevel::DEBUG, "CASO BOOLEANO");
            if(sizeof(uint8_t) > size_disponible_buffer){
               //offset = sizeof(uint8_t);
               offset = size_disponible_buffer;
               Logger::log(LogLevel::DEBUG, "Nos hemos quedado a medias leyendo el BOOL");
               size_disponible_buffer -= sizeof(uint8_t);
               break;
            };
            size_disponible_buffer -= sizeof(uint8_t);
            // Podemos proceder a la lectura:
            uint8_t valor_tmp;
            std::memcpy(&valor_tmp, buffer.data() + contador_bytes_buffer, sizeof(uint8_t));
            contador_bytes_buffer += sizeof(uint8_t);
            // En este caso convertimos el valor a uno booleano:
            bool buffer_bool = false;
            if(valor_tmp == 1){
               buffer_bool = true;
            };
            columnas[col_names[contador_cols]].push_back(buffer_bool);
            break;
         };
         case dataType::STRING: {
            Logger::log(LogLevel::DEBUG, "CASO STRING");
            uint32_t size_disponible_buffer_snapshot = size_disponible_buffer;

            uint32_t string_size;
            // Antes nos aseguramos de que quede espacio para leer el tamaño de la string:
            if(sizeof(uint32_t) > size_disponible_buffer){
               offset = size_disponible_buffer;
               Logger::log(LogLevel::DEBUG, "Nos hemos quedado a medias leyendo el TAMANO de la STRING");
               size_disponible_buffer -= sizeof(uint32_t);
               break;
            };
            size_disponible_buffer -= sizeof(uint32_t);
            Logger::log(LogLevel::DEBUG, "Restamos al size disponible del buffer: ", false, true);
            Logger::log(LogLevel::DEBUG, sizeof(uint32_t), true, false);
            Logger::log(LogLevel::DEBUG, "Ahora el 'size_disponible_buffer' es: ", false, true);
            Logger::log(LogLevel::DEBUG, size_disponible_buffer, true, false);
            // Pasados este punto, hemos podido leer el tamaño de la string entero
            // Ahora veremos si podemos leer su tamño y contenido de una:


            // Leemos del buffer el tamaño de la string
            std::memcpy(&string_size, buffer.data() + contador_bytes_buffer, sizeof(uint32_t));
            contador_bytes_buffer += sizeof(uint32_t);

            // Ahora vemos si tenemos espacio suficiente para leer la string como tal;
            if(string_size + sizeof(uint32_t) > size_disponible_buffer){
               //offset = sizeof(string_size) + sizeof(uint32_t);
               offset = size_disponible_buffer_snapshot;
               //offset = sizeof(string_size);
               Logger::log(LogLevel::DEBUG, "Nos hemos quedado a medias leyendo la STRING");
               Logger::log(LogLevel::DEBUG, "El tamano de la string que dejamos a medias es de: ", false, true);
               Logger::log(LogLevel::DEBUG, string_size, true, false);

               size_disponible_buffer -=  string_size;
               break;
            };
            size_disponible_buffer -=  string_size;
            Logger::log(LogLevel::DEBUG, "Restamos al size disponible del buffer: ", false, true);
            Logger::log(LogLevel::DEBUG, string_size, false, false);
            Logger::log(LogLevel::DEBUG, " (hemos restado 'string_size')", true, false);
            Logger::log(LogLevel::DEBUG, "Ahora el 'size_disponible_buffer' es: ", false, true);
            Logger::log(LogLevel::DEBUG, size_disponible_buffer, true, false);
            // Realizamos la lectura:
            std::string valor_tmp;
            valor_tmp.resize(string_size); // Resize de la string
            std::memcpy(valor_tmp.data(), buffer.data() + contador_bytes_buffer, string_size);
            contador_bytes_buffer += string_size;
            columnas[col_names[contador_cols]].push_back(valor_tmp);
         };
      };
      Logger::log(LogLevel::DEBUG, "Hemos terminado la asignacion de memoria");
   } else {
      Logger::log(LogLevel::DEBUG, "NO se ha entrado a la asignacion de memoria");
   };
};




///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


void disk_io::lectura_datos_monolitica(table* tabla){

   // Abrimos el archivo:
   std::string nombre_tabla = tabla->metadata_ptr->name;
   std::string ruta_tabla = "data/" + nombre_tabla + "_data.bin";
   std::ifstream in(ruta_tabla, std::ios::binary);

   // Calculamos el tamaño del archivo:
   in.seekg(0, std::ios::end);
   std::streampos end_pos = in.tellg();
   in.seekg(0, std::ios::beg);
   std::streampos ini_pos = in.tellg();
   std::streampos file_size = end_pos - ini_pos;

   // PUNTEROS AUXILIARES:
   uint32_t posicion_inicial = 0;
   //uint32_t posicion_final = 0;
   uint32_t buffer_size = 128;
   uint32_t offset = 0;
   uint32_t final_archivo = static_cast<uint32_t>(file_size);
   Logger::log(LogLevel::DEBUG, "EL TAMANO DEL ARCHIVO ES: ", false, true);
   Logger::log(LogLevel::DEBUG, final_archivo, true, false);
   Logger::flush();
   uint32_t tamano_archivo_disponible = final_archivo;
   uint32_t size_dinamico_lectura = 0;
   uint32_t puntero_lectura = 0;

   uint32_t contador_columnas_permanente = 0; // Para llevar un conteo persistente de las columnas

   // incializamos el buffer de lectura
   std::array<char, 128> buffer;

   Logger::flush();
   Logger::flush();
   Logger::log(LogLevel::DEBUG, "7777777777777777777777777777777777777777777");
   Logger::flush();
   Logger::log(LogLevel::DEBUG, "SE HA INICIADO LA LECTURA CON BUFFER DE LOS DATOS:");



   // Accedemos al tipo de datos desde el esquema:
   table_metadata* metadata = tabla->metadata_ptr;
   std::vector<dataType> tipos_datos = tabla->metadata_ptr->column_types;
   uint32_t n_cols = tipos_datos.size();
   std::vector<std::string> col_names = tabla->metadata_ptr->column_names;
   uint32_t n_filas;

   // Leemos el número de filas:
   in.read(reinterpret_cast<char*>(&n_filas), sizeof(uint32_t));

   int aux_counter = 1;

   // uint32_t contador_columna = 0;


   uint32_t size_disponible = 0;

   std::streampos current_pos = in.tellg();
   uint32_t posicion_actual = static_cast<uint32_t>(current_pos);


   bool is_eof_bool = false; // Booleano para indicar si estamos al final o no:

   Logger::log(LogLevel::DEBUG, "Se han leido el numero de filas");
   while(!is_eof_bool){
   //while(!disk_io::is_eof(in, end_pos)){
      Logger::log(LogLevel::DEBUG, "Iteracion del bucle while principal: ", false, true);
      Logger::log(LogLevel::DEBUG, aux_counter, true, false);
      Logger::log(LogLevel::DEBUG, "Posicion alctual del puntero de lectura: ", false, true);
      Logger::log(LogLevel::DEBUG, posicion_actual, true, false);


      Logger::flush();


      uint32_t final_calculado = final_archivo - posicion_inicial;
      size_dinamico_lectura = buffer_size - offset;
      if(size_dinamico_lectura > tamano_archivo_disponible){
         size_dinamico_lectura = tamano_archivo_disponible;
      };

      // Actualizamos la posicion_final:
      posicion_inicial += size_dinamico_lectura;


      // Justo antes de la lectura, movemos los bytes del offset, del final al principio:
      std::copy(
            buffer.end() - offset, 
            buffer.end(), 
            buffer.begin()
         );

      //std::memmove(buffer.data(), buffer.data() + size_dinamico_lectura, offset);

      // Ahora si realizamos la lectura:
      //in.seekg(-offset, std::ios::cur); // Nos movemos -offset el puntero de lectura para pdoer leear en lo que nos quedamos a medias
      in.read(buffer.data() + offset , size_dinamico_lectura);
      Logger::log(LogLevel::DEBUG, "Hemos leido estos bytes: ", false, true);
      Logger::log(LogLevel::DEBUG, size_dinamico_lectura, true, false);
      Logger::log(LogLevel::DEBUG, "El offset en esta lectura es de: ", false, true);
      Logger::log(LogLevel::DEBUG, offset, true, false);
      Logger::flush();
      puntero_lectura += size_dinamico_lectura;

      /*if(aux_counter == 300){
         break;
      };*/

      // Ahora ya podemos volver a filar el offset a cero:
      offset = 0;
      ///////////////////////////////////////////////////////////////////////////////////
      //Ya tenemos el buffer, ahora, procesamos su contenido

      // Leer por filas y columnas y con un contador de bytes para ver donde vamos y si dejamos una variable a medias:
      uint32_t contador_bytes_buffer = 0;

      //size_disponible = size_dinamico_lectura;
      size_disponible = 128;


      // inicializamos donde guardaremos los datos en disco:
      std::map<std::string, std::vector<Values>>& columnas = tabla->data_buffer_ptr->columns;
      while(size_disponible >= 0 && offset == 0){
         //for(int j = 0; j<n_cols; j++){
         // Actualizamos el tamaño disponible del buffer
         //size_disponible = size_disponible - contador_bytes_buffer;

         Logger::log(LogLevel::DEBUG, "++++++++++++++++++++++++++++++++++++++++++++++");

         Logger::log(LogLevel::DEBUG, "Iteracion por la columna: ", false, true);
         Logger::log(LogLevel::DEBUG, col_names[contador_columnas_permanente], false, false);
         Logger::log(LogLevel::DEBUG, " numero: ", false, false);
         Logger::log(LogLevel::DEBUG, contador_columnas_permanente, true, false);


         Logger::log(LogLevel::DEBUG, "size_disponible antes de lectura: ", false, true);
         Logger::log(LogLevel::DEBUG, size_disponible, true, false);
         Logger::log(LogLevel::DEBUG, "offset antes de lectura: ", false, true);
         Logger::log(LogLevel::DEBUG, offset, true, false);

         disk_io::write_in_memory_with_data_buffer(
            tipos_datos[contador_columnas_permanente],
            col_names,
            size_disponible, // Size disponible del buffer
            contador_bytes_buffer,
            columnas,
            contador_columnas_permanente,
            buffer,
            offset
         );

         Logger::log(LogLevel::DEBUG, "size_disponible despues de lectura: ", false, true);
         Logger::log(LogLevel::DEBUG, size_disponible, true, false);
         Logger::log(LogLevel::DEBUG, "offset despues de lectura: ", false, true);
         Logger::log(LogLevel::DEBUG, offset, true, false);
         Logger::log(LogLevel::DEBUG, "++++++++++++++++++++++++++++++++++++++++++++++");
         Logger::flush();

         if(offset == 0){
            contador_columnas_permanente += 1;
         };


         if(contador_columnas_permanente>=n_cols){
            contador_columnas_permanente = 0;
         };


            // Si tenemos aprovisionamiento llenamos la sección de datos venidos de disco de la tabla en RAM

      };

      aux_counter += 1;

      // Recalculamos el tamño disponbile del archivo:
      tamano_archivo_disponible -= size_dinamico_lectura;

      is_eof_bool = disk_io::is_eof_2(puntero_lectura, size_dinamico_lectura);
      
   };
   in.close();
};

//-----------------------------------------------------------------------------------------------------------
void disk_io::aux_vector_buffer_write_disk(std::vector<char>& buffer, std::ofstream& out){

   if (out.is_open()) {
         // Escribimos todo el contenido una sola vez
         out.write(buffer.data(), buffer.size());
   };
};

void disk_io::aux_vector_buffer_write_disk(std::vector<char>& buffer, std::fstream& out){

   if (out.is_open()) {
         // Escribimos todo el contenido una sola vez
         out.write(buffer.data(), buffer.size());
   };
};


//---------------------------------------------------------------------------------
//========================================================
//== FUNION ESCRITURA METADATOS: =========================
//========================================================

void disk_io::eliminar_archivo_binario_metadatos(table*& tb){
   /*
   Función que dada la referencia al puntero de la tabla,
   elimina sus metadatos en disco si estos existen.
   */

   std::string tb_name = tb->metadata_ptr->name;

   std::filesystem::path ruta = "data/" + tb_name + "_meta.bin";

   if(std::filesystem::exists(ruta)){
      // Sólo eliminamos si existe el archivo:
      std::filesystem::remove(ruta);
   };
};


void disk_io::eliminar_archivo_binario_datos(table*& tb){
   /*
   Función que dada la referencia al puntero de la tabla,
   elimina sus datos en disco si estos existen.
   */

   std::string tb_name = tb->metadata_ptr->name;

   std::filesystem::path ruta = "data/" + tb_name + "_data.bin";

   if(std::filesystem::exists(ruta)){
      // Sólo eliminamos si existe el archivo:
      std::filesystem::remove(ruta);
   };
}; 

//========================================================
//== FUNION ESCRITURA METADATOS: =========================
//========================================================
uint32_t disk_io::write_table_metadata(table* tabla){
   /*
   Función que escribe los metadatos en disco.
   */
   if (!tabla) return 0;
   std::string nombre_tabla = tabla->metadata_ptr->name;
   std::string ruta_tabla = "data/" + nombre_tabla + "_meta.bin";

   // Abrimos la escritura:
   std::ofstream out(ruta_tabla, std::ios::binary | std::ios::out);

   /*
   Usamos un vector de punteros de caracteres para así no 
   realizar tantas llamadas a la escritura
   */
   std::vector<char> buffer;
   char* tmp_char_ptr = nullptr;

   // == Escribimos los metadatos: ===============================

   // -- Escribimos el nombre -----------------------------------------
   uint32_t size_nombre = nombre_tabla.size();
   //out.write(reinterpret_cast<char*>(&size_nombre), sizeof(uint32_t));
   tmp_char_ptr = reinterpret_cast<char*>(&size_nombre);
   buffer.insert(buffer.end(),
                 tmp_char_ptr,
                 tmp_char_ptr + sizeof(uint32_t)
                );
   //out.write(nombre_tabla.data(), size_nombre);
   tmp_char_ptr = nombre_tabla.data();
   buffer.insert(buffer.end(),
                 tmp_char_ptr,
                 tmp_char_ptr + size_nombre
                );

   // -- Escribimos el número de columnas -----------------------------
   uint32_t num_cols = (tabla->metadata_ptr->column_names).size();
   //out.write(reinterpret_cast<char*>(&num_cols), sizeof(uint32_t));
   tmp_char_ptr = reinterpret_cast<char*>(&num_cols);
   buffer.insert(buffer.end(),
                 tmp_char_ptr,
                 tmp_char_ptr + sizeof(uint32_t)
                );

   std::map<std::string, std::vector<Values>> columnas = tabla->data_ptr->columns;
   if(!columnas.empty()){
      std::string column_name = (tabla->metadata_ptr->column_names)[0];
      std::vector<Values> col_datos = columnas.at(column_name);
      uint32_t n_rows = col_datos.size();
      uint32_t n_rows_total = n_rows;
      uint32_t n_rows_disk = 0;
      // Escribimos las filas en RAM:
      if(tabla->data_buffer_ptr){
         std::map<std::string, std::vector<Values>>& columnas_disco = tabla->data_buffer_ptr->columns;
         if(!columnas_disco.empty()){
            n_rows_disk = columnas_disco.at(column_name).size();
         };
         //n_rows_disk = tabla->data_buffer_ptr->columns.size();
         n_rows_total += n_rows_disk;
      };
      Logger::log(LogLevel::DEBUG, "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
      Logger::log(LogLevel::DEBUG, "Escritura de los metadatos");
      Logger::log(LogLevel::DEBUG, "Escribimos el total de filas: ", false, true);
      Logger::log(LogLevel::DEBUG, n_rows_total, true, false);
      Logger::log(LogLevel::DEBUG, "filas en RAM: ", false, true);
      Logger::log(LogLevel::DEBUG, n_rows, false, false);
      Logger::log(LogLevel::DEBUG, " filas en disco: ", false, false);
      Logger::log(LogLevel::DEBUG, n_rows_disk, true, false);
      Logger::log(LogLevel::DEBUG, "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
      //out.write(reinterpret_cast<char*>(&n_rows_total), sizeof(uint32_t));
      tmp_char_ptr = reinterpret_cast<char*>(&n_rows_total);
      buffer.insert(buffer.end(),
                  tmp_char_ptr,
                  tmp_char_ptr + sizeof(uint32_t)
                  );
      for(uint32_t i=0; i<num_cols; i++){
         // -- Escribimos los datos de cada columna ---------------------
         // -- Escribimos el nombre:
         std::string column_name = (tabla->metadata_ptr->column_names)[i];
         uint32_t size_column_name = column_name.size();
         //out.write(reinterpret_cast<char*>(&size_column_name), sizeof(uint32_t));
         tmp_char_ptr = reinterpret_cast<char*>(&size_column_name);
         buffer.insert(buffer.end(),
                     tmp_char_ptr,
                     tmp_char_ptr + sizeof(uint32_t)
                     );
         //out.write(column_name.data(), size_column_name);
         tmp_char_ptr = column_name.data();
         buffer.insert(buffer.end(),
                     tmp_char_ptr,
                     tmp_char_ptr + size_column_name
                     );
         // -- Escribimos el tipo de dato:
         dataType col_type = (tabla->metadata_ptr->column_types)[i];
         uint32_t col_type_disk = static_cast<uint32_t>(col_type);
         //out.write(reinterpret_cast<char*>(&col_type_disk), sizeof(uint32_t));
         tmp_char_ptr = reinterpret_cast<char*>(&col_type_disk);
         buffer.insert(buffer.end(),
                     tmp_char_ptr,
                     tmp_char_ptr + sizeof(uint32_t)
                     );
         // -- Escribimos si es clave primaria:
         bool column_is_key = (tabla->metadata_ptr->primary_list)[i];
         uint8_t key_val = column_is_key ? 1 : 0;
         //out.write(reinterpret_cast<char*>(&key_val), sizeof(uint8_t));
         tmp_char_ptr = reinterpret_cast<char*>(&key_val);
         buffer.insert(buffer.end(),
                     tmp_char_ptr,
                     tmp_char_ptr + sizeof(uint8_t)
                     );
      };
      // Ahora es cuando recorremos los vectores y hacemos la escritura como tal
       disk_io::aux_vector_buffer_write_disk(buffer,
                                             out
                                            );
      out.flush();
      out.close();
      return n_rows;
   };
   out.close();
   return 0;
};


// Funcion auxliar para escribir un valor concreto en disco:
void disk_io::write_aux_val(Values value, dataType tipo_dato, std::fstream& out){

   /*
   Variante de la función con std::fstream como 'out'
   */

   switch(tipo_dato){

      case dataType::INT: {
         int buffer;
         buffer = std::get<int>(value);
         out.write(reinterpret_cast<char*>(&buffer), sizeof(int));
         break;
      };
      case dataType::FLOAT: {
         float buffer;
         buffer = std::get<float>(value);
         out.write(reinterpret_cast<char*>(&buffer), sizeof(float));
         break;
      };
      case dataType::BOOL: {
         bool buffer_bool;
         uint8_t buffer_int8;
         buffer_bool = std::get<bool>(value);
         if(buffer_bool){
            buffer_int8 = 1;
         }else{
            buffer_int8 = 0;
         };
         out.write(reinterpret_cast<char*>(&buffer_int8), sizeof(uint8_t));
         break;
      };
      case dataType::STRING: {
         std::string buffer;
         uint32_t string_size;
         buffer = std::get<std::string>(value);
         string_size = buffer.size();
         // Primero escribimos el tamaño de la string:
         out.write(reinterpret_cast<char*>(&string_size), sizeof(uint32_t));
         // Ahora ya si escribimos la cadena de texto:
         out.write(reinterpret_cast<char*>(buffer.data()), string_size);
         break;
      };
   };
};


// Funcion auxliar para escribir un valor concreto en disco:
void disk_io::write_aux_val(Values value, dataType tipo_dato, std::ofstream& out){

   /*
   Variante de la función con std::ofstream como 'out'
   */

   switch(tipo_dato){

      case dataType::INT: {
         int buffer;
         buffer = std::get<int>(value);
         out.write(reinterpret_cast<char*>(&buffer), sizeof(int));
         break;
      };
      case dataType::FLOAT: {
         float buffer;
         buffer = std::get<float>(value);
         out.write(reinterpret_cast<char*>(&buffer), sizeof(float));
         break;
      };
      case dataType::BOOL: {
         bool buffer_bool;
         uint8_t buffer_int8;
         buffer_bool = std::get<bool>(value);
         if(buffer_bool){
            buffer_int8 = 1;
         }else{
            buffer_int8 = 0;
         };
         out.write(reinterpret_cast<char*>(&buffer_int8), sizeof(uint8_t));
         break;
      };
      case dataType::STRING: {
         std::string buffer;
         uint32_t string_size;
         buffer = std::get<std::string>(value);
         string_size = buffer.size();
         // Primero escribimos el tamaño de la string:
         out.write(reinterpret_cast<char*>(&string_size), sizeof(uint32_t));
         // Ahora ya si escribimos la cadena de texto:
         out.write(reinterpret_cast<char*>(buffer.data()), string_size);
         break;
      };
   };
};


// Función auxiliar para escribir un buffer en vez de en disco:
void disk_io::write_aux_val_buffer(Values value, dataType tipo_dato, std::vector<char>& buffer){

   /*
   Función parecida a 'write_aux_val', pero esta vez escribe un buffer de carácteres.
   Dejándolo listo para así escribir el buffer en una sola llamada al disco,
   reduciendo latencia de interacción con el disco en escritura.
   Si se lee este mismo buffer se ahorrará también significativamente en tiempo de
   lectura.
   */

   char* tmp_char_ptr = nullptr;

   switch(tipo_dato){

      case dataType::INT: {
         int int_val;
         int_val = std::get<int>(value);
         tmp_char_ptr = reinterpret_cast<char*>(&int_val);
         //out.write(reinterpret_cast<char*>(&buffer), sizeof(int));
         buffer.insert(buffer.end(),
                     tmp_char_ptr,
                     tmp_char_ptr + sizeof(int)
                     );
         break;
      };
      case dataType::FLOAT: {
         float float_val;
         float_val = std::get<float>(value);
         tmp_char_ptr = reinterpret_cast<char*>(&float_val);
         //out.write(reinterpret_cast<char*>(&buffer), sizeof(float));
         buffer.insert(buffer.end(),
                     tmp_char_ptr,
                     tmp_char_ptr + sizeof(float)
                     );
         break;
      };
      case dataType::BOOL: {
         bool buffer_bool;
         uint8_t buffer_int8;
         buffer_bool = std::get<bool>(value);
         if(buffer_bool){
            buffer_int8 = 1;
         }else{
            buffer_int8 = 0;
         };
         tmp_char_ptr = reinterpret_cast<char*>(&buffer_int8);
         //out.write(reinterpret_cast<char*>(&buffer_int8), sizeof(uint8_t));
         buffer.insert(buffer.end(),
                     tmp_char_ptr,
                     tmp_char_ptr + sizeof(uint8_t)
                     );
         break;
      };
      case dataType::STRING: {
         std::string string_val;
         uint32_t string_size;
         string_val = std::get<std::string>(value);
         string_size = string_val.size();
         // Primero escribimos el tamaño de la string:
         tmp_char_ptr = reinterpret_cast<char*>(&string_size);
         buffer.insert(buffer.end(),
                     tmp_char_ptr,
                     tmp_char_ptr + sizeof(uint32_t)
                     );
         // Ahora ya si escribimos la cadena de texto:
         tmp_char_ptr = string_val.data();
         buffer.insert(buffer.end(),
                     tmp_char_ptr,
                     tmp_char_ptr + string_size
                     );
         break;
      };
   };
};




bool disk_io::write_aux_val_buffer_v2(Values value, dataType tipo_dato, std::array<char, 128>& buffer, uint32_t& bytes_escritos_buffer, uint32_t& size_disponible_buffer){

   /*
   Función parecida a 'write_aux_val', pero esta vez escribe un buffer de carácteres.
   Dejándolo listo para así escribir el buffer en una sola llamada al disco,
   reduciendo latencia de interacción con el disco en escritura.
   Si se lee este mismo buffer se ahorrará también significativamente en tiempo de
   lectura.
   */

   char* tmp_char_ptr = nullptr;

   switch(tipo_dato){

      case dataType::INT: {
         Logger::log(LogLevel::DEBUG, "CASO INT");
         int int_val;
         int_val = std::get<int>(value);
         tmp_char_ptr = reinterpret_cast<char*>(&int_val);
         if(sizeof(int)>size_disponible_buffer){
            return true;
         };
         std::memcpy(
            buffer.data() + bytes_escritos_buffer, // 1. Destino (puntero al inicio + desplazamiento)
            tmp_char_ptr,                // 2. Origen (la dirección de memoria de tu variable)
            sizeof(int)               // 3. Tamaño (cuántos bytes ocupa la variable)
         );
         bytes_escritos_buffer += sizeof(int);
         size_disponible_buffer -= sizeof(int);
         return false;
      };
      case dataType::FLOAT: {
         Logger::log(LogLevel::DEBUG, "CASO FLOAT");
         float float_val;
         float_val = std::get<float>(value);
         tmp_char_ptr = reinterpret_cast<char*>(&float_val);
         //out.write(reinterpret_cast<char*>(&buffer), sizeof(float));
         if(sizeof(float)>size_disponible_buffer){
            return true;
         };
         std::memcpy(
            buffer.data() + bytes_escritos_buffer, // 1. Destino (puntero al inicio + desplazamiento)
            tmp_char_ptr,                // 2. Origen (la dirección de memoria de tu variable)
            sizeof(float)               // 3. Tamaño (cuántos bytes ocupa la variable)
         );
         bytes_escritos_buffer += sizeof(float);
         size_disponible_buffer -= sizeof(float);
         return false;
      };
      case dataType::BOOL: {
         Logger::log(LogLevel::DEBUG, "CASO BOOL");
         bool buffer_bool;
         uint8_t buffer_int8;
         buffer_bool = std::get<bool>(value);
         if(buffer_bool){
            buffer_int8 = 1;
         }else{
            buffer_int8 = 0;
         };
         tmp_char_ptr = reinterpret_cast<char*>(&buffer_int8);
         //out.write(reinterpret_cast<char*>(&buffer_int8), sizeof(uint8_t));
         if(sizeof(uint8_t)>size_disponible_buffer){
            return true;
         };
         std::memcpy(
            buffer.data() + bytes_escritos_buffer, // 1. Destino (puntero al inicio + desplazamiento)
            tmp_char_ptr,                // 2. Origen (la dirección de memoria de tu variable)
            sizeof(uint8_t)               // 3. Tamaño (cuántos bytes ocupa la variable)
         );
         bytes_escritos_buffer += sizeof(uint8_t);
         size_disponible_buffer -= sizeof(uint8_t);
         return false;
      };
      case dataType::STRING: {
         Logger::log(LogLevel::DEBUG, "CASO STRING");
         std::string string_val;
         uint32_t string_size;
         string_val = std::get<std::string>(value);
         string_size = string_val.size();
         // Primero escribimos el tamaño de la string:
         tmp_char_ptr = reinterpret_cast<char*>(&string_size);
         if(sizeof(uint32_t) + string_size > size_disponible_buffer){
            return true;
         };
         std::memcpy(
            buffer.data() + bytes_escritos_buffer, // 1. Destino (puntero al inicio + desplazamiento)
            tmp_char_ptr,                // 2. Origen (la dirección de memoria de tu variable)
            sizeof(uint32_t)               // 3. Tamaño (cuántos bytes ocupa la variable)
         );
         bytes_escritos_buffer += sizeof(uint32_t);
         size_disponible_buffer -= sizeof(uint32_t);
         // Ahora ya si escribimos la cadena de texto:
         tmp_char_ptr = string_val.data();
         std::memcpy(
            buffer.data() + bytes_escritos_buffer, // 1. Destino (puntero al inicio + desplazamiento)
            tmp_char_ptr,                // 2. Origen (la dirección de memoria de tu variable)
            string_size               // 3. Tamaño (cuántos bytes ocupa la variable)
         );
         bytes_escritos_buffer += string_size;
         size_disponible_buffer -= string_size;
         return false;
      };
   };
};

//========================================================
//== FUNION ESCRITURA DATOS: =============================
//========================================================

void disk_io::write_table_data(table* tabla, uint32_t n_rows){

   if (!tabla) return;
   std::string nombre_tabla = tabla->metadata_ptr->name;
   std::string ruta_tabla = "data/" + nombre_tabla + "_data.bin";

   // Abrimos la escritura:
   std::fstream out(ruta_tabla, std::ios::in | std::ios::out | std::ios::binary);

   // Ahora vemos si el archivo esta abierto o no:
   if(!out.is_open()){
      // Lo escribimos solo como out:
      out.clear();
      out.open(ruta_tabla, std::ios::out | std::ios::binary);
   } else {
      out.seekp(0, std::ios::beg);
   };

   if (!out) {
      Logger::log(LogLevel::ERROR, "Error fatal abriendo archivo");
      return;
   };


   table_metadata* metadata = tabla->metadata_ptr;
   std::vector<dataType> tipos_datos = tabla->metadata_ptr->column_types;
   std::vector<std::string> columnas_nombre = tabla->metadata_ptr->column_names;
   uint32_t n_cols = tipos_datos.size();
   uint32_t n_filas = metadata->n_filas_ram;

   // Creamos el iterador por filas:
   auto it = disk_buffer::tableRowIterator_only_ram(nombre_tabla);
   std::map<std::string, Values> fila_a_escribir;
   Values valor_tmp;

   // Antes de escribir los datos, escribimos las filas de datos totales:
   uint32_t n_f_disk = tabla->metadata_ptr->n_filas_disco;
   uint32_t n_f_ram = tabla->metadata_ptr->n_filas_ram;

   uint32_t n_filas_total = n_f_disk + n_f_ram;
   Logger::log(LogLevel::DEBUG, "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
   Logger::log(LogLevel::DEBUG, "FILAS QUE SE ESCRIBEN EN DISCO:");
   Logger::log(LogLevel::DEBUG, "Filas en RAM: ", false, true);
   Logger::log(LogLevel::DEBUG, n_f_ram, true, false);
   Logger::log(LogLevel::DEBUG, "Filas en Disco: ", false, true);
   Logger::log(LogLevel::DEBUG, n_f_disk, true, false);
   Logger::log(LogLevel::DEBUG, "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
   out.seekp(0);
   out.write(reinterpret_cast<char*>(&n_filas_total), sizeof(uint32_t));

   // Ahora nos movemos al final para poder escribir sólo al final
   out.flush();
   out.clear();
   out.seekp(0, std::ios::end);
   std::vector<char> buffer;
   // Ahora iteraremos hasta que esteé vacía la fila a escribir
   while(!it.is_eof()){
      fila_a_escribir = it.get_next_row_ram_viva();
      // Ahora iteramos por cada columna:
      for(int i = 0; i<n_cols; i++){
         // valor_tmp = fila_a_escribir[i]; // Obtenemos el valor de una fila y columna concretos
         valor_tmp = fila_a_escribir.at(columnas_nombre[i]); // Obtenemos el valor de una fila y columna concretos
         //disk_io::write_aux_val(valor_tmp, tipos_datos[i], out);
         disk_io::write_aux_val_buffer(valor_tmp, tipos_datos[i], buffer);
      };
   };
   // Antes de cerrar la escritura, escribimos el buffer de escritura:
   disk_io::aux_vector_buffer_write_disk(buffer, out);
   out.flush();
   out.close();
};



void disk_io::write_table_data_with_buffer(table* tabla, uint32_t n_rows){

   if (!tabla) return;
   std::string nombre_tabla = tabla->metadata_ptr->name;
   std::string ruta_tabla = "data/" + nombre_tabla + "_data.bin";

   // Abrimos la escritura:
   std::fstream out(ruta_tabla, std::ios::in | std::ios::out | std::ios::binary);

   // Ahora vemos si el archivo esta abierto o no:
   if(!out.is_open()){
      // Lo escribimos solo como out:
      out.clear();
      out.open(ruta_tabla, std::ios::out | std::ios::binary);
   } else {
      out.seekp(0, std::ios::beg);
   };

   if (!out) {
      Logger::log(LogLevel::ERROR, "Error fatal abriendo archivo");
      return;
   };
   //////////////////////////////////////////////////////////////////
   // Preraramos el buffer de escritura:
   std::array<char, 128> buffer;
   uint32_t bytes_escritos_buffer = 0;
   uint32_t size_disponible_buffer = 128;


   ///////////////////////////////////////////////////////////////////


   table_metadata* metadata = tabla->metadata_ptr;
   std::vector<dataType> tipos_datos = tabla->metadata_ptr->column_types;
   std::vector<std::string> columnas_nombre = tabla->metadata_ptr->column_names;
   uint32_t n_cols = tipos_datos.size();
   uint32_t n_filas = metadata->n_filas_ram;

   // Creamos el iterador por filas:
   auto it = disk_buffer::tableRowIterator_only_ram(nombre_tabla);
   std::map<std::string, Values> fila_a_escribir; // Mapa nombre <-> vector de valores
   Values valor_tmp;

   // Antes de escribir los datos, escribimos las filas de datos totales:
   uint32_t n_f_disk = tabla->metadata_ptr->n_filas_disco;
   uint32_t n_f_ram = tabla->metadata_ptr->n_filas_ram;

   uint32_t n_filas_total = n_f_disk + n_f_ram;
   Logger::log(LogLevel::DEBUG, "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
   Logger::log(LogLevel::DEBUG, "FILAS QUE SE ESCRIBEN EN DISCO:");
   Logger::log(LogLevel::DEBUG, "Filas en RAM: ", false, true);
   Logger::log(LogLevel::DEBUG, n_f_ram, true, false);
   Logger::log(LogLevel::DEBUG, "Filas en Disco: ", false, true);
   Logger::log(LogLevel::DEBUG, n_f_disk, true, false);
   Logger::log(LogLevel::DEBUG, "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
   out.seekp(0);
   out.write(reinterpret_cast<char*>(&n_filas_total), sizeof(uint32_t));

   // Ahora nos movemos al final para poder escribir sólo al final
   out.flush();
   out.clear();
   out.seekp(0, std::ios::end);
   //std::vector<char> buffer;
   ///////////////////////////////////////////////////////////////////////
   // Ahora aquí si comenzamos con la escritura de datos por buffer:
   uint32_t contador_columnas = 0;
   uint32_t contador_filas = metadata->n_filas_ram;
   bool offset = false;
   uint32_t contador_fila_aux = 0;
   //bool is_eof_value = it.is_eof();
   // Ahora iteraremos hasta que esteé vacía la fila a escribir
   //while(!it.is_eof()){
   while(!it.is_eof() || contador_columnas < n_cols){

      Logger::log(LogLevel::DEBUG, "Valor de si es EOF ANTES de actualizarlo: ", false, true);
      if(it.is_eof()){
         Logger::log(LogLevel::DEBUG, "true", true, false);
      } else {
         Logger::log(LogLevel::DEBUG, "false", true, false);
      };

      if(contador_columnas>=n_cols){
         contador_columnas = 0;
      };
      if(contador_columnas == 0 && !offset && !it.is_eof()){
         Logger::log(LogLevel::DEBUG, "ENTRAMOS A RECUPERAR LA SIGUIENTE FILA");
         fila_a_escribir = it.get_next_row_ram_viva();
         contador_fila_aux += 1;
      };

      Logger::log(LogLevel::DEBUG, "Valor de si es EOF DESPUES de actualizarlo: ", false, true);
      if(it.is_eof()){
         Logger::log(LogLevel::DEBUG, "true", true, false);
      } else {
         Logger::log(LogLevel::DEBUG, "false", true, false);
      };
      /*while(!offset){
         size_disponible_buffer
      };*/
      if(!offset){
         valor_tmp = fila_a_escribir.at(columnas_nombre[contador_columnas]); // Obtenemos el valor de una fila y columna concretos
      };
      //disk_io::write_aux_val(valor_tmp, tipos_datos[i], out);
      //disk_io::write_aux_val_buffer(valor_tmp, tipos_datos[contador_columnas], buffer);

      Logger::flush();
      Logger::log(LogLevel::DEBUG, "????????????????????????????????????????????????????????????");
      Logger::log(LogLevel::DEBUG, "Insercion de valor en el buffer");
      Logger::log(LogLevel::DEBUG, "fila: ", false, true);
      Logger::log(LogLevel::DEBUG, contador_fila_aux, true, false);
      Logger::log(LogLevel::DEBUG, "columna: ", false, true);
      Logger::log(LogLevel::DEBUG, contador_columnas, true, false);
      Logger::log(LogLevel::DEBUG, "ANTES de una iteracion en la insercion en el buffer:");
      Logger::log(LogLevel::DEBUG, "DESPUES de una iteracion en la insercion en el buffer:");
      Logger::log(LogLevel::DEBUG, "bytes_escritos_buffer: ", false, true);
      Logger::log(LogLevel::DEBUG, bytes_escritos_buffer, true, false);
      Logger::log(LogLevel::DEBUG, "size_disponible_buffer: ", false, true);
      Logger::log(LogLevel::DEBUG, size_disponible_buffer, true, false);
      Logger::log(LogLevel::DEBUG, "offset: ", false, true);
      if(offset){
         Logger::log(LogLevel::DEBUG, "true", true, false);
      } else {
         Logger::log(LogLevel::DEBUG, "false", true, false);
      };
      offset = disk_io::write_aux_val_buffer_v2(valor_tmp, tipos_datos[contador_columnas], buffer, bytes_escritos_buffer, size_disponible_buffer);

      Logger::log(LogLevel::DEBUG, "DESPUES de una iteracion en la insercion en el buffer:");
      Logger::log(LogLevel::DEBUG, "bytes_escritos_buffer: ", false, true);
      Logger::log(LogLevel::DEBUG, bytes_escritos_buffer, true, false);
      Logger::log(LogLevel::DEBUG, "size_disponible_buffer: ", false, true);
      Logger::log(LogLevel::DEBUG, size_disponible_buffer, true, false);
      Logger::log(LogLevel::DEBUG, "offset: ", false, true);
      if(offset){
         Logger::log(LogLevel::DEBUG, "true", true, false);
      } else {
         Logger::log(LogLevel::DEBUG, "false", true, false);
      };
      Logger::log(LogLevel::DEBUG, "????????????????????????????????????????????????????????????");
      Logger::flush();

      //std::memcpy(valor_tmp.data(), buffer.data() + contador_bytes_buffer, string_size);
 

      if(!offset){
         contador_columnas += 1;
      } else {
         // Escribimos ya en el disco duro los buytes ya escritos:
         out.write(buffer.data(), bytes_escritos_buffer);
         // Reiniciamos las variables de:
         size_disponible_buffer = 128;
         bytes_escritos_buffer = 0;
         offset = false;
         Logger::log(LogLevel::DEBUG, "Se realiza una escritura del buffer");
      };
      // Escribimos los datos si estamos en la ultima iteración del bucle:
      if(it.is_eof() && contador_columnas>=n_cols){
         out.write(buffer.data(), bytes_escritos_buffer);
         out.flush();
      };
   };
   if(it.is_eof() && contador_columnas>=n_cols){
      Logger::log(LogLevel::DEBUG, "ES EL FIN DE LA RECUPERACION DE FILAS");
   };

   // Antes de cerrar la escritura, escribimos el buffer de escritura:
   //disk_io::aux_vector_buffer_write_disk(buffer, out);
   out.close();
};

//========================================================
//== FUNION LECTURA DATOS: ===============================
//========================================================


// Funcion auxliar para leer un valor concreto en disco:
Values disk_io::read_aux_val(dataType tipo_dato, std::ifstream& in){

   Values value;
   switch(tipo_dato){

      case dataType::INT: {
         int buffer;
         in.read(reinterpret_cast<char*>(&buffer), sizeof(int));
         value = buffer;
         break;
      };
      case dataType::FLOAT: {
         float buffer;
         in.read(reinterpret_cast<char*>(&buffer), sizeof(float));
         value = buffer;
         break;
      };
      case dataType::BOOL: {
         bool buffer_bool;
         uint8_t buffer_int8;
         in.read(reinterpret_cast<char*>(&buffer_int8), sizeof(uint8_t));
         if(buffer_int8==1){
            buffer_bool = true;
         }else{
            buffer_bool = false;
         };
         value = buffer_bool;
         break;
      };
      case dataType::STRING: {
         std::string buffer;
         uint32_t string_size;
         // Primero escribimos el tamaño de la string:
         in.read(reinterpret_cast<char*>(&string_size), sizeof(uint32_t));
         // Ajustamos el tamaño de la cadena de texto donde escribiremos:
         buffer.resize(string_size);   // ← reservar memoria
         // Ahora ya si escribimos la cadena de texto:
         in.read(buffer.data(), string_size);
         /* No es necesario 'reinterpret_cast<char*>' porque data()
         de una std::string ya es un puntero a un caracter de texto
         */
         value = buffer;
         break;
      };
   };
   return value;
};


void disk_io::read_table_data(table*& tabla){
   std::string nombre_tabla = tabla->metadata_ptr->name;
   std::string ruta_tabla = "data/" + nombre_tabla + "_data.bin";
   std::ifstream in(ruta_tabla, std::ios::binary);
   if(Logger::level == LogLevel::DEBUG){
      Logger::flush();
   }else{
      Logger::flush(false);
   };
   
   // Obtenemos los datos necesarios para la lectura:
   table_metadata* metadata = tabla->metadata_ptr;
   std::vector<dataType> tipos_datos = tabla->metadata_ptr->column_types;
   uint32_t n_cols = tipos_datos.size();
   std::vector<std::string> col_names = tabla->metadata_ptr->column_names;
   uint32_t n_filas;

   // Donde guardaremos los datos:
   std::map<std::string, std::vector<Values>>& columnas = tabla->data_buffer_ptr->columns;

   // Creamos el objeto de iteració para 
   // Creamos el iterador por filas:
   std::map<std::string, Values> fila_a_escribir;
   Values valor_tmp;


   // Primero de todo, leemos las filas a leer:
   //uint32_t n_filas = 0;
   in.read(reinterpret_cast<char*>(&n_filas), sizeof(uint32_t));
   // Ahora iteramos por otodas las filas:
   for(int i = 0; i<n_filas; i++){
      for(int j = 0; j<n_cols; j++){
         // Usamos una función auxliar para leer los datos según su tipo:
         valor_tmp = disk_io::read_aux_val(tipos_datos[j], in);
         // Hemos recuperado el valor j de la fila i
         // Aho0ra rellenamos el vector correspondiente:
         if(columnas.find(col_names[j]) == columnas.end()){
            columnas[col_names[j]].push_back(valor_tmp);
         }else{
            columnas.at(col_names[j]).push_back(valor_tmp);
         };
      };
   };
   in.close();
};



void disk_io::read_table_metadata(std::filesystem::path ruta_tabla, std::string nombre_tabla_str){
   std::string tabla_nombre = ruta_tabla.stem().string();
   std::string ruta_tabla_str = ruta_tabla.string();

   table_metadata* metadatos_puntero = global_table_dict.at(nombre_tabla_str)->metadata_ptr;

   // COMIENZA LA LECTURA:
   std::ifstream in(ruta_tabla_str, std::ios::binary);

   // Leemos el tamaño del nombre:
   uint32_t size_nombre = 0;
   in.read(reinterpret_cast<char*>(&size_nombre), sizeof(uint32_t));

   // Leemos el nombre:
   std::string nombre_tabla(size_nombre, '\0');
   in.read(&nombre_tabla[0], size_nombre);
   metadatos_puntero->name = nombre_tabla;

   // Leemos el numero de columnas:
   uint32_t num_cols_lectura = 0;
   in.read(reinterpret_cast<char*>(&num_cols_lectura), sizeof(uint32_t));
   // Este vslor no se escribe, lo usaremos para iterar por cada columna

   // Leemos el numero de filas en disco:
   uint32_t num_filas_disco_lectura = 0;
   in.read(reinterpret_cast<char*>(&num_filas_disco_lectura), sizeof(uint32_t));
   metadatos_puntero->n_filas_disco = num_filas_disco_lectura;

   // Ahora iteramos poe cada columna, añadiendo metadatos de cada una:
   for(uint32_t i=0; i<num_cols_lectura; i++){
      // Leemos el nombre de la columna:
      uint32_t size_column_name;
      in.read(reinterpret_cast<char*>(&size_column_name), sizeof(uint32_t));
      std::string columna_nombre_leido(size_column_name, '\0');
      in.read(&columna_nombre_leido[0], size_column_name);
      metadatos_puntero->column_names.push_back(columna_nombre_leido);

      // Leemos el tipo de dato:
      uint32_t col_type_int;
      in.read(reinterpret_cast<char*>(&col_type_int), sizeof(uint32_t));
      dataType col_type = static_cast<dataType>(col_type_int);
      metadatos_puntero->column_types.push_back(col_type);

      // Recuperamos si esclave primaria:
      uint8_t column_is_key_num;
      bool column_is_key;
      in.read(reinterpret_cast<char*>(&column_is_key_num), sizeof(uint8_t));
      if(column_is_key_num == 1){
         column_is_key = true;
      } else {
         column_is_key = false;
      };
      metadatos_puntero->primary_list.push_back(column_is_key);

      // Nos saltsmos el num cols de size uint32_t
      // En realidad no necesitamos leer nafa mas

   };
   Logger::log(LogLevel::ERROR, "Metadatos leidos con exito");
   if(Logger::level == LogLevel::DEBUG){
      Logger::flush();
   }else{
      Logger::flush(false);
   };
   in.close();
};










   // Funcion auxilar recursiva:
void disk_io::escanear_tablas_recursiva(const std::filesystem::path& ruta, std::vector<std::filesystem::path>& arr_tablas){
   // Iteramos por cada elemento:
   for (const auto& entrada : fs::directory_iterator(ruta)){
      // Caso de quecsea una ruta:
      if (fs::is_directory(entrada)){
         disk_io::escanear_tablas_recursiva(entrada.path(), arr_tablas);
      } else if(fs::is_regular_file(entrada) && entrada.path().extension().string() == ".bin" && entrada.path().filename().string().find("_meta") != std::string::npos){
         /*
         Caso base de la recursion
         Solo adicionamos metadatos
         */
         arr_tablas.push_back(entrada);
      };
   };
};

// SOLO ESCANEA LOS METADATOS DE LAS TABLAS:
std::vector<std::filesystem::path> disk_io::escanear_tablas(){

      fs::path ruta_tablas = "data";
      // Vector con el nombre de todas las tablas:
      std::vector<std::filesystem::path> arr_tablas;

      disk_io::escanear_tablas_recursiva(ruta_tablas, arr_tablas);
      return arr_tablas;
   };


void disk_io::lectura_metadatos_todas_tablas(){
   
   std::vector<std::filesystem::path> arr_tablas;
   arr_tablas = disk_io::escanear_tablas();
   for(int i = 0; i<arr_tablas.size(); i++){
      // Creamos el objeto de la tabla y su entrada en el diccionario global:
      // Inicializamos el diccionsrio global y sus elementos:
      table* tabla;
      // REGISTRAMOS EL NOMBRE DE LA TABLA SIN ESE '_meta':
      std::string nombre_tabla_str = arr_tablas[i].stem().string();
      if(nombre_tabla_str.ends_with("_meta")){
         nombre_tabla_str.erase(nombre_tabla_str.size() -5);
      };
      global_table_dict[nombre_tabla_str] = new table;
      tabla = global_table_dict.at(nombre_tabla_str);
      tabla->metadata_ptr = new table_metadata();
      tabla->data_ptr = new table_data();
      // tabla->data_buffer_ptr = new table_data_buffer(); No lo inicializamos hasta que se llame al constructor de 'tableRowIterator'
      disk_io::read_table_metadata(arr_tablas[i], nombre_tabla_str);
   };
   disk_io::debug_print_metadatos_memoria();
};



   // FUNCIÓN DE ESCRITURA DE TODAS LAS TABLAS:


void disk_io::write_dump(){

   // Primero vemos si el diccionario esta vacio o no:
   if(global_table_dict.empty()){
      // El diccionario esta vacio, salimos
      Logger::log(LogLevel::DEBUG, "EL DICCIONARIO DE TABLAS NO EXISTE. SALIMOS");
      return;
   }else{
      //El diccionario tiene contenido:
      for (const auto& [table_name, table_ptr] : global_table_dict){
         int32_t n_rows;
         // Solo enviamos a escribir si la tabla existe:
         if (table_ptr != nullptr){
            // Sólo permitimos la escritura si el dat_ptr (no confundir con el buffer del disco) NO está vacío:
            if(table_ptr->data_ptr){
               if(!table_ptr->data_ptr->columns.empty()){

                  // Primero escribimos los metadatos:
                  uint32_t n_rows = 0;

                  n_rows = disk_io::write_table_metadata(table_ptr);
                  //disk_io::write_table_data(table_ptr, n_rows);
                  disk_io::write_table_data_with_buffer(table_ptr, n_rows);

                  // Actualizamos los metadatos de la tabla para mostar el numero de filas escritas en disco:
                  table_ptr->metadata_ptr->n_filas_disco = n_rows;
               } else {
                  Logger::log(LogLevel::DEBUG, "EXISTE la tabla en el diccionario, pero NO tiene datos en RAM viva. NO LA ESCRIBIMOS");
               };
            } else {
               Logger::log(LogLevel::DEBUG, "EXISTE la tabla en el diccionario. NO EXISTE SU ENTRADA DE DATOS. NO LA ESCRIBIMO0S");
            };
         }else{
            Logger::log(LogLevel::DEBUG, "ERROR: Se ha encontrado una entrada vacia para la tabla: " + table_name);
         };
      };
   };

   // Justo Antes de concluir la escritura, eliminamos el archivo WAL:
   Logger::log(LogLevel::DEBUG, "ELIMINAMOS EL ARCHIVO WAL DE DATOS DE BACKUP");
   disk_wal::delete_wal_bin_file();
};


void disk_io::debug_print_metadatos_memoria() {
   Logger::log(LogLevel::DEBUG, "======= DEBUG DE METADATOS EN MEMORIA =======");
    
   if (global_table_dict.empty()) {
      Logger::log(LogLevel::ERROR, "El diccionario global de tablas está VACÍO.");
      return;
    };

   for (auto const& [nombre, tabla_ptr] : global_table_dict) {
      Logger::log(LogLevel::DEBUG, "TABLA (Dicc): " + nombre);
      
      if (!tabla_ptr || !tabla_ptr->metadata_ptr) {
         Logger::log(LogLevel::ERROR, "  [!] Error: Puntero a tabla o metadatos es NULL");
         continue;
      };

      table_metadata* meta = tabla_ptr->metadata_ptr;
      Logger::log(LogLevel::DEBUG, "  Nombre en Struct: " + meta->name);
      Logger::log(LogLevel::DEBUG, "  Filas en disco:   " + std::to_string(meta->n_filas_disco));
      
      size_t n_cols = meta->column_names.size();
      size_t n_types = meta->column_types.size();
      size_t n_pks = meta->primary_list.size();

      Logger::log(LogLevel::DEBUG, "  Sincronización de Vectores:");
      Logger::log(LogLevel::DEBUG, "    - Nombres: " + std::to_string(n_cols));
      Logger::log(LogLevel::DEBUG, "    - Tipos:   " + std::to_string(n_types));
      Logger::log(LogLevel::DEBUG, "    - PKs:     " + std::to_string(n_pks));

      Logger::log(LogLevel::DEBUG, "  DETALLE DE COLUMNAS:");
      // Iteramos sobre el máximo encontrado para detectar desajustes
      size_t max_idx = std::max({n_cols, n_types, n_pks});
      
      for (size_t i = 0; i < max_idx; i++) {
         std::string col_name = (i < n_cols) ? meta->column_names[i] : "!!! MISSING NAME !!!";
         std::string tipo_str = "NOT_READ";
         
         if (i < n_types) {
            switch(meta->column_types[i]) {
               case dataType::INT:    tipo_str = "INT"; break;
               case dataType::FLOAT:  tipo_str = "FLOAT"; break;
               case dataType::STRING: tipo_str = "STRING"; break;
               case dataType::BOOL:   tipo_str = "BOOL"; break;
               case dataType::UNKNOWN: tipo_str = "UNKNOWN"; break;
               // El compilador ya no se quejará, cubrimos todo el enum
               }
         }
         
         std::string es_pk = (i < n_pks && meta->primary_list[i]) ? "[PK]" : "    ";
         
         Logger::log(LogLevel::DEBUG, "    [" + std::to_string(i) + "] " + es_pk + " " + 
                     col_name + " (" + tipo_str + ")");
      }
   };
   Logger::log(LogLevel::DEBUG, "==============================================");
   if(Logger::level == LogLevel::DEBUG){
   Logger::flush();
   }else{
   Logger::flush(false);
   };
};