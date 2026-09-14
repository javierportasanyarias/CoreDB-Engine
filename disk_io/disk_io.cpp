#include "disk_wal_utils.h"
#include "disk_metadata.h"
#include "disk_out.h"
#include "disk_io.h"


////////////////////////////////////////////////////////////////////
// WRITE DUMP //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

// Fución que itera sobre las tablas en las que escribir:
void disk_io::write_dump(){

   // Primero vemos si el diccionario esta vacio o no:
   if(global_table_dict.empty()){
      // El diccionario esta vacio, salimos
      Logger::log(LogLevel::DEBUG, "TABLE HASH MAP DOES NOT EXIST. PROGRAM TERMINATION");
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
                  table_ptr->metadata_ptr->n_rows_disk = n_rows;
               } else {
                  Logger::log(LogLevel::DEBUG, "Table dies EXIST within th hash map, but there is no data in the volatile memory. WRITE OPERATION CANCELED");
               };
            } else {
               Logger::log(LogLevel::DEBUG, "Table dies EXIST within th hash map. There is no data entry. WRITE OPERATION CANCELED");
            };
         }else{
            Logger::log(LogLevel::DEBUG, "ERROR: Table entry does not exist in global table dictionary. Table name is: " + table_name);
         };
      };
   };

   // Justo Antes de concluir la escritura, eliminamos el archivo WAL:
   Logger::log(LogLevel::DEBUG, "WAL BACKUP FILE DELETION");
   disk_wal_utils::delete_wal_bin_file();
};


void disk_io::debug_print_mem_metadata() {
   Logger::log(LogLevel::DEBUG, "======= METADATA IN MEMORY DEBUG =======");
    
   if (global_table_dict.empty()) {
      Logger::log(LogLevel::ERROR, "Global table dictionary is EMPTY.");
      return;
    };

   for (auto const& [name_tmp, table_ptr] : global_table_dict) {
      Logger::log(LogLevel::DEBUG, "TABLE (Dicc): " + name_tmp);
      
      if (!table_ptr || !table_ptr->metadata_ptr) {
         Logger::log(LogLevel::ERROR, "  [!] Error: Table or metadata pointers are NULL");
         continue;
      };

      table_metadata* meta = table_ptr->metadata_ptr;
      Logger::log(LogLevel::DEBUG, "  Struct name: " + meta->name);
      Logger::log(LogLevel::DEBUG, "  Rows in disk:   " + std::to_string(meta->n_rows_disk));
      
      size_t n_cols = meta->n_cols;
      size_t n_types = meta->column_types.size();
      size_t n_pks = meta->primary_list.size();

      Logger::log(LogLevel::DEBUG, "  Data vectors:");
      Logger::log(LogLevel::DEBUG, "    - Names: " + std::to_string(n_cols));
      Logger::log(LogLevel::DEBUG, "    - Types:   " + std::to_string(n_types));
      Logger::log(LogLevel::DEBUG, "    - PKs:     " + std::to_string(n_pks));

      Logger::log(LogLevel::DEBUG, "  COLUMNS DETAIL:");
      // Iteramos sobre el máximo encontrado para detectar desajustes
      size_t max_idx = std::max({n_cols, n_types, n_pks});
      
      for (size_t i = 0; i < max_idx; i++) {
         std::string col_name = (i < n_cols) ? meta->column_names[i] : "!!! MISSING NAME !!!";
         std::string type_str = "NOT_READ";
         
         if (i < n_types) {
            switch(meta->column_types[i]) {
               case dataType::INT:
                  type_str = "INT";
                  break;
               case dataType::FLOAT:
                  type_str = "FLOAT";
                  break;
               case dataType::STRING:
                  type_str = "STRING";
                  break;
               case dataType::BOOL:
                  type_str = "BOOL";
                  break;
               case dataType::UNKNOWN:
                  type_str = "UNKNOWN";
                  break;
               }
         }
         
         std::string is_pk = (i < n_pks && meta->primary_list[i]) ? "[PK]" : "    ";
         
         Logger::log(LogLevel::DEBUG, "    [" + std::to_string(i) + "] " + is_pk + " " +  col_name + " (" + type_str + ")");
      }
   };
   Logger::log(LogLevel::DEBUG, "==============================================");
   if(Logger::level == LogLevel::DEBUG){
   Logger::flush(LogLevel::DEBUG);
   }else{
   Logger::flush(LogLevel::DEBUG, false);
   };
};