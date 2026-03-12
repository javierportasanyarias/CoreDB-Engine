#include "globals.h"
#include "disk_io.h"
#include "logging.h"
#include "disk_buffer.h"

// namespace disk_buffer {

   // ==========================================
   // == FUNCIONES AUXILIARES: CONTEO FILAS ====
   // ==========================================

void disk_buffer::contar_datos_ram_una_tabla(std::string& nombre_tabla) {
   table* table_ptr = global_table_dict.at(nombre_tabla);
   
   // 1. Verificamos que data_ptr exista y que el mapa de columnas no esté vacío
   if (table_ptr->data_ptr && !table_ptr->data_ptr->columns.empty()) {
      
         // 2. Tomamos la PRIMERA columna que encontremos
         auto it = table_ptr->data_ptr->columns.begin();
      
         // 3. El número de FILAS es el tamaño del vector de esa columna
         uint32_t n_rows = static_cast<uint32_t>(it->second.size());
      
      table_ptr->metadata_ptr->n_filas_ram = n_rows;
      
         Logger::log(LogLevel::DEBUG, "Filas contadas en RAM para " + nombre_tabla + ": " + std::to_string(n_rows));
   } else {
         table_ptr->metadata_ptr->n_filas_ram = 0;
   }
   Logger::flush();
};



void disk_buffer::contar_datos_en_ram_todas_tablas(){

   // iteramos por todas las tablas:
   for (const auto& [table_name, table_ptr] : global_table_dict){
      // Contamos el numero de filas en RAM viva:
      uint32_t n_rows = static_cast<uint32_t>(table_ptr->data_ptr->columns.size());
   table_ptr->metadata_ptr->n_filas_ram = n_rows;
   };

};

   // ==========================================
   // == FUNCION PRINCIPAL: ITERADOR FILAS =====
   // ==========================================


// Metodo constructor
disk_buffer::tableRowIterator::tableRowIterator(std::string tabla_nombre){
   contador = 0;
   tabla_ptr = global_table_dict.at(tabla_nombre);
   // Solo en caso de tenerlo, contamos las filas en RAM viva:
   if(tabla_ptr->data_ptr){
      contar_datos_ram_una_tabla(tabla_nombre);
   };
   Logger::log(LogLevel::DEBUG, "Se han contado las filas en la RAM viva");
   if(Logger::level == LogLevel::OUTPUT){
      Logger::flush(false);
   } else {
      Logger::flush();
   };
   // Aqui deberismos cargar los dstos desde el disco:
   if(!tabla_ptr->data_buffer_ptr){
      // Solo leemos si existe el archivo:
      Logger::log(LogLevel::DEBUG, "NO EXISTE EL APARTADO EN RAM DE DATOS EN DISCO, ENTRAMOS A LEERLO SI EL ARCHIVO EXISTE");
      if (fs::exists("data/" + tabla_nombre + "_data.bin")){
         Logger::log(LogLevel::DEBUG, "LEEMOS LOS DATOS EN DISCO:", true, true);
         // Antes de leer, creamos la región de la RAM para los datos en disco:
         tabla_ptr->data_buffer_ptr = new table_data_buffer();
         disk_io::read_table_data(tabla_ptr); // Esta funcion ya actusliza el conteo en disco
      } else {
         Logger::log(LogLevel::DEBUG, "LOS DATOS DE LA TABLA: "+tabla_nombre+ " no existen en disco", true, true);
      }
   };
   this->eof = (tabla_ptr->metadata_ptr->n_filas_disco + tabla_ptr->metadata_ptr->n_filas_ram == 0);
   Logger::log(LogLevel::DEBUG, "Se ha construido tableRowIterator hasta 'eof'");
   Logger::log(LogLevel::DEBUG, "El booleano 'eof' nos sale :", false, true);
   if(this->eof){
      Logger::log(LogLevel::DEBUG, "true", true, false);
   } else {
            Logger::log(LogLevel::DEBUG, "false", true, false);
   };
   Logger::log(LogLevel::DEBUG, "Hemos leido con exito los datos en disco");
   if(Logger::level == LogLevel::OUTPUT){
      Logger::flush(false);
   } else {
      Logger::flush();
   };
};

// Para consultar eof:
bool disk_buffer::tableRowIterator::is_eof() {
   return eof;
};

