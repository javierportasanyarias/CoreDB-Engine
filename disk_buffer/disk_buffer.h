#ifndef DISK_BUFFER
#define DISK_BUFFER

#include "globals.h"
#include "disk_io.h"
#include "logging.h"

namespace disk_buffer {

   // ==========================================
   // == FUNCIONES AUXILIARES: CONTEO FILAS ====
   // ==========================================

   void contar_datos_ram_una_tabla(std::string& nombre_tabla) {
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



   void contar_datos_en_ram_todas_tablas(){

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

   class tableRowIterator {
   public:
      uint32_t contador;
      table* tabla_ptr;
      bool eof;
      // Metodo constructor
      tableRowIterator(std::string tabla_nombre){
	 contador = 0;
         tabla_ptr = global_table_dict.at(tabla_nombre);
	 // Solo en caso de tenerlo, contamos las filas en RAM viva:
	 if(tabla_ptr->data_ptr){
            contar_datos_ram_una_tabla(tabla_nombre);
	 };
	 Logger::log(LogLevel::DEBUG, "Se han contado las filas en la RAM viva");
	 Logger::flush();
	 // Aqui deberismos cargar los dstos desde el disco:
	 if(!tabla_ptr->data_buffer_ptr){
		 // Solo leemos si existe el archivo:
		 if (fs::exists("data/" + tabla_nombre + ".bin")){
		    disk_io::read_table_data(tabla_ptr); // Esta funcion ya actusliza el conteo en disco
		 };
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
	 Logger::flush();
      };
      // Para consultar eof:
      bool is_eof() {
        return eof;
      };

      // Para devolver la prox fila:
      std::map<std::string, Values> get_next_row(){
         std::map<std::string, Values> map_fila_retornar;
	 uint32_t n_f_disk = tabla_ptr->metadata_ptr->n_filas_disco;
	 uint32_t n_f_ram = tabla_ptr->metadata_ptr->n_filas_ram;
	 uint32_t n_f_total = n_f_disk + n_f_ram;
	 Logger::log(LogLevel::DEBUG, "N filas disco: ", false, true);
	 Logger::log(LogLevel::DEBUG, n_f_disk, true, false);
	 Logger::log(LogLevel::DEBUG, "N filas RAM: ", false, true);
         Logger::log(LogLevel::DEBUG, n_f_ram, true, false);
	 Logger::flush();
         // Iteramos por cada columna:
         for (const std::string& nombre_col : tabla_ptr->metadata_ptr->column_names) {
	    if(contador < n_f_disk){
	       // Leemos primero desde disco:
	       Logger::log(LogLevel::DEBUG, "Leemos desde el disco");
	       Logger::flush();
               map_fila_retornar[nombre_col] = tabla_ptr->data_buffer_ptr->columns.at(nombre_col)[contador];
	    } else if(contador < n_f_total) {
		uint32_t indice_relativo_ram = contador - n_f_disk;
               // Leemos desde la RAM viva:
	       Logger::log(LogLevel::DEBUG, "Leemos desde RAM viva");
	       Logger::flush();
	       map_fila_retornar[nombre_col] = tabla_ptr->data_ptr->columns.at(nombre_col)[indice_relativo_ram];
	    };
         }; // Termina la iteracion de columna
         contador += 1;
         if(contador >= (tabla_ptr->metadata_ptr->n_filas_disco + tabla_ptr->metadata_ptr->n_filas_ram)){
            this-> eof = true;
	 };
         return map_fila_retornar;
      }; // Termina el metodo 'get_next_row'

   }; // Termina 'tableRowIterator' 

}; // Cierre del namespace 'disk_buffer'

#endif
