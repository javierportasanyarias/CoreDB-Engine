

#include "data_struct.h"
#include <iostream>
#include <fstream>
#include <string>
#include "globals.h"
#include <map>
#include "logging.h"
#include "filesystem"
#include "disk_aux.h"
#include "disk_wal.h"
#include "disk_buffer.h"
#include <cstring> // Para usar std::memcpy
#include "disk_aux.h"
#include "disk_metadata.h"

//========================================================
//== FUNION ESCRITURA METADATOS EN EL WAL: ===============
//========================================================
void disk_wal::write_table_wal_metadata(table* tabla){
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
      int varlo_tmp_int = 0;
      tmp_char_ptr = reinterpret_cast<char*>(&varlo_tmp_int);
      //buffer.insert(buffer.end(),
                  //tmp_char_ptr,
                  //tmp_char_ptr + sizeof(uint32_t)
                  //);
      std::memcpy(ptr_curr, tmp_char_ptr, sizeof(uint32_t));
      ptr_curr += sizeof(uint32_t);
      Logger::log(LogLevel::DEBUG, "Numero de filas registrado en el buffer con exito. Al ser escritura en el WAL SIEMPRE sera cero");
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
//== FUNION LECTURA METADATOS EN EL WAL: =================
//========================================================


char* disk_wal::recuperar_meta_wal_tabla_buffer(std::ifstream& in, std::string nombre_tabla){
   /*
   Función que devuelve un buffer con los metadatosdatos de ua tabla concreta, dado su nombre.
   */

   Logger::log(LogLevel::DEBUG, "Dentro de la función para leer el buffer de los metadatos");

   //std::vector<char> buffer;
   char* buffer = nullptr;
   // Ahora vemos si el archivo de metadatos ya existe o no:
   if (fs::exists("metadata/" + nombre_tabla + "_meta.bin")) return buffer;
   Logger::log(LogLevel::DEBUG, "El archivo de metadatos no existe, por lo que proseguimos");
   // En caso de no existir, leemos los datos desde el WAL de metadatos:

   Logger::log(LogLevel::DEBUG, "Pasamos a leer el buffer de los metadatos:");
   // 1º) Leemos el tamaño del buffer de memoria de los metadatos de la tabla concreta:
   Logger::log(LogLevel::DEBUG, "1º) Leemos el tamaño del buffer");
   uint32_t buffer_size = 0;
   in.read(reinterpret_cast<char*>(&buffer_size), sizeof(uint32_t));
   try{
      buffer = new char[buffer_size];
      Logger::log(LogLevel::DEBUG, "   Tamaño del buffer leido con exito");
      Logger::log(LogLevel::DEBUG, "2º) Leemos el buffer");
      // 2º) Leemos el buffer de datos:
      //buffer.resize(buffer_size); // <- Reajustamos el tamaño del buffer
      in.read(buffer, buffer_size);
      Logger::log(LogLevel::DEBUG, "   Buffer leido con exito");
   }catch(...){
      delete[] buffer;
      in.close();
      throw;
   };
   in.close();

   return buffer;

};


std::string disk_wal::recuperar_meta_wal_tabla_nombre(std::ifstream& in){
   /*
   Función que devuelve el nombre de la tabla de la que actualmente se está leyendo los datos/metadatos
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


void disk_wal::aux_read_single_table_wal_metadata(std::ifstream& in){
   /*
   Función auxliar que lee los metadatos de una sola tabla desde el WAL de metadatos de una tabla
   */
   Logger::log(LogLevel::DEBUG, "Dentro de la funcion de lectura de Metadatos del WAL");
   // Leemos el nombre de la tabla:
   Logger::log(LogLevel::DEBUG, "Procedemos a recuperar el nombre de la tabla");
   std::string nombre_tabla;
   nombre_tabla = disk_wal::recuperar_meta_wal_tabla_nombre(in);
   Logger::log(LogLevel::DEBUG, "Nombre de la tabla recuperado con éxito");
   // Recuperamos el buffer de datos:
   Logger::log(LogLevel::DEBUG, "Procedemos a recuperar el buffer de metadatos");
   //std::vector<char> buffer;
   char* buffer = nullptr;
   buffer = disk_wal::recuperar_meta_wal_tabla_buffer(in, nombre_tabla);
   Logger::log(LogLevel::DEBUG, "Buffer de metadatos ya recuperado");
   //if(buffer.empty()) return;

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
   std::memcpy(&num_cols, buffer + offset_read, sizeof(uint32_t));
   offset_read += sizeof(uint32_t);
   Logger::log(LogLevel::DEBUG, "Numero de columnas copiado con exito");

   // Leemos el numero de filas (siempre será cero en el WAL):
   Logger::log(LogLevel::DEBUG, "Copiamos el numero de filas");
   uint32_t n_filas = 0;
   std::memcpy(&n_filas, buffer + offset_read, sizeof(uint32_t));
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
      std::memcpy(&size_column_name, buffer + offset_read, sizeof(uint32_t));
      offset_read += sizeof(uint32_t);

      std::string column_name;
      column_name.resize(size_column_name);
      std::memcpy(column_name.data(), buffer + offset_read, size_column_name);
      offset_read += size_column_name;

      tb_created->metadata_ptr->column_names.push_back(column_name);

      // Ahora leemos el tipo de dato:
      uint32_t col_type_disk_int;
      std::memcpy(&col_type_disk_int, buffer + offset_read, sizeof(uint32_t));
      dataType col_type_disk = static_cast<dataType>(col_type_disk_int);
      offset_read += sizeof(uint32_t);

      tb_created->metadata_ptr->column_types.push_back(col_type_disk);

      // Ahora leemos si la variable es clave primaria o no:
      uint8_t key_val_int;
      bool key_val = true;
      std::memcpy(&key_val_int, buffer + offset_read, sizeof(uint8_t));
      offset_read += sizeof(uint8_t);
      if(key_val_int == 0){
         key_val = false;
      };
      tb_created->metadata_ptr->primary_list.push_back(key_val);
   };

   delete[] buffer;

};

