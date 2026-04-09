

#include "data_struct.h"
#include <iostream>
#include <fstream>
#include <string>
#include "globals.h"
#include <map>
#include "logging.h"
#include "filesystem"
#include "disk_io.h"
#include "disk_buffer.h"

void disk_io::aux_vector_buffer_write_disk(std::vector<char> buffer, std::ofstream& out){

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


//========================================================
//== FUNION ESCRITURA METADATOS EN EL WAL: ===============
//========================================================
void disk_io::write_table_wal_metadata(table* tabla){
   Logger::log(LogLevel::DEBUG, "Dentro de la escritura de metadatos en el WAL");
   /*
   Función que escribe los metadatos en el archivo WAL para
   persistecia de los datos
   */
   if (!tabla) return;
   std::string nombre_tabla = tabla->metadata_ptr->name;
   // std::string ruta_tabla = "data/" + nombre_tabla + "_meta.bin";

   // Abrimos la escritura:
   //std::ofstream out("backup_data/wal.bin", std::ios::binary | std::ios::out);
   std::ofstream out("backup_data/wal.bin", std::ios::binary | std::ios::app);
   Logger::log(LogLevel::DEBUG, "Ya se ha abierto el archivo");


   /*
   Usamos un vector de punteros de caracteres para así no 
   realizar tantas llamadas a la escritura
   */
   std::vector<char> buffer;
   char* tmp_char_ptr = nullptr;

   /*
   Al ser una escritura en el WAL, antes debemos escribir por separado y antes:
   -> Tipo de datos (1 byte):
      * 0: metadatos
      * 1: datos
   -> Tamaño (4 bytes):
      Almacena el tamaño en bytes de la información en sí
   */

   // == Escribimos los metadatos: ===============================

   // -- Escribimos el nombre -----------------------------------------
   Logger::log(LogLevel::DEBUG, "Pasamos a escribir el nombre de la tabla:");
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
   Logger::log(LogLevel::DEBUG, "Nombre de la tabla registrado en el buffer con exito");
   // -- Escribimos el número de columnas -----------------------------
   uint32_t num_cols = (tabla->metadata_ptr->column_names).size();
   //out.write(reinterpret_cast<char*>(&num_cols), sizeof(uint32_t));
   tmp_char_ptr = reinterpret_cast<char*>(&num_cols);
   buffer.insert(buffer.end(),
                 tmp_char_ptr,
                 tmp_char_ptr + sizeof(uint32_t)
                );
   Logger::log(LogLevel::DEBUG, "Numero de columnas registrado en el buffer con exito");
   /* En caso de escribir en el WAL los metadatos,
   siempre va a estar vacios los datos, por lo que el conteo de
   filas en RAM y recupoeradas del disco serán cero
   Además los datos no existen todavía, por lo que no podremos
   accedr a "data_ptr" de la tabla sin que de error
   */
   int varlo_tmp_int = 0;
   tmp_char_ptr = reinterpret_cast<char*>(&varlo_tmp_int);
   buffer.insert(buffer.end(),
               tmp_char_ptr,
               tmp_char_ptr + sizeof(uint32_t)
               );
    Logger::log(LogLevel::DEBUG, "Numero de filas registrado en el buffer con exito. Al ser escritura en el WAL SIEMPRE sera cero");
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
   Logger::log(LogLevel::DEBUG, "Metadatos ya registrados en el buffer a escribir");
   Logger::log(LogLevel::DEBUG, "Al ser escritura en el WAL escribimos: el tipo de dato y su tamaño");
   // Ya tenemos el buffer listo para escritura, pero al ser la escritura en WAL, antes debemos
   // escribir el tipo y el tamaño de la información.

   // Escribimos el ipo de dato (metadato)
   Logger::log(LogLevel::DEBUG, "Registramos el tipo de dato");
   uint8_t valorCero = 0;
   out.write(reinterpret_cast<const char*>(&valorCero), sizeof(uint8_t));
   Logger::log(LogLevel::DEBUG, "Tipo de dato escrito con exito. Al ser metadato es cero");
   // Escribimos el tamaño de los metadatos:
   Logger::log(LogLevel::DEBUG, "Registramos el tamaño del buffer de los metadatos");
   //uint32_t buffer_size = static_cast<uint32_t>(buffer.size());
   uint32_t buffer_size = sizeof(buffer.data());
   out.write(reinterpret_cast<char*>(&buffer_size), sizeof(uint32_t));
   Logger::log(LogLevel::DEBUG, "Tamaño del buffer de datos escito con exito");
   Logger::log(LogLevel::DEBUG, "Pasamos a escribir el buffer de datos en disco:");
   // Ahora ya sí podemos escribir el contenido del buffer en sí
   disk_io::aux_vector_buffer_write_disk(buffer,
                                         out
                                         );
   Logger::log(LogLevel::DEBUG, "Buffer de datos escrito con EXITO");
   out.flush();
   out.close();
   return;
};



// Funcion auxliar para escribir un valor concreto en disco:
void disk_io::write_aux_val(Values value, dataType tipo_dato, std::fstream& out){

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

   // Ahora iteraremos hasta que esteé vacía la fila a escribir
   while(!it.is_eof()){
      fila_a_escribir = it.get_next_row_ram_viva();
      // Ahora iteramos por cada columna:
      for(int i = 0; i<n_cols; i++){
         // valor_tmp = fila_a_escribir[i]; // Obtenemos el valor de una fila y columna concretos
         valor_tmp = fila_a_escribir.at(columnas_nombre[i]); // Obtenemos el valor de una fila y columna concretos
         disk_io::write_aux_val(valor_tmp, tipos_datos[i], out);
      };
   };
   out.flush();
   out.close();
};

//========================================================
//== FUNION ESCRITURA DATOS EN EL WAL: ===================
//========================================================

void disk_io::write_table_data_wal(table* tabla, uint32_t n_rows_a_escribir){

   if (!tabla) return;
   std::string nombre_tabla = tabla->metadata_ptr->name;
   /*std::string ruta_tabla = "data/" + nombre_tabla + "_data.bin";

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
   };*/
   std::ofstream out("backup_data/wal.bin", std::ios::binary | std::ios::app);
   Logger::log(LogLevel::DEBUG, "Ya se ha abierto el archivo");

   table_metadata* metadata = tabla->metadata_ptr;
   std::vector<dataType> tipos_datos = tabla->metadata_ptr->column_types;
   std::vector<std::string> columnas_nombre = tabla->metadata_ptr->column_names;
   uint32_t n_cols = tipos_datos.size();
   uint32_t n_filas = metadata->n_filas_ram;

   // Creamos el iterador por filas:
   auto it = disk_buffer::tableRowIterator_only_ram_for_wal(nombre_tabla, n_rows_a_escribir);
   std::map<std::string, Values> fila_a_escribir;
   Values valor_tmp;

   Logger::log(LogLevel::DEBUG, "Filas insertadas que escribiremos en el WAL: ", false, true);
   Logger::log(LogLevel::DEBUG, n_rows_a_escribir, true, false);
   out.write(reinterpret_cast<char*>(&n_rows_a_escribir), sizeof(uint32_t));

   // Ahora nos movemos al final para poder escribir sólo al final

   // Ahora iteraremos hasta que esteé vacía la fila a escribir
   Logger::log(LogLevel::DEBUG, "Pasamos a la iteración de escribir las filas");
   Logger::log(LogLevel::DEBUG, " ");
   Logger::log(LogLevel::DEBUG, "//////////////////////////////////////////////////////////////");
   while(!it.is_eof()){
      fila_a_escribir = it.get_next_row_ram_viva();
      Logger::log(LogLevel::DEBUG, "Insertamos la fila: ", false, true);
      Logger::log(LogLevel::DEBUG, it.contador, true, false);
      // Ahora iteramos por cada columna:
      for(int i = 0; i<n_cols; i++){
         // valor_tmp = fila_a_escribir[i]; // Obtenemos el valor de una fila y columna concretos
         Logger::log(LogLevel::DEBUG, "Pasamos a recuperar la variable 'valor_tmp'");
         valor_tmp = fila_a_escribir.at(columnas_nombre[i]); // Obtenemos el valor de una fila y columna concretos
         Logger::log(LogLevel::DEBUG, "Variable 'valor_tmp' recuperada con exito");
         disk_io::write_aux_val(valor_tmp, tipos_datos[i], out);
         Logger::log(LogLevel::DEBUG, "Escritura de la fila: ", false, true);
         Logger::log(LogLevel::DEBUG, it.contador, false, false);
         Logger::log(LogLevel::DEBUG, " terminada con exito", true, false);
         Logger::flush();
      };
      Logger::flush();
   };
   Logger::flush();
   out.flush();
   out.close();
};

//========================================================
//== FUNION LECTURA DATOS: ===============================
//========================================================


// Funcion auxliar para escribir un valor concreto en disco:
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

//========================================================
//== ELIMINAR EL ARCHIVO WAL ENTERO: =====================
//========================================================


void disk_io::delete_wal_bin_file(){
   if(fs::remove("backup_data/wal.bin")){
      Logger::log(LogLevel::DEBUG, "Archivo WAl eliminado con EXITO");
   } else {
      Logger::log(LogLevel::DEBUG, "El archivo WAL no existía, por lo que NO ha sido eliminado");
   };
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
                  disk_io::write_table_data(table_ptr, n_rows);
                  // Actualizamos los metadatos de la tabla para mostar el numero de filas escritas en disco:
                  table_ptr->metadata_ptr->n_filas_disco = n_rows;
                  return;
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
   disk_io::delete_wal_bin_file();
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