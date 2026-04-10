#include "globals.h"
#include "disk_io.h"
#include "logging.h"
#include "disk_buffer.h"


// ==========================================
// == FUNCIONES AUXILIARES: CONTEO FILAS ====
// ==========================================


// == Para contar filas de una tabla =================
void disk_buffer::contar_datos_ram_una_tabla(std::string& nombre_tabla) {
   /*
   Método auxiliar para actualizar el contador de filas en RAM viva
   o de datos añadidos en la misma sesión. 
   No retorna nada, actualiza el contador de filas en RAM en los metadatos de una tabla
   dado como input su nombre
   */
   table* table_ptr = global_table_dict.at(nombre_tabla);
   
   // 1. Verificamos que data_ptr exista y que el mapa de columnas no esté vacío
   if (table_ptr->data_ptr && !table_ptr->data_ptr->columns.empty()) {
      
         // 2. Tomamos la PRIMERA columna que encontremos
         auto it = table_ptr->data_ptr->columns.begin();
      
         // 3. El número de FILAS es el tamaño del vector de esa columna
         uint32_t n_rows = static_cast<uint32_t>(it->second.size());
      
      table_ptr->metadata_ptr->n_filas_ram = n_rows;
      
   } else {
         table_ptr->metadata_ptr->n_filas_ram = 0;
   };

};


// == Para contar todas filas ========================
void disk_buffer::contar_datos_en_ram_todas_tablas(){
   /*
   Función que actualiza el contador de filas en RAM viva de
   todas las tablas cargadas en memoria.
   */
   // iteramos por todas las tablas:
   for (const auto& [table_name, table_ptr] : global_table_dict){
      // Contamos el numero de filas en RAM viva:
      uint32_t n_rows = static_cast<uint32_t>(table_ptr->data_ptr->columns.size());
   table_ptr->metadata_ptr->n_filas_ram = n_rows;
   };

};

//====================================================
//=============== tableRowIterator ===================
//====================================================


// == Método constructor =============================
disk_buffer::tableRowIterator::tableRowIterator(std::string tabla_nombre){
   /*
   Constructor de la clase 'tableRowIterator'
   */
   contador = 0;
   tabla_ptr = global_table_dict.at(tabla_nombre);
   // Solo en caso de tenerlo, contamos las filas en RAM viva:
   if(tabla_ptr->data_ptr &&  tabla_ptr->metadata_ptr->n_filas_ram == 0){
      contar_datos_ram_una_tabla(tabla_nombre);
   };
   if(Logger::level == LogLevel::OUTPUT){
      Logger::flush(false);
   } else {
      Logger::flush();
   };
   // Aqui deberismos cargar los dstos desde el disco:
   if(!tabla_ptr->data_buffer_ptr){
      // Solo leemos si existe el archivo:
      if (fs::exists("data/" + tabla_nombre + "_data.bin")){

         // Antes de leer, creamos la región de la RAM para los datos en disco:
         tabla_ptr->data_buffer_ptr = new table_data_buffer();
         disk_io::read_table_data(tabla_ptr); // Lee los datos en disco
         /* El número de filas en disco ya se ha recuperado a leer los metadatos
         Este se recuperó en la lectura de los metadatos, resultado de la suma de los datos
         en RAM y en disco.
         */
      };
   };
   this->eof = (tabla_ptr->metadata_ptr->n_filas_disco + tabla_ptr->metadata_ptr->n_filas_ram == 0);
};


// == Para consultar eof =============================
bool disk_buffer::tableRowIterator::is_eof() {
   /*
   Simple función para retornar el atributo booleano 'eof'
   */
   return eof;
};


