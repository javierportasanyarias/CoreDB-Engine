

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
#include <cstring> // Para usar std::memcpy

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
   /*uint32_t n_tablas_meta = 0;
   if (!fs::exists("backup_data/wal.bin")) {
      // No existe el archivo previamente:
      std::ofstream out("backup_data/wal.bin", std::ios::binary | std::ios::app);
      out.seekp(0); // Vamos al principio
      out.write(reinterpret_cast<char*>(&n_tablas_meta), sizeof(uint32_t));

   } else {
      std::fstream out("backup_data/wal.bin", std::ios::binary | std::ios::in | std::ios::out);
      out.seekp(0); // Vamos al principio
      out.read(reinterpret_cast<char*>(&n_tablas_meta), sizeof(uint32_t));
      n_tablas_meta += 1;
      out.seekp(0); // Vamos al principio
      out.write(reinterpret_cast<char*>(&n_tablas_meta), sizeof(uint32_t));
      //out.seekp(0, std::ios::end); // Volvemos al final para escribir el nuevo bloque de metadatos
   };*/

   // Volvemos a abrir el archivo ,esta vez en modo append:
   std::ofstream out("backup_data/wal.bin", std::ios::binary | std::ios::app);
   //out.seekp(0, std::ios::end); // Volvemos al final para escribir el nuevo bloque de metadatos


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
   uint32_t num_cols = (tabla->metadata_ptr->column_names).size();
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

      tmp_char_ptr = reinterpret_cast<char*>(&size_column_name);
      buffer.insert(buffer.end(),
                  tmp_char_ptr,
                  tmp_char_ptr + sizeof(uint32_t)
                  );

      tmp_char_ptr = column_name.data();
      buffer.insert(buffer.end(),
                  tmp_char_ptr,
                  tmp_char_ptr + size_column_name
                  );
      // -- Escribimos el tipo de dato:
      dataType col_type = (tabla->metadata_ptr->column_types)[i];
      uint32_t col_type_disk = static_cast<uint32_t>(col_type);

      tmp_char_ptr = reinterpret_cast<char*>(&col_type_disk);
      buffer.insert(buffer.end(),
                  tmp_char_ptr,
                  tmp_char_ptr + sizeof(uint32_t)
                  );
      // -- Escribimos si es clave primaria:
      bool column_is_key = (tabla->metadata_ptr->primary_list)[i];
      uint8_t key_val = column_is_key ? 1 : 0;

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
   /*Logger::log(LogLevel::DEBUG, "Registramos el tipo de dato");
   uint8_t valorCero = 0;
   out.write(reinterpret_cast<const char*>(&valorCero), sizeof(uint8_t));
   Logger::log(LogLevel::DEBUG, "Tipo de dato escrito con exito. Al ser metadato es cero");
   */
   // Escribimos el tamaño de los metadatos:
   Logger::log(LogLevel::DEBUG, "Registramos el tamaño del buffer de los metadatos");
   //uint32_t buffer_size = static_cast<uint32_t>(buffer.size());
   //uint32_t buffer_size = sizeof(buffer.data());
   uint32_t buffer_size = buffer.size();
   // Modificamos el tamaño del buffer para que este no tenga en cuenta el nombre de la tabla:
   //buffer_size = buffer_size - sizeof(uint32_t) - size_nombre; // Al final NO se modifca
   out.write(reinterpret_cast<char*>(&buffer_size), sizeof(uint32_t));
   Logger::log(LogLevel::DEBUG, "Tamaño del buffer de datos escito con exito");
   Logger::log(LogLevel::DEBUG, "Pasamos a escribir el buffer de datos en disco:");
   // Ahora ya sí podemos escribir el contenido del buffer en sí
   disk_io::aux_vector_buffer_write_disk(buffer, out);
   Logger::log(LogLevel::DEBUG, "Buffer de datos escrito con EXITO");
   out.flush();
   out.close();
   return;
};


//========================================================
//== FUNION LECTURA METADATOS EN EL WAL: =================
//========================================================


