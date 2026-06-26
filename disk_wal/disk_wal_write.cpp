#include "data_struct.h"
#include <iostream>
#include <fstream>
#include <string>
#include "globals.h"
#include <map>
#include "logging.h"
#include "filesystem"
#include "disk_aux.h"
#include "disk_aux.h"
#include "disk_metadata.h"
#include "disk_wal_write.h"

//========================================================
//== FUNION ESCRITURA METADATOS EN EL WAL: ===============
//========================================================
void disk_wal_write::write_table_wal_metadata(table* tabla){
   /*
   Función que escribe los metadatos nada más han sido creados en la memoria RAM.
   Lo que se escribe en el bloque de matadatos es lo siguiente:
      1º Tipo de dato: 0 porque es metadato.
      2º Tamaño del nombre de la tabla.
      3º Nombre de la tabla.
      4º Tamaño del buffer de datos
      5º Buffer de datos:
   El número de filas totales se escribe siempre a cero, a diferencia de los
   metadatos escritos de forma normal, no en el wal.

   */
   Logger::log(LogLevel::DEBUG, "Dentro de la escritura de metadatos en el WAL");
   /*
   Función que escribe los metadatos en el archivo WAL para
   persistecia de los datos
   */
   if (!tabla) return;
   // We do not make metadata pointer an alias as we do not need to modify any value
   table_metadata* metadatos_puntero = tabla->metadata_ptr;
   std::string nombre_tabla = metadatos_puntero->name;
 
   // Volvemos a abrir el archivo ,esta vez en modo append:
   std::ofstream out("backup_data/wal.bin", std::ios::binary | std::ios::app);
   //out.seekp(0, std::ios::end); // Volvemos al final para escribir el nuevo bloque de metadatos


   /*
   Usamos un vector de punteros de caracteres para así no 
   realizar tantas llamadas a la escritura
   */

   uint32_t meta_byte_size = disk_metadata::calculate_metadata_byte_size(metadatos_puntero);
   //std::vector<char> buffer;
   char* buffer = nullptr;
   char* tmp_char_ptr = nullptr;
   try {
      buffer = new char[meta_byte_size];
      char* ptr_curr = buffer;
      /*
      Al ser una escritura en el WAL, antes debemos escribir por separado y antes:
      -> Tipo de datos (1 byte):
         * 0: metadatos
         * 1: datos
      -> Tamaño (4 bytes):
         Almacena el tamaño en bytes de la información en sí
      */
      // Escribimos el tipo de dato
      uint8_t tipo_dato = 0; // 0 porque es un metadato
      out.write(reinterpret_cast<char*>(&tipo_dato), sizeof(uint8_t));
      // == Escribimos los metadatos: ===============================

      // -- Escribimos el nombre -----------------------------------------
      Logger::log(LogLevel::DEBUG, "Pasamos a escribir el nombre de la tabla:");

      uint32_t size_nombre = nombre_tabla.size();
      out.write(reinterpret_cast<char*>(&size_nombre), sizeof(uint32_t));
      out.write(nombre_tabla.data(), size_nombre);



      Logger::log(LogLevel::DEBUG, "Nombre de la tabla registrado en el buffer con exito");
      // -- Escribimos el número de columnas -----------------------------
      uint32_t num_cols = metadatos_puntero->n_cols;
      Logger::flush();
      Logger::log(LogLevel::DEBUG, "=========================================");
      Logger::log(LogLevel::DEBUG, "Nº de columnas escritas en los metadatos del WAL: ", false, true);
      Logger::log(LogLevel::DEBUG, num_cols, true, false);
      Logger::log(LogLevel::DEBUG, "=========================================");
      Logger::flush();
      tmp_char_ptr = reinterpret_cast<char*>(&num_cols);
      //buffer.insert(buffer.end(),
                  //tmp_char_ptr,
                  //tmp_char_ptr + sizeof(uint32_t)
                  //);
      std::memcpy(ptr_curr, tmp_char_ptr, sizeof(uint32_t));
      ptr_curr += sizeof(uint32_t);
      Logger::log(LogLevel::DEBUG, "Numero de columnas registrado en el buffer con exito");
      /* En caso de escribir en el WAL los metadatos,
      siempre va a estar vacios los datos, por lo que el conteo de
      filas en RAM y recupoeradas del disco serán cero
      Además los datos no existen todavía, por lo que no podremos
      accedr a "data_ptr" de la tabla sin que de error
      */
      //int varlo_tmp_int = 0;
      //tmp_char_ptr = reinterpret_cast<char*>(&varlo_tmp_int);
      //buffer.insert(buffer.end(),
                  //tmp_char_ptr,
                  //tmp_char_ptr + sizeof(uint32_t)
                  //);
      //std::memcpy(ptr_curr, tmp_char_ptr, sizeof(uint32_t));
      //ptr_curr += sizeof(uint32_t);
      //Logger::log(LogLevel::DEBUG, "Numero de filas registrado en el buffer con exito. Al ser escritura en el WAL SIEMPRE sera cero");
      for(uint32_t i=0; i<num_cols; i++){
         // -- Escribimos los datos de cada columna ---------------------
         // -- Escribimos el nombre:
         Logger::log(LogLevel::DEBUG, "Iteracion del bucle: ", false, true);
         Logger::log(LogLevel::DEBUG, i, true, false);
         std::string column_name = (metadatos_puntero->column_names)[i];
         uint32_t size_column_name = column_name.size();
         Logger::log(LogLevel::DEBUG, "'column_name' recuperada y su tamaño");
         Logger::flush();

         Logger::log(LogLevel::DEBUG, "Insertamos el numero de filas");
         tmp_char_ptr = reinterpret_cast<char*>(&size_column_name);
         //buffer.insert(buffer.end(),
                     //tmp_char_ptr,
                     //tmp_char_ptr + sizeof(uint32_t)
                     //);
         std::memcpy(ptr_curr, tmp_char_ptr, sizeof(uint32_t));
         ptr_curr += sizeof(uint32_t);
         Logger::log(LogLevel::DEBUG, "Número de filas escrito en el buffer con exito");

         Logger::log(LogLevel::DEBUG, "Insertamos el nombre de la columna");
         tmp_char_ptr = column_name.data();
         //buffer.insert(buffer.end(),
                     //tmp_char_ptr,
                     //tmp_char_ptr + size_column_name
                     //);
         std::memcpy(ptr_curr, tmp_char_ptr, size_column_name);
         ptr_curr += size_column_name;
         Logger::log(LogLevel::DEBUG, "Nombre de la columna escrito en el buffer con exito");
         // -- Escribimos el tipo de dato:
         Logger::log(LogLevel::DEBUG, "Insertamos el tipo de dato");
         dataType col_type = (metadatos_puntero->column_types)[i];
         uint32_t col_type_disk = static_cast<uint32_t>(col_type);
                  
         tmp_char_ptr = reinterpret_cast<char*>(&col_type_disk);
         //buffer.insert(buffer.end(),
                     //tmp_char_ptr,
                     //tmp_char_ptr + sizeof(uint32_t)
                     //);
         std::memcpy(ptr_curr, tmp_char_ptr, sizeof(uint32_t));
         ptr_curr += sizeof(uint32_t);
         Logger::log(LogLevel::DEBUG, "Tipo de dato escrito en el buffer con exito");
         // -- Escribimos si es clave primaria:
         Logger::log(LogLevel::DEBUG, "Accedemos primero a si es clave primaria o no");
         bool column_is_key = (metadatos_puntero->primary_list)[i];
         Logger::log(LogLevel::DEBUG, "'column_is_key' acceso correcto");
         uint8_t key_val = column_is_key ? 1 : 0;
         Logger::log(LogLevel::DEBUG, "Booleano convertino a número con éxito");
         Logger::log(LogLevel::DEBUG, "Ahora ya si pasamos a escribir el booleano convertido en el buffer");
         tmp_char_ptr = reinterpret_cast<char*>(&key_val);
         //buffer.insert(buffer.end(),
                     //tmp_char_ptr,
                     //tmp_char_ptr + sizeof(uint8_t)
                     //);
         std::memcpy(ptr_curr, tmp_char_ptr, sizeof(uint8_t));
         ptr_curr += sizeof(uint8_t);
         Logger::log(LogLevel::DEBUG, "Booleano de clave primaria escrito en el buffer con exito");
      };
      Logger::log(LogLevel::DEBUG, "Metadatos ya registrados en el buffer a escribir");
      Logger::log(LogLevel::DEBUG, "Al ser escritura en el WAL escribimos: el tipo de dato y su tamaño");
      /* Ya tenemos el buffer listo para escritura, pero al ser la escritura en WAL, antes debemos
      escribir el tamaño de la información.
      */
      // Escribimos el tamaño de los metadatos:
      Logger::log(LogLevel::DEBUG, "Registramos el tamaño del buffer de los metadatos");
      //uint32_t buffer_size = buffer.size();
      //out.write(reinterpret_cast<char*>(&buffer_size), sizeof(uint32_t));
      out.write(reinterpret_cast<char*>(&meta_byte_size), sizeof(uint32_t));
      Logger::log(LogLevel::DEBUG, "Tamaño del buffer de datos escito con exito");
      Logger::log(LogLevel::DEBUG, "Pasamos a escribir el buffer de datos en disco:");
      // Ahora ya sí podemos escribir el contenido del buffer en sí
      disk_aux::aux_vector_buffer_write_disk(buffer, 
                                             meta_byte_size,
                                             out
                                             );
      Logger::log(LogLevel::DEBUG, "Buffer de datos escrito con EXITO");
      out.flush();
   }catch(...){
      delete[] buffer;
      out.close();
      throw;
   };
   delete[] buffer;
   out.close();
   return;
};