// == Para obtener la próxima fila ===================
std::map<std::string, Values> disk_buffer::tableRowIterator::get_next_row(){
   /*
   Función de iteración sobre las filas de una tabla.
   Es un método de la clase 'tableRowIterator'.
   Funciona de la siguiente manera:
      1º Itera por cada columna de la tabla.
      2º Dado el atributo 'contador' extrae dicho elemento de cada vector de columnas de cada columna.
         Dependiendo del valor de 'contador' se extrae de los datos en RAM viva o los leídos en disco.
         Primero siempre se lee desde el disco y luego desde la RAM viva.
      3º Una vez terminado el bucle se actualiza el valor del atributo 'eof'.
      4º Se incrementa en uno el contador de fila.
   */
   std::map<std::string, Values> map_fila_retornar;
   uint32_t n_f_disk = tabla_ptr->metadata_ptr->n_filas_disco;
   uint32_t n_f_ram = tabla_ptr->metadata_ptr->n_filas_ram;
   uint32_t n_f_total = n_f_disk + n_f_ram;

   // Iteramos por cada columna:
   for (const std::string& nombre_col : tabla_ptr->metadata_ptr->column_names) {
      if(contador < n_f_disk){
         map_fila_retornar[nombre_col] = this->tabla_ptr->data_buffer_ptr->columns.at(nombre_col)[contador];
      } else if(contador < n_f_total) {
         uint32_t indice_relativo_ram = contador - n_f_disk;
         // Leemos desde la RAM viva:
         map_fila_retornar[nombre_col] = tabla_ptr->data_ptr->columns.at(nombre_col)[indice_relativo_ram];
      };
   }; // Termina la iteracion de columna
   contador += 1;
   if(contador >= (tabla_ptr->metadata_ptr->n_filas_disco + tabla_ptr->metadata_ptr->n_filas_ram)){
      this-> eof = true;
   };
   return map_fila_retornar;
}; // Termina el metodo 'get_next_row'
   
   
//====================================================
//== tableRowIterator pero solo para RAM =============
//====================================================

// == Método constructor =============================
disk_buffer::tableRowIterator_only_ram::tableRowIterator_only_ram(std::string tabla_nombre){
   /*
   Constructor de la clase 'tableRowIterator_only_ram'
   */
   contador = 0;
   tabla_ptr = global_table_dict.at(tabla_nombre);
   // Solo en caso de tenerlo, contamos las filas en RAM viva:
   if(tabla_ptr->data_ptr &&  tabla_ptr->metadata_ptr->n_filas_ram == 0){
      contar_datos_ram_una_tabla(tabla_nombre);
   };

   this->eof = (tabla_ptr->metadata_ptr->n_filas_ram == 0);
};

// == Para consultar eof =============================
bool disk_buffer::tableRowIterator_only_ram::is_eof() {
   /*
   Simple función para retornar el atributo booleano 'eof'
   */
   return eof;
};


// == Para obtener la próxima fila (RAM viva) ========
std::map<std::string, Values> disk_buffer::tableRowIterator_only_ram::get_next_row_ram_viva(){
   /*
   Método parecido a 'get_next_row' de tableRowIterator. Sólo que aquí únicamente
   se retornaran datos de la misma sesión o RAM viva.
   Funciona de la siguiente manera:
      1º Itera por cada columna de la tabla.
      2º Dado el atributo 'contador' extrae dicho elemento de cada vector de columnas de cada columna,
         , siempre de los datos en RAM viva.
      3º Una vez terminado el bucle se actualiza el valor del atributo 'eof'.
      4º Se incrementa en uno el contador de fila.
   */
   std::map<std::string, Values> map_fila_retornar;
   uint32_t n_f_total = tabla_ptr->metadata_ptr->n_filas_ram;
   // Iteramos por cada columna:
   for (const std::string& nombre_col : tabla_ptr->metadata_ptr->column_names) {

      if(contador < n_f_total){
         // Leemos desde la RAM viva:
         map_fila_retornar[nombre_col] = tabla_ptr->data_ptr->columns.at(nombre_col)[contador];
      }; 
   };
   contador += 1;
   if(contador >= (tabla_ptr->metadata_ptr->n_filas_ram)){
      this-> eof = true;
   };
   return map_fila_retornar;
}; // Termina el metodo 'get_next_row_ram_viva'