std::vector<char> disk_io::recuperar_meta_wal_tabla_buffer(std::ifstream& in, std::string nombre_tabla){
   /*
   Función que devuelve un buffer con los datos de ua tabla concreta.
   La función devuelve:
   2º) El buffer de los datos leídos
   */

   Logger::log(LogLevel::DEBUG, "Dentro de la función para leer el buffer de los metadatos");

   std::vector<char> buffer;
   // Ahora vemos si el archivo de metadatos ya existe o no:
   if (fs::exists("data/" + nombre_tabla + "_meta.bin")) return buffer;
   Logger::log(LogLevel::DEBUG, "El archivo de metadatos no existe, por lo que proseguimos");
   // En caso de no existir, leemos los datos desde el WAL de metadatos:

   Logger::log(LogLevel::DEBUG, "Pasamos a leer el buffer de los metadatos:");
   // 1º) Leemos el tamaño del buffer de memoria de los metadatos de la tabla concreta:
   Logger::log(LogLevel::DEBUG, "1º) Leemos el tamaño del buffer");
   uint32_t buffer_size = 0;
   in.read(reinterpret_cast<char*>(&buffer_size), sizeof(uint32_t));
   Logger::log(LogLevel::DEBUG, "   Tamaño del buffer leido con exito");
   Logger::log(LogLevel::DEBUG, "2º) Leemos el buffer");
   // 2º) Leemos el buffer de datos:
   buffer.resize(buffer_size); // <- Reajustamos el tamaño del buffer
   in.read(buffer.data(), buffer_size);
   Logger::log(LogLevel::DEBUG, "   Buffer leido con exito");

   return buffer;

};


std::string disk_io::recuperar_meta_wal_tabla_nombre(std::ifstream& in){
   /*
   Función que devuelve un buffer con los datos de ua tabla concreta.
   La función devuelve:
   1º) El nombre de la tabla
   */
   Logger::log(LogLevel::DEBUG, "Dentro de la funcion para recuperar el nombre de la tabla");
   // Leemos el nombre de la tabls
   Logger::log(LogLevel::DEBUG, "Leemos el tamano del nombre de la tabla");
   uint32_t size_nombre_tabla = 0;
   in.read(reinterpret_cast<char*>(&size_nombre_tabla), sizeof(uint32_t));
   Logger::log(LogLevel::DEBUG, "Leemos el nombre de la tabla");
   std::string nombre_tabla;
   nombre_tabla.resize(size_nombre_tabla);   // ← reservar memoria
   in.read(nombre_tabla.data(), size_nombre_tabla);
   Logger::log(LogLevel::DEBUG, "Nombre de la tabla recuperado con EXITO");
   return nombre_tabla;
};


