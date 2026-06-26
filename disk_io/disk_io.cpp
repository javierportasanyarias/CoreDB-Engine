

#include "data_struct.h"
#include <iostream>
#include <fstream>
#include <string>
#include "globals.h"
#include <map>
#include "logging.h"
#include "filesystem"
#include "disk_io.h"
#include "disk_wal_utils.h"
#include "disk_buffer.h"
#include <cstring> // Para usar std::memcpy
#include "part_sort.h"
#include "disk_metadata.h"
#include "disk_out.h"



////////////////////////////////////////////////////////////////////
// ESCRITURA MASIVA ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

// Fución que itera sobre las tablas en las que escribir:
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

                  n_rows = disk_metadata::write_table_metadata(table_ptr);
                  disk_out::write_table_data(table_ptr);

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
   disk_wal_utils::delete_wal_bin_file();
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
      
      size_t n_cols = meta->n_cols;
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