//========================================================
//==== FUNION ESCRITURA DATOS EN EL WAL: =================
//========================================================


void disk_wal_write::walDataWriter::control_unit(){
   switch(this->state){
      case 0: {
         Logger::log(LogLevel::DEBUG, "State 0");
         if (!this->table_obj){
            this->state = 255;
            break;
         };
         table_metadata* metadata = this->table_obj->metadata_ptr;
         std::string nombre_tabla = metadata->name;
         
         this->out.open("backup_data/wal.bin", std::ios::binary | std::ios::app);
         Logger::log(LogLevel::DEBUG, "Ya se ha abierto el archivo");

         this->tipos_datos = metadata->column_types;
         this->columnas_nombre = metadata->column_names;
         this->num_cols = metadata->n_cols;

         this->row_iterator = new disk_buffer::tableRowIterator_only_ram_for_wal(nombre_tabla, this->n_rows_a_escribir);
         //Values valor_tmp;

         uint8_t tipo_dato = 1; 
         this->out.write(reinterpret_cast<char*>(&tipo_dato), sizeof(uint8_t));

         uint32_t size_nombre_tabla = nombre_tabla.size();
         this->out.write(reinterpret_cast<char*>(&size_nombre_tabla), sizeof(uint32_t));
         this->out.write(nombre_tabla.data(), size_nombre_tabla);

         Logger::log(LogLevel::DEBUG, "Filas insertadas que escribiremos en el WAL: ", false, true);
         Logger::log(LogLevel::DEBUG, this->n_rows_a_escribir, true, false);
         this->out.write(reinterpret_cast<char*>(&this->n_rows_a_escribir), sizeof(uint32_t));
         // We write the buffer size used to wirte this very same data batch:
         this->out.write(reinterpret_cast<char*>(&size_buffer_bytes), sizeof(uint32_t));
         this->out.flush();

         this->state = 3;
         break;
      };

      case 1: {
         Logger::log(LogLevel::DEBUG, "State 1");
         if(this->row_iterator->is_eof()){
            Logger::log(LogLevel::DEBUG, "EOF General");
            this->state = 7;
            break;
         }else{
            this->state = 2;
            break;
         };
      };

      case 2: {
         Logger::log(LogLevel::DEBUG, "State 2");
         Logger::log(LogLevel::DEBUG, "Columna actual: ", false, true);
         Logger::log(LogLevel::DEBUG, this->current_col, true, false);
         Logger::log(LogLevel::DEBUG, "Columnas totales: ", false, true);
         Logger::log(LogLevel::DEBUG, this->num_cols, true, false);
         if(this->eof_row()){
            Logger::log(LogLevel::DEBUG, "EOF Row");
            this->state = 3;
            this->current_col = 0;
            break;
         }else{
            this->state = 4;
            break;
         };
      };

      case 3: {
         Logger::log(LogLevel::DEBUG, "State 3");
         this->fila_a_escribir = this->row_iterator->get_next_row_ram_viva();
         this->state = 4;
         break;
      };

      case 4: {
         Logger::log(LogLevel::DEBUG, "State 4");
         Logger::log(LogLevel::DEBUG, "Columna actual: ", false, true);
         Logger::log(LogLevel::DEBUG, this->current_col, true, false);
         Logger::log(LogLevel::DEBUG, "Columna nombre: ", false, true);
         Logger::log(LogLevel::DEBUG, this->columnas_nombre[this->current_col], true, false);
         Logger::log(LogLevel::DEBUG, "Columna tipo: ", true, true);
         //Logger::log(LogLevel::DEBUG, this->tipos_datos[this->current_col], true, false);
         disk_aux::write_aux_val_buffer_with_size_check(
            this->fila_a_escribir.at(this->columnas_nombre[this->current_col]),
            this->tipos_datos[this->current_col],
            this->buffer_pointer,
            this->offset,
            this->bytes_written,
            this->bytes_remain,
            this->ptr_str_ini,
            this->current_col
         );
         this->state = 5;
         break;
      };

      case 5: {
         Logger::log(LogLevel::DEBUG, "State 5");
         Logger::log(LogLevel::DEBUG, "Bytes escritos: ", false, true);
         Logger::log(LogLevel::DEBUG, this->bytes_written, true, false);
         Logger::log(LogLevel::DEBUG, "Offset: ", false, true);
         Logger::log(LogLevel::DEBUG, this->offset, true, false);
         if(this->bytes_written + this->offset >= size_buffer_bytes){
            Logger::log(LogLevel::DEBUG, "EOF Buffer");
            this->state = 6;
            break;
         }else{
            // Si no se ha legado al offset
            this->state = 1;
            break;
         };
      };

      case 6: {
         Logger::log(LogLevel::DEBUG, "State 6");
         Logger::log(LogLevel::DEBUG, "Estado alcanzado si se llega al EOF del Buffer");
         //Logger::log(LogLevel::DEBUG, "Escribimos: ", false, true);
         //Logger::log(LogLevel::DEBUG, this->bytes_written, false, false);
         //Logger::log(LogLevel::DEBUG, " bytes", true, false);
         //this->out.write(this->buffer, this->bytes_written);


         // LOG PARA VER LO QUE SE HA ESCRITO EN EL BUFFER:
         Logger::log(LogLevel::DEBUG, "?????????????????????????????????????????????????????????????");
         Logger::log_buffer(LogLevel::DEBUG, this->buffer, size_buffer_bytes, true, true);
         Logger::flush();
         Logger::log(LogLevel::DEBUG, "?????????????????????????????????????????????????????????????");

         Logger::log(LogLevel::DEBUG, "Escribimos: ", false, true);
         Logger::log(LogLevel::DEBUG, size_buffer_bytes, false, false);
         Logger::log(LogLevel::DEBUG, " bytes", true, false);
         this->out.write(this->buffer, size_buffer_bytes);
         // No escribimos bytes basura:
         //this->out.write(this->buffer, this->bytes_written);
         this->out.flush();
         //if(!this->ptr_str_ini){
            //this->current_col += 1;
         //};
         this->offset = 0;
         this->bytes_written = 0;
         this->buffer_pointer = this->buffer;
         this->bytes_remain = size_buffer_bytes;
         this->state = 1;
         // Buffer reset to all zeros:
         std::memset(this->buffer, 0, size_buffer_bytes);
         break;
      };

      case 7: {
         Logger::log(LogLevel::DEBUG, "State 7");
         Logger::log(LogLevel::DEBUG, "Columna actual: ", false, true);
         Logger::log(LogLevel::DEBUG, this->current_col, true, false);
         Logger::log(LogLevel::DEBUG, "Columnas totales: ", false, true);
         Logger::log(LogLevel::DEBUG, this->num_cols, true, false);
         if(this->eof_row()){
            Logger::log(LogLevel::DEBUG, "EOF Rows");
            this->state = 8;
            //this->current_col = 0;
            break;
         }else{
            this->state = 4;
            break;
         };
      };

      case 8: {
         Logger::log(LogLevel::DEBUG, "State 8");
         Logger::log(LogLevel::DEBUG, "Nº de bytes de 'bytes_written': ", false, true);
         Logger::log(LogLevel::DEBUG, this->bytes_written, true, false);
         Logger::log(LogLevel::DEBUG, "Escribimos: ", false, true);
         Logger::log(LogLevel::DEBUG, size_buffer_bytes, false, false);
         Logger::log(LogLevel::DEBUG, " bytes", true, false);
         //this->out.write(this->buffer_pointer, this->bytes_written);

         // We only write if there is something left to write:
         if(this->bytes_written > 0){
            Logger::log(LogLevel::DEBUG, "Como 'bytes_written' > 0, procedemos a escribir lo que queda con padding");
            this->out.write(this->buffer, size_buffer_bytes);
            // No escribimos basura:
            //this->out.write(this->buffer, this->bytes_written);
            this->out.flush();
         };
         this->state = 255;
         break;
      };

      case 255: {
         this->out.flush();
         Logger::log(LogLevel::DEBUG, "State 255");
         break;
      };
   };
};