void disk_io::aux_read_single_table_wal_metadata(std::ifstream& in){
   /*
   Función auxliar que lee los metadatos de una sola tabla desde el WAL de metadatos de una tabla:
   */
   Logger::log(LogLevel::DEBUG, "Dentro de la funcion de lectura de Metadatos del WAL");
   // Leemos el nombre de la tabla:
   Logger::log(LogLevel::DEBUG, "Procedemos a recuperar el nombre de la tabla");
   std::string nombre_tabla;
   nombre_tabla = disk_io::recuperar_meta_wal_tabla_nombre(in);
   Logger::log(LogLevel::DEBUG, "Nombre de la tabla recuperado con éxito");
   // Recuperamos el buffer de datos:
   Logger::log(LogLevel::DEBUG, "Procedemos a recuperar el buffer de metadatos");
   std::vector<char> buffer;
   buffer = disk_io::recuperar_meta_wal_tabla_buffer(in, nombre_tabla);
   Logger::log(LogLevel::DEBUG, "Buffer de metadatos ya recuperado");
   if(buffer.empty()) return;

   /////////////////////////////////////////////
   // Antes de leer vemos is los metadatos de la tabla están en memoria RAM:
   auto it = global_table_dict.find(nombre_tabla);
   if(it != global_table_dict.end()){
      Logger::log(LogLevel::DEBUG, "Los metadatos YA están en RAM, por lo que abortamos el defnirilos de nuevo");
      return;
   };
   // No existe en los metadatos de la RAM, por lo que creamos la entrada:
   Logger::log(LogLevel::DEBUG, "La tabla: ", false, true);
   Logger::log(LogLevel::DEBUG, nombre_tabla, false, false);
   Logger::log(LogLevel::DEBUG, " no existia. La creamos", true, false);

   //table* tb_created = global_table_dict[nombre_tabla];
   // Si el puntero es nulo, es que el mapa acaba de crear la entrada vacía
   table* tb_created = new table();
   global_table_dict[nombre_tabla] = tb_created;

   Logger::log(LogLevel::DEBUG, "Tabla creada con exito");
   // Creamos además sus metadatos:
   Logger::log(LogLevel::DEBUG, "Creamos un nuevo puntero para los metadatos de la nueva tabla");
   table_metadata* metadatos_puntero = new table_metadata;
   tb_created->metadata_ptr = metadatos_puntero;
   Logger::log(LogLevel::DEBUG, "Puntero de metadatos creado con exito");
   Logger::log(LogLevel::DEBUG, "Pasamos a llenar los metadatos de la RAM");
   Logger::flush();

   // Pasamos a insertar el nombre en la RAM de latabla:
   Logger::log(LogLevel::DEBUG, "Insertamos el nombre");
   ((*(tb_created->metadata_ptr)).name) = nombre_tabla;
   Logger::log(LogLevel::DEBUG, "Nombre insertado con exito");


   /////////////////////////////////////////////
   // Ya recuperado el buffer de datos, debemos leerlo y rellenar los metadatos de la tabla en RAM
   Logger::log(LogLevel::DEBUG, "inicializamos el offset");
   uint32_t offset_read = 0; // Offset de lectura

   // Leemos el número de columnas:
   Logger::log(LogLevel::DEBUG, "Copiamos el numero de columnas");
   uint32_t num_cols = 0;
   std::memcpy(&num_cols, buffer.data() + offset_read, sizeof(uint32_t));
   offset_read += sizeof(uint32_t);
   Logger::log(LogLevel::DEBUG, "Numero de columnas copiado con exito");

   // Leemos el numero de filas (siempre será cero en el WAL):
   Logger::log(LogLevel::DEBUG, "Copiamos el numero de filas");
   uint32_t n_filas = 0;
   std::memcpy(&n_filas, buffer.data() + offset_read, sizeof(uint32_t));
   offset_read += sizeof(uint32_t);
   Logger::log(LogLevel::DEBUG, "Numero de filas copiado con exito");

   Logger::log(LogLevel::DEBUG, "El numero de filas leido es: ", false, true);
   Logger::log(LogLevel::DEBUG, n_filas, true, false);
   Logger::log(LogLevel::DEBUG, "El numero de columnas leido es: ", false, true);
   Logger::log(LogLevel::DEBUG, num_cols, true, false);
   Logger::flush();


   Logger::log(LogLevel::DEBUG, "Entramos en el bucle por columnas, para escribirlas en RAM");
   // Ahora iteramos por cada columna:
   for(int i = 0; i<num_cols; i++){

      // Leemos el nombre de la columna:
      uint32_t size_column_name = 0;
      std::memcpy(&size_column_name, buffer.data() + offset_read, sizeof(uint32_t));
      offset_read += sizeof(uint32_t);

      std::string column_name;
      column_name.resize(size_column_name);
      std::memcpy(column_name.data(), buffer.data() + offset_read, size_column_name);
      offset_read += size_column_name;

      tb_created->metadata_ptr->column_names.push_back(column_name);

      // Ahora leemos el tipo de dato:
      uint32_t col_type_disk_int;
      std::memcpy(&col_type_disk_int, buffer.data() + offset_read, sizeof(uint32_t));
      dataType col_type_disk = static_cast<dataType>(col_type_disk_int);
      offset_read += sizeof(uint32_t);

      tb_created->metadata_ptr->column_types.push_back(col_type_disk);

      // Ahora leemos si la variable es clave primaria o no:
      uint8_t key_val_int;
      bool key_val = true;
      std::memcpy(&key_val_int, buffer.data() + offset_read, sizeof(uint8_t));
      offset_read += sizeof(uint8_t);
      if(key_val_int == 0){
         key_val = false;
      };
      tb_created->metadata_ptr->primary_list.push_back(key_val);
   };

};

void disk_io::recuperar_data_wal_tabla_buffer(std::ifstream& in, std::string nombre_tabla){

   Logger::log(LogLevel::DEBUG, "Dentro de la funcion de recuperacion del buffer de datos");
   // Leemos el número de filas:
   Logger::log(LogLevel::DEBUG, "Leemos el numero de filas");
   uint32_t n_filas = 0;
   in.read(reinterpret_cast<char*>(&n_filas), sizeof(uint32_t));
   Logger::log(LogLevel::DEBUG, "Numero de filas a recuperar leido con exito");
   Logger::log(LogLevel::DEBUG, "El numero de filas a recuperar es de: ", false, true);
   Logger::log(LogLevel::DEBUG, n_filas, true, false);

   // Recuperamos la tabla:
   Logger::log(LogLevel::DEBUG, "Pasamos a recuperar el bojeto de la tabla: ", false, true);
   Logger::log(LogLevel::DEBUG, nombre_tabla, true, false);
   Logger::log(LogLevel::DEBUG, "******************************");
   auto it = global_table_dict.find(nombre_tabla);

   // Asumimos que la tabla SÍ existe simpre: haya sido recuperada o esté en los metadatos:
   table* tabla = it->second;
   Logger::log(LogLevel::DEBUG, "Objeto de la tabla recuperado");
   // Recuperamos el número de columnas
   Logger::log(LogLevel::DEBUG, "Pasamos a recuperar sus metadatos:");
   table_metadata* metadata = tabla->metadata_ptr;
   std::vector<dataType> tipos_datos = tabla->metadata_ptr->column_types;
   Logger::log(LogLevel::DEBUG, "Recuperamos el numero de columnas a recuperar");
   uint32_t n_cols = tipos_datos.size();
   Logger::log(LogLevel::DEBUG, "El numero de columnas es de: ", false, true);
   Logger::log(LogLevel::DEBUG, n_cols, true, false);

   // Recuperamos otros datos necesarios:
   Logger::log(LogLevel::DEBUG, "Recuperamos los nombres de las columnas");
   std::vector<std::string> col_names = tabla->metadata_ptr->column_names;
   Logger::log(LogLevel::DEBUG, "Nombres de las columnas recuperado");

   Values valor_tmp;

   Logger::log(LogLevel::DEBUG, "Pasamos a crear el puntero a los datosa en RAM viva de la tabla");
   //std::map<std::string, std::vector<Values>>& columnas = tabla->data_buffer_ptr->columns;
   // Antes vemo si el puntero de datos en RAM viva existe:
   if(!tabla->data_ptr){
      table_data* table_data_puntero = new table_data;
      tabla->data_ptr = table_data_puntero;
   };
   std::map<std::string, std::vector<Values>>& columnas = tabla->data_ptr->columns;

   Logger::log(LogLevel::DEBUG, "Puntero a los datos de la tabla CREADO con EXITO");

   // Ahora leemos el contenido:
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
};