void disk_wal::recuperar_data_wal_tabla_buffer(std::ifstream& in, std::string nombre_tabla){

   /*
   Función que una vez leídos los metadatos de una tabla ya en memoria RAM,
   pasa a leer todas las columnas y filas presentes en e bloque de datos
   que se está leyendo actualmente.
   */

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
   uint32_t n_cols = tabla->metadata_ptr->n_cols;
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
         valor_tmp = disk_aux::read_aux_val(tipos_datos[j], in);
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

void disk_wal::aux_read_single_table_wal_data(std::ifstream& in){

   /*
   Función que lee y carga en el diccionario global de tablas los metadatos
   de una tabla concreta.
   */

   // Leemos el nombre de la tabla:
   std::string nombre_tabla;
   nombre_tabla = disk_wal::recuperar_meta_wal_tabla_nombre(in); // NO ESTAMOS LEYENDO METADATOS



   // Ahora leemos el contenido de los datos de la tabla y lo guardamos en un buffer:
   //std::vector<char> buffer;
   disk_wal::recuperar_data_wal_tabla_buffer(in, nombre_tabla); // Esta función ya insert los datos en la RAM

};


// FUNCIONES AUXILIARES:

bool disk_wal::is_eof_read(std::ifstream& in) {
   /*
   Función que detecta si se ha llegado al final de un archivo
      1º Guardamos la posición actual
      2º Guardamos la posición del final
      3º Volvemos a la posición actual
         (así el comprobar si estamos en
         la posición final no altera el
         puntero de lectura)
   */
    // 1º Posición actual
    std::streampos current_pos = in.tellg();

    // 2º Miramos dónde está el final
    in.seekg(0, std::ios::end);
    std::streampos end_pos = in.tellg();

    // 3º Volvemos a la posición donde estábamos antes de entrar a esta función
    in.seekg(current_pos);

    return current_pos >= end_pos;
};

void disk_wal::read_wal(){

   /*
   Función principal que se encarga de la lectura del archivo de recuperación 'WAL'.
   Este archivo 'WAL' está organizado de la siguiente manera:
   El 'WAL' se divide en bloques de datos y estos pueden ser de metadatos o datos.
   Los bloques de metadatos contienen todos los metadatos de una tabla, con la
   particularidad de que el conteo de filas total escrito siempre será cero (ya que
   en el momento de escritura no tenemos forma de estimar o no tiene sentido el hacerlo
   el número de filas escritas en el WAL).
   Por otro lado, los datos que se insertan en una sola operación se guardan en un bloque de datos.
   Si en una operación se declaran más de dos filas, en dicho bloque se registraán ese N número de
   filas.

   Los bloques de datos se almacenan en disco con el siguiente formato:

   * Metadatos:
      -> Tipo de datos (uint8_t): 0 (ya que es metadato)
      -> Tamaño del nombre de la tabla (uint32_t)
      -> Nombre de la tabla a leer sus metadatos (std::string)
      -> Tamaño del buffer de datos (uint32_t): Tamaño del buffer de datos. Esto facilita
         la lectura en una sola llamada, ahorrando tiempo.
      -> Buffer de datos (std::vector<char>): Contenido en sí de los metadatos.
         Su contenido es el mismo que el escrito en los metadatos escritos de forma
         "normal", pero con el conteo total de filas siempre a cero.
   
   * Datos:
      -> Tipo de datos (uint8_t): 1 (ya que es metadato)
      -> Tamaño del nombre de la tabla (uint32_t)
      -> Nombre de la tabla a leer sus metadatos (std::string)
      -> Bloque de datos insertado e la operación registrada. Se insertan por filas de la
         misma forma a cuando se hace de forma normal.

   Cada operación, sea crear una tabla o insertar N filas, provoca una escritura en el archivo
   'WAL' de un bloque

   Esta función iterará leyendo bloque a bloque hasta que se halle el final del archivo.
   */

   Logger::log(LogLevel::DEBUG, "Entramos en la lectura del WAL");

   if(!fs::exists("backup_data/wal.bin")) return;

   std::ifstream in("backup_data/wal.bin", std::ios::binary);


   // Vamos leyendo por bloques:
   Logger::log(LogLevel::DEBUG, "Definimos 'eof' para que nos ayude a ver si hemos llegado al final del archivo");
   bool eof = false;
   eof = disk_wal::is_eof_read(in);
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
         disk_wal::aux_read_single_table_wal_metadata(in);
      } else {
         // Dato:
         Logger::log(LogLevel::DEBUG, "Datos", true, false);
         disk_wal::aux_read_single_table_wal_data(in);
      };
      in.clear();
      Logger::flush();
      eof = disk_wal::is_eof_read(in);
   };
   in.close();
};