disk_buffer::tableRowIterator_only_ram_for_wal_inverse_order::tableRowIterator_only_ram_for_wal_inverse_order(std::string tabla_nombre, int num_filas_a_insertar){

   /*
   Constructor de la clase 'tableRowIterator_only_ram_for_wal_inverse_order'
   */
   this->n_filas_insertadas = num_filas_a_insertar;
   //contador = 0;
   tabla_ptr = global_table_dict.at(tabla_nombre);
   /*
   No contamos las filas en RAM viva porque solo recuperaremos
   los ultimos datos y si no fallo la inserción está asegurado
   que esas filas están en la RAM viva
   */
   if(tabla_ptr->data_ptr &&  tabla_ptr->metadata_ptr->n_filas_ram == 0){
      contar_datos_ram_una_tabla(tabla_nombre);
   };
   this->contador = tabla_ptr->metadata_ptr->n_filas_ram - 1;

   this->eof = (this->contador < 0);
}; // Termina el metodo 'tableRowIterator_only_ram_for_wal_inverse_order'

// == Para consultar eof =============================
bool disk_buffer::tableRowIterator_only_ram_for_wal_inverse_order::is_eof() {
   /*
   Simple función para retornar el atributo booleano 'eof'
   */
   Logger::log(LogLevel::DEBUG, "SE HA LLEGADO A LA CONDICION DE EOF EN ESCRITURA DE FILAS EN EL WAL");
   return eof;
};

