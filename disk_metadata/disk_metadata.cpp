

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
#include "part_sort.h"
#include "disk_aux.h"
#include "disk_metadata.h"



////////////////////////////////////////////////////////////////////
// METADATOS ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void disk_metadata::read_table_metadata(std::filesystem::path ruta_tabla, std::string nombre_tabla_str){
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


uint32_t disk_metadata::write_table_metadata(table* tabla){
   /*
   Función que escribe los metadatos en disco.
   */
   if (!tabla) return 0;
   std::string nombre_tabla = tabla->metadata_ptr->name;
   std::string ruta_tabla = "metadata/" + nombre_tabla + "_meta.bin";

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
       disk_aux::aux_vector_buffer_write_disk(buffer,
                                             out
                                            );
      out.flush();
      out.close();
      return n_rows;
   };
   out.close();
   return 0;
};

////////////////////////////////////////////////////////////////////
// LECTURA DE TODOS LOS METADATOS //////////////////////////////////
////////////////////////////////////////////////////////////////////


void disk_metadata::lectura_metadatos_todas_tablas(){
   
   std::vector<std::filesystem::path> arr_tablas;
   arr_tablas = disk_aux::escanear_tablas();
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
      disk_metadata::read_table_metadata(arr_tablas[i], nombre_tabla_str);
   };
   disk_io::debug_print_metadatos_memoria();
};