//========================================================
//== FUNION ESCRITURA DATOS EN EL WAL: ===================
//========================================================


class walDataWriter {

   public:
      // Counters:
      uint32_t offset = 0;
      uint32_t bytes_written = 0;
      uint32_t bytes_remain = size_buffer_bytes;
      uint8_t state = 0;
      char* ptr_str_ini = nullptr; // 💡 Inicializado a limpio

      // Auxiliary metadata:
      std::vector<dataType> tipos_datos;
      std::vector<std::string> columnas_nombre;

      // Buffer:
      char* buffer = new char[size_buffer_bytes];
      char* buffer_pointer = nullptr;

      // Columns:
      uint32_t num_cols = 0;
      uint32_t current_col = 0;

      // Variables de control inyectadas:
      uint32_t n_rows_a_escribir = 0;

      // Write and table:
      std::ofstream out;
      table* table_obj = nullptr;
      disk_buffer::tableRowIterator_only_ram_for_wal* row_iterator;
      std::map<std::string, Values> fila_a_escribir;

// El constructor ahora compilará perfectamente porque los punteros no tienen constructores obligatorios
      walDataWriter(table* t_obj, uint32_t rows_to_write) 
        : table_obj(t_obj), n_rows_a_escribir(rows_to_write), row_iterator(nullptr) {
         Logger::log(LogLevel::DEBUG, "Creacion de un objeto nuevo 'walDataWriter'");
         this->buffer_pointer = this->buffer;
      }

      ~walDataWriter() {
         delete[] this->buffer;
         delete this->row_iterator;
         if (this->out.is_open()) {
            this->out.close();
         }
      }

      bool eof_row(){
         return this->current_col + 1 > this->num_cols;
      };

      void control_unit(){
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
                  this->state = 1;
                  break;
               };
            };

            case 6: {
               Logger::log(LogLevel::DEBUG, "State 6");
               this->out.write(this->buffer_pointer, this->bytes_written);
               this->out.flush();
               //if(!this->ptr_str_ini){
                  //this->current_col += 1;
               //};
               this->offset = 0;
               this->bytes_written = 0;
               this->buffer_pointer = this->buffer;
               this->bytes_remain = size_buffer_bytes;
               this->state = 1;
               break;
            };

            case 7: {
               Logger::log(LogLevel::DEBUG, "State 7");
               Logger::log(LogLevel::DEBUG, "Columna actual: ", false, true);
               Logger::log(LogLevel::DEBUG, this->current_col, true, false);
               Logger::log(LogLevel::DEBUG, "Columnas totales: ", false, true);
               Logger::log(LogLevel::DEBUG, this->num_cols, true, false);
               if(this->eof_row()){
                  Logger::log(LogLevel::DEBUG, "EOF Row");
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
               this->out.write(this->buffer_pointer, this->bytes_written);
               this->out.flush();
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

      void execute_fsm() {
         bool aux_bool = true;
         while(aux_bool) {
            if(this->state == 255){
               aux_bool = false;
            };
            this->control_unit();
         }
      }
};


void disk_wal::write_table_data_wal(table* tabla, uint32_t n_rows_a_escribir){
   // We create the data writer object:
   walDataWriter data_writer_obj(tabla, n_rows_a_escribir);
   // We execute the data writing FSM within:
   data_writer_obj.execute_fsm();
};

void disk_wal::write_table_data_wal_viejo(table* tabla, uint32_t n_rows_a_escribir){

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

//========================================================
//== ELIMINAR EL ARCHIVO WAL ENTERO: =====================
//========================================================


void disk_wal::delete_wal_bin_file(){
   /*
   Función que elimina el arhivo de recuperación o 'WAL'.
   Debería ejecutarse al final de cada sesión sólo y únicamente después
   de haber realizado la escritura de los archivos binarios de
   metadatos y datos binarios sin fallos
   */
   if(fs::remove("backup_data/wal.bin")){
      Logger::log(LogLevel::DEBUG, "Archivo WAl eliminado con EXITO");
   } else {
      Logger::log(LogLevel::DEBUG, "El archivo WAL no existía, por lo que NO ha sido eliminado");
   };
};