void disk_wal_write::walDataWriter::execute_fsm(){
   bool aux_bool = true;
   while(aux_bool) {
      if(this->state == 255){
         aux_bool = false;
      };
      this->control_unit();
   }
}



void disk_wal_write::write_table_data_wal(table* tabla, uint32_t n_rows_a_escribir){
   // We create the data writer object:
   disk_wal_write::walDataWriter data_writer_obj(tabla, n_rows_a_escribir);
   // We execute the data writing FSM within:
   data_writer_obj.execute_fsm();
};



//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
// CODIGO LEGACY.



void disk_wal_write::write_table_data_wal_viejo(table* tabla, uint32_t n_rows_a_escribir){

   /*
   Función toma como argumentos:
      1) Puntero al objeto de la tabla sobre la que se han insertado datos.
      2) Número de filas a escribir en la operación de inserción a hacer backup.

   Esta función toma estos dos argumentos y escribe un bloque de datos
   en el archivo de recuperación o 'WAL'.
   */

   if (!tabla) return;
   std::string nombre_tabla = tabla->metadata_ptr->name;


   std::ofstream out("backup_data/wal.bin", std::ios::binary | std::ios::app);
   Logger::log(LogLevel::DEBUG, "Ya se ha abierto el archivo");

   table_metadata* metadata = tabla->metadata_ptr;
   std::vector<dataType> tipos_datos = tabla->metadata_ptr->column_types;
   std::vector<std::string> columnas_nombre = tabla->metadata_ptr->column_names;
   uint32_t n_cols = tabla->metadata_ptr->n_cols;
   uint32_t n_filas = metadata->n_filas_ram;

   // Creamos el iterador por filas:
   //auto it = disk_buffer::tableRowIterator_only_ram_for_wal_inverse_order(nombre_tabla, n_rows_a_escribir);

   auto it = disk_buffer::tableRowIterator_only_ram_for_wal(nombre_tabla, n_rows_a_escribir);
   std::map<std::string, Values> fila_a_escribir;
   Values valor_tmp;

   // Antes de nada, escribimos el tipo de dato que es:
   uint8_t tipo_dato = 1; // 1 porque es dato
   out.write(reinterpret_cast<char*>(&tipo_dato), sizeof(uint8_t));

   // Escribimos el nombre de la tabla:
   uint32_t size_nombre_tabla = nombre_tabla.size();
   out.write(reinterpret_cast<char*>(&size_nombre_tabla), sizeof(uint32_t));
   out.write(nombre_tabla.data(), size_nombre_tabla);


   Logger::log(LogLevel::DEBUG, "Filas insertadas que escribiremos en el WAL: ", false, true);
   Logger::log(LogLevel::DEBUG, n_rows_a_escribir, true, false);
   out.write(reinterpret_cast<char*>(&n_rows_a_escribir), sizeof(uint32_t));

   // Inicializamos el buffer de datos a escribir:
   std::vector<char> buffer;

   // Ahora nos movemos al final para poder escribir sólo al final

   // Ahora iteraremos hasta que esteé vacía la fila a escribir
   Logger::log(LogLevel::DEBUG, "Pasamos a la iteración de escribir las filas");
   Logger::log(LogLevel::DEBUG, " ");
   Logger::log(LogLevel::DEBUG, "//////////////////////////////////////////////////////////////");
   while(!it.is_eof()){
      fila_a_escribir = it.get_next_row_ram_viva();
      Logger::log(LogLevel::DEBUG, "Insertamos la fila: ", false, true);
      Logger::log(LogLevel::DEBUG, it.contador - 1, true, false);
      // Ahora iteramos por cada columna:
      for(int i = 0; i<n_cols; i++){
         // valor_tmp = fila_a_escribir[i]; // Obtenemos el valor de una fila y columna concretos
         Logger::log(LogLevel::DEBUG, "Pasamos a recuperar la variable 'valor_tmp'");
         valor_tmp = fila_a_escribir.at(columnas_nombre[i]); // Obtenemos el valor de una fila y columna concretos
         Logger::log(LogLevel::DEBUG, "Variable 'valor_tmp' recuperada con exito");
         //disk_io::write_aux_val(valor_tmp, tipos_datos[i], out);
         disk_aux::write_aux_val_buffer(valor_tmp, tipos_datos[i], buffer);
         Logger::log(LogLevel::DEBUG, "Escritura de la fila: ", false, true);
         Logger::log(LogLevel::DEBUG, it.contador, false, false);
         Logger::log(LogLevel::DEBUG, " terminada con exito", true, false);
         Logger::flush();
      };
      Logger::flush();
   };
   Logger::flush();
   // Antes de cerrar la escritura, escribimos el buffer de escritura:
   disk_aux::aux_vector_buffer_write_disk(buffer, out);
   out.flush();
   out.close();
};