// == Para obtener la próxima ultimas filas en RAM viva (RAM viva) ========
std::map<std::string, Values> disk_buffer::tableRowIterator_only_ram_for_wal_inverse_order::get_next_row_ram_viva(){
   /*
   Método parecido a 'get_next_row' de tableRowIterator. Sólo que aquí únicamente
   se retornaran datos de la misma sesión o RAM viva.
   Funciona de la siguiente manera:
      1º Itera por cada columna de la tabla.
      2º Dado el atributo 'contador' extrae dicho elemento de cada vector de columnas de cada columna,
         , siempre de los datos en RAM viva.
      3º Una vez terminado el bucle se actualiza el valor del atributo 'eof'.
      4º Se incrementa en uno el contador de fila.
   */
   std::map<std::string, Values> map_fila_retornar;
   uint32_t n_f_total = tabla_ptr->metadata_ptr->n_filas_ram;
   // Iteramos por cada columna:
   for (const std::string& nombre_col : tabla_ptr->metadata_ptr->column_names) {

      Logger::log(LogLevel::DEBUG, "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
      Logger::log(LogLevel::DEBUG, "Contador: ", false, true);
      Logger::log(LogLevel::DEBUG, this->contador, true, false);
      Logger::log(LogLevel::DEBUG, "n_f_total: ", false, true);
      Logger::log(LogLevel::DEBUG, n_f_total, true, false);
      Logger::log(LogLevel::DEBUG, "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
      if(this->contador < n_f_total){
         // Leemos desde la RAM viva:
         Logger::log(LogLevel::DEBUG, "Pasamois a rellenar el std::map de la fila a escribir en el WAL");
         map_fila_retornar[nombre_col] = tabla_ptr->data_ptr->columns.at(nombre_col)[this->contador];
      } else  {
         Logger::log(LogLevel::DEBUG, "La fila RETORNARÁ VACÍA");
      };
   };
   // Condiciones de contorno generales:
   if(this->contador < 0 || this->contador > tabla_ptr->metadata_ptr->n_filas_ram){
      this-> eof = true;
   };

   //if((tabla_ptr->metadata_ptr->n_filas_ram) - this->contador + 1 >= this->n_filas_insertadas){
   // Condición específica de EOF:
   if(this->contador < tabla_ptr->metadata_ptr->n_filas_ram - this->n_filas_insertadas + 1){
      this-> eof = true;
   };
   //} else if(contador <= 0) {
      //this-> eof = true;
   //};
   this->contador -= 1;
   return map_fila_retornar;
}; // Termina el metodo 'get_next_row_ram_viva'







disk_buffer::tableRowIterator_only_ram_for_wal::tableRowIterator_only_ram_for_wal(std::string tabla_nombre, int num_filas_a_insertar){

   /*
   Constructor de la clase 'tableRowIterator_only_ram_for_wal'
   */
   this->n_filas_insertadas = num_filas_a_insertar;
   //contador = 0;
   tabla_ptr = global_table_dict.at(tabla_nombre);
   /*
   No contamos las filas en RAM viva porque solo recuperaremos
   los ultimos datos y si no fallo la inserción está asegurado
   que esas filas están en la RAM viva
   */
   if(tabla_ptr->data_ptr &&  tabla_ptr->metadata_ptr->n_filas_ram == 0){
      contar_datos_ram_una_tabla(tabla_nombre);
   };
   this->contador = tabla_ptr->metadata_ptr->n_filas_ram - num_filas_a_insertar;

   this->eof = (this->contador >= tabla_ptr->metadata_ptr->n_filas_ram);
}; // Termina el metodo 'tableRowIterator_only_ram_for_wal'

// == Para consultar eof =============================
bool disk_buffer::tableRowIterator_only_ram_for_wal::is_eof() {
   /*
   Simple función para retornar el atributo booleano 'eof'
   */
   Logger::log(LogLevel::DEBUG, "SE HA LLEGADO A LA CONDICION DE EOF EN ESCRITURA DE FILAS EN EL WAL");
   return eof;
};

// == Para obtener la próxima ultimas filas en RAM viva (RAM viva) ========
std::map<std::string, Values> disk_buffer::tableRowIterator_only_ram_for_wal::get_next_row_ram_viva(){
   /*
   Método parecido a 'get_next_row' de tableRowIterator. Sólo que aquí únicamente
   se retornaran datos de la misma sesión o RAM viva.
   Funciona de la siguiente manera:
      1º Itera por cada columna de la tabla.
      2º Dado el atributo 'contador' extrae dicho elemento de cada vector de columnas de cada columna,
         , siempre de los datos en RAM viva.
      3º Una vez terminado el bucle se actualiza el valor del atributo 'eof'.
      4º Se incrementa en uno el contador de fila.
   */
   std::map<std::string, Values> map_fila_retornar;
   uint32_t n_f_total = tabla_ptr->metadata_ptr->n_filas_ram;
   // Iteramos por cada columna:
   for (const std::string& nombre_col : tabla_ptr->metadata_ptr->column_names) {

      Logger::log(LogLevel::DEBUG, "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
      Logger::log(LogLevel::DEBUG, "Contador: ", false, true);
      Logger::log(LogLevel::DEBUG, this->contador, true, false);
      Logger::log(LogLevel::DEBUG, "n_f_total: ", false, true);
      Logger::log(LogLevel::DEBUG, n_f_total, true, false);
      Logger::log(LogLevel::DEBUG, "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
      if(this->contador < n_f_total){
         // Leemos desde la RAM viva:
         Logger::log(LogLevel::DEBUG, "Pasamois a rellenar el std::map de la fila a escribir en el WAL");
         map_fila_retornar[nombre_col] = tabla_ptr->data_ptr->columns.at(nombre_col)[this->contador];
      } else  {
         Logger::log(LogLevel::DEBUG, "La fila RETORNARÁ VACÍA");
      };
   };
   this->contador += 1;
   // Condiciones de contorno generales:
   if(this->contador < 0 || this->contador >= tabla_ptr->metadata_ptr->n_filas_ram){
      this->eof = true;
   };

   //if((tabla_ptr->metadata_ptr->n_filas_ram) - this->contador + 1 >= this->n_filas_insertadas){
   // Condición específica de EOF:
   /*if(this->contador < tabla_ptr->metadata_ptr->n_filas_ram - this->n_filas_insertadas + 1){
      this-> eof = true;
   };*/
   //} else if(contador <= 0) {
      //this-> eof = true;
   //};
   return map_fila_retornar;
}; // Termina el metodo 'get_next_row_ram_viva'