// Para devolver la prox fila:
std::map<std::string, Values> disk_buffer::tableRowIterator::get_next_row(){
   std::map<std::string, Values> map_fila_retornar;
   uint32_t n_f_disk = tabla_ptr->metadata_ptr->n_filas_disco;
   uint32_t n_f_ram = tabla_ptr->metadata_ptr->n_filas_ram;
   uint32_t n_f_total = n_f_disk + n_f_ram;
   Logger::log(LogLevel::DEBUG, "N filas disco: ", false, true);
   Logger::log(LogLevel::DEBUG, n_f_disk, true, false);
   Logger::log(LogLevel::DEBUG, "N filas RAM: ", false, true);
   Logger::log(LogLevel::DEBUG, n_f_ram, true, false);
   //std::map<std::string, std::vector<Values>> columnas = tabla_ptr->data_buffer_ptr->columns;
   if(Logger::level == LogLevel::OUTPUT){
      Logger::flush(false);
   } else {
      Logger::flush();
   };
   // Iteramos por cada columna:
   for (const std::string& nombre_col : tabla_ptr->metadata_ptr->column_names) {
      Logger::log(LogLevel::DEBUG, "Nombre de la columna a leer de la RAM: ", false, true);
      Logger::log(LogLevel::DEBUG, nombre_col, true, false);
      if(contador < n_f_disk){
         // Leemos primero desde disco:
         Logger::log(LogLevel::DEBUG, "Leemos desde el disco");
         if(Logger::level == LogLevel::OUTPUT){
            Logger::flush(false);
         } else {
            Logger::flush();
         };
         if(this->tabla_ptr->data_buffer_ptr->columns.find(nombre_col) != this->tabla_ptr->data_buffer_ptr->columns.end()){
            Logger::log(LogLevel::DEBUG, "SI existe: ", false, true);
            Logger::log(LogLevel::DEBUG, nombre_col, false, false);
            Logger::log(LogLevel::DEBUG, " en 'tabla_ptr->data_buffer_ptr->columns'", true, false);
         } else {
            Logger::log(LogLevel::DEBUG, "NO existe: ", false, true);
            Logger::log(LogLevel::DEBUG, nombre_col, false, false);
            Logger::log(LogLevel::DEBUG, " en 'tabla_ptr->data_buffer_ptr->columns'", true, false); 
         };
         map_fila_retornar[nombre_col] = this->tabla_ptr->data_buffer_ptr->columns.at(nombre_col)[contador];
      } else if(contador < n_f_total) {
         uint32_t indice_relativo_ram = contador - n_f_disk;
         // Leemos desde la RAM viva:
         Logger::log(LogLevel::DEBUG, "Leemos desde RAM viva");
         if(Logger::level == LogLevel::OUTPUT){
            Logger::flush(false);
         } else {
            Logger::flush();
         };
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

   disk_buffer::tableRowIterator_only_ram::tableRowIterator_only_ram(std::string tabla_nombre){
   contador = 0;
   tabla_ptr = global_table_dict.at(tabla_nombre);
   // Solo en caso de tenerlo, contamos las filas en RAM viva:
   if(tabla_ptr->data_ptr){
      contar_datos_ram_una_tabla(tabla_nombre);
   };
   Logger::log(LogLevel::DEBUG, "Se han contado las filas en la RAM viva");
   if(Logger::level == LogLevel::OUTPUT){
      Logger::flush(false);
   } else {
      Logger::flush();
   };
   // Aqui deberismos cargar los dstos desde el disco:
   /*if(!tabla_ptr->data_buffer_ptr){
      // Solo leemos si existe el archivo:
      Logger::log(LogLevel::DEBUG, "NO EXISTE EL APARTADO EN RAM DE DATOS EN DISCO, ENTRAMOS A LEERLO SI EL ARCHIVO EXISTE");
      if (fs::exists("data/" + tabla_nombre + "_data.bin")){
         Logger::log(LogLevel::DEBUG, "LEEMOS LOS DATOS EN DISCO:", true, true);
         // Antes de leer, creamos la región de la RAM para los datos en disco:
         tabla_ptr->data_buffer_ptr = new table_data_buffer();
         disk_io::read_table_data(tabla_ptr); // Esta funcion ya actusliza el conteo en disco
      } else {
         Logger::log(LogLevel::DEBUG, "LOS DATOS DE LA TABLA: "+tabla_nombre+ " no existen en disco", true, true);
      }
   };*/
   this->eof = (tabla_ptr->metadata_ptr->n_filas_ram == 0);
   Logger::log(LogLevel::DEBUG, "Se ha construido tableRowIterator hasta 'eof'");
   Logger::log(LogLevel::DEBUG, "El booleano 'eof' nos sale :", false, true);
   if(this->eof){
      Logger::log(LogLevel::DEBUG, "true", true, false);
   } else {
      Logger::log(LogLevel::DEBUG, "false", true, false);
   };
};

// Para consultar eof:
bool disk_buffer::tableRowIterator_only_ram::is_eof() {
   return eof;
};

// == PARA OBTENER LA PROXIMA FILA de la ram viva:
std::map<std::string, Values> disk_buffer::tableRowIterator_only_ram::get_next_row_ram_viva(){
   std::map<std::string, Values> map_fila_retornar;
   // uint32_t n_f_disk = tabla_ptr->metadata_ptr->n_filas_disco;
   uint32_t n_f_total = tabla_ptr->metadata_ptr->n_filas_ram;
   // uint32_t n_f_total = n_f_disk + n_f_ram;
   // Logger::log(LogLevel::DEBUG, "N filas disco: ", false, true);
   // Logger::log(LogLevel::DEBUG, n_f_disk, true, false);
   Logger::log(LogLevel::DEBUG, "N filas RAM: ", false, true);
   Logger::log(LogLevel::DEBUG, n_f_total, true, false);
   if(Logger::level == LogLevel::OUTPUT){
      Logger::flush(false);
   } else {
      Logger::flush();
   };
   // Iteramos por cada columna:
   for (const std::string& nombre_col : tabla_ptr->metadata_ptr->column_names) {
      Logger::log(LogLevel::DEBUG, "Nombre de la columna a leer de la RAM: ", false, true);
      Logger::log(LogLevel::DEBUG, nombre_col, true, false);

      if(contador < n_f_total){
         // Leemos desde la RAM viva:
         Logger::log(LogLevel::DEBUG, "Leemos desde RAM viva");
         if(Logger::level == LogLevel::OUTPUT){
            Logger::flush(false);
         } else {
            Logger::flush();
         };
         map_fila_retornar[nombre_col] = tabla_ptr->data_ptr->columns.at(nombre_col)[contador];
         Logger::log(LogLevel::DEBUG, "Valor recuperado de la tabla en RAM viva");
         if(Logger::level == LogLevel::OUTPUT){
            Logger::flush(false);
         } else {
            Logger::flush();
         };
      }; 
   };
   contador += 1;
   if(contador >= (tabla_ptr->metadata_ptr->n_filas_ram)){
      this-> eof = true;
   };
   return map_fila_retornar;
}; // Termina el metodo 'get_next_row_ram_viva'

// }; // Cierre del namespace 'disk_buffer'