void disk_io::aux_read_single_table_wal_data(std::ifstream& in){

   // Leemos el nombre de la tabla:
   std::string nombre_tabla;
   nombre_tabla = disk_io::recuperar_meta_wal_tabla_nombre(in); // NO ESTAMOS LEYENDO METADATOS



   // Ahora leemos el contenido de los datos de la tabla y lo guardamos en un buffer:
   std::vector<char> buffer;
   disk_io::recuperar_data_wal_tabla_buffer(in, nombre_tabla); // Esta función ya insert los datos en la RAM

};


// FUNCIONES AUXILIARES:

bool disk_io::is_eof_read(std::ifstream& in) {
    // 1. Guardamos dónde estamos ahora mismo
    std::streampos current_pos = in.tellg();

    // 2. Miramos dónde está el final
    in.seekg(0, std::ios::end);
    std::streampos end_pos = in.tellg();

    // 3. VOLVEMOS a donde estábamos (no al principio)
    in.seekg(current_pos);

    return current_pos >= end_pos;
};

void disk_io::read_wal(){

   Logger::log(LogLevel::DEBUG, "Entramos en la lectura del WAL");

   if(!fs::exists("backup_data/wal.bin")) return;

   std::ifstream in("backup_data/wal.bin", std::ios::binary);


   // Vamos leyendo por bloques:
   Logger::log(LogLevel::DEBUG, "Definimos 'eof' para que nos ayude a ver si hemos llegado al final del archivo");
   bool eof = false;
   eof = disk_io::is_eof_read(in);
   Logger::log(LogLevel::DEBUG, " 'eof' definido con exito");
   while(!eof){
      Logger::log(LogLevel::DEBUG, "Entramos al bucle de lectura");
      // Antes de nada leemos el tipo de dato:
      Logger::log(LogLevel::DEBUG, "Leemos el tipo de dato");
      uint8_t tipo_dato = 0;
      in.read(reinterpret_cast<char*>(&tipo_dato), sizeof(uint8_t));
      Logger::log(LogLevel::DEBUG, "El bloque de datos a leer es: ", false, true);
      // Si tipo_dato es 0 es metadato y si es 1 es dato
      if(tipo_dato == 0){
         // Metadato:
         Logger::log(LogLevel::DEBUG, "Metadatos", true, false);
         disk_io::aux_read_single_table_wal_metadata(in);
      } else {
         // Dato:
         Logger::log(LogLevel::DEBUG, "Datos", true, false);
         disk_io::aux_read_single_table_wal_data(in);
      };
      Logger::flush();
      eof = disk_io::is_eof_read(in);
   };
   in.close();
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


   std::ofstream out("backup_data/wal.bin", std::ios::binary | std::ios::app);
   Logger::log(LogLevel::DEBUG, "Ya se ha abierto el archivo");

   table_metadata* metadata = tabla->metadata_ptr;
   std::vector<dataType> tipos_datos = tabla->metadata_ptr->column_types;
   std::vector<std::string> columnas_nombre = tabla->metadata_ptr->column_names;
   uint32_t n_cols = tipos_datos.size();
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

   // Actualizamos el contador

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
                  disk_io::write_table_data(table_ptr, n_rows);
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
   //disk_io::delete_wal_bin_file();
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