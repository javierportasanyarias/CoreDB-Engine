

#include "data_struct.h"
#include <iostream>
#include <fstream>
#include <string>
#include "globals.h"
#include <map>
#include "logging.h"
#include "filesystem"
#include "disk_wal.h"
#include "disk_buffer.h"
#include <cstring> // Para usar std::memcpy
#include "part_sort.h"
#include "disk_aux.h"
#include "disk_in.h"


////////////////////////////////////////////////////////////////////
// LECTURA DE DATOS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void disk_in::read_fixed_len_int_columns(std::string path_var, std::string partition_current, std::vector<Values>& vec_vals){

   Logger::log(LogLevel::DEBUG, "Dentro de la lectura de los INT");
   std::ifstream in;
   std::string ruta = path_var + partition_current + ".dat";
   Logger::log(LogLevel::DEBUG, "Abrimos el archivo de datos: ", false, true);
   Logger::log(LogLevel::DEBUG, ruta, true, false);
   in.open(ruta, std::ios::in | std::ios::binary);
   if (!in.is_open()) {
      Logger::log(LogLevel::ERROR, "¡No se pudo abrir el archivo! Ruta: ", false, true);
      Logger::log(LogLevel::ERROR, ruta, true, false);
      return; // Sal de la función si no hay archivo
   }else{
      Logger::log(LogLevel::DEBUG, "Archivo abierto en modo lectura");
   };

   // Hallamos el tamaño del archivo:
   std::streamsize file_size = disk_aux::return_file_size(in);
   Logger::log(LogLevel::DEBUG, "Se ha recuperado el tamaño del archivo");
   if(file_size > 0){
      Logger::log(LogLevel::DEBUG, "El archivo NO esta vacio");
      // Calculamos el tamaño:
      uint32_t num_elementos = file_size/sizeof(int);
      Logger::log(LogLevel::DEBUG, "Numero de leementos o numero de filas obtenido con exito");
      // Sólo si existe el archivo y no es nulo, lo leemos entero:
      Logger::log(LogLevel::DEBUG, "Inicializamos el buffer de caracteres");
      std::vector<char> buffer(file_size);
      Logger::log(LogLevel::DEBUG, "Buffer de caracteres incializado con exito");
      Logger::log(LogLevel::DEBUG, "Procedemos a realizar la lectura:");
      in.read(buffer.data(), file_size);
      Logger::log(LogLevel::DEBUG, "Lectura del disco realizada con exito");

      // Ajustamos el vector
      vec_vals.reserve(num_elementos);

      // Añadimos uno a uno los elementos:
      Logger::log(LogLevel::DEBUG, "Procedemos a adicionar 1 a 1 los elementos:");
      disk_aux::fill_vector_int(num_elementos, buffer, vec_vals);    
      Logger::log(LogLevel::DEBUG, "Elementos ya adicionados al std::vector<Values>");  
   } else{
      Logger::log(LogLevel::DEBUG, "El archivo ESTA vacio");
   };

   in.close();
};


void disk_in::read_fixed_len_float_columns(std::string path_var, std::string partition_current, std::vector<Values>& vec_vals){

   std::ifstream in;
   std::string ruta = path_var + partition_current + ".dat";
   in.open(ruta, std::ios::in | std::ios::binary);

   // Hallamos el tamaño del archivo:
   std::streamsize file_size = disk_aux::return_file_size(in);
   if(file_size > 0){
      // Calculamos el tamaño:
      uint32_t num_elementos = file_size/sizeof(float);
      // Sólo si existe el archivo y no es nulo, lo leemos entero:
      std::vector<char> buffer(file_size);
      in.read(buffer.data(), file_size);

      // Ajustamos el vector
      //vec_vals.resize(num_elementos);

      // Añadimos uno a uno los elementos:
      disk_aux::fill_vector_float(num_elementos, buffer, vec_vals);      
   };

   in.close();
};


void disk_in::read_fixed_len_bool_columns(std::string path_var, std::string partition_current, std::vector<Values>& vec_vals){

   std::ifstream in;
   std::string ruta = path_var + partition_current + ".dat";
   in.open(ruta, std::ios::in | std::ios::binary);

   // Hallamos el tamaño del archivo:
   std::streamsize file_size = disk_aux::return_file_size(in);
   if(file_size > 0){
      // Sólo si existe el archivo y no es nulo, lo leemos entero:
      std::vector<char> buffer(file_size);
      in.read(buffer.data(), file_size);

      // Calculamos el tamaño:
      uint32_t num_elementos = file_size/sizeof(uint8_t);

      // Ahora iteramos uno a uno para recuperar cada valor:
      uint32_t puntero_aux = 0;
      uint8_t tmp_bool_number;
      bool tmp_bool;
      for(int i = 0; i<num_elementos; i++){
         tmp_bool = false;
         // Leemos la variable uint32_t
         std::memcpy(
            &tmp_bool_number,
            buffer.data() + puntero_aux,
            1
         );
         // Convertimos la variable a booleano:
         if(tmp_bool_number == 1){
            tmp_bool = true;;
         };
         // Lo añadimos al buffer con tipo bool:
         vec_vals.push_back(tmp_bool);
      };
   };
   in.close();
};


void disk_in::read_fixed_len_string_columns(std::string path_var, std::string partition_current, std::vector<Values>& vec_vals){

   std::ifstream in_str;
   std::ifstream in_idx;
   std::string ruta_str = path_var + partition_current + ".bin";
   std::string ruta_idx = path_var + partition_current + ".idx";
   in_str.open(ruta_str, std::ios::in | std::ios::binary);
   in_idx.open(ruta_idx, std::ios::in | std::ios::binary);

   // Hallamos el tamaño del archivo:
   std::streamsize file_size_str = disk_aux::return_file_size(in_str);
   std::streamsize file_size_idx = disk_aux::return_file_size(in_idx);
   uint32_t tmp_size = 0;
   if(file_size_idx > 0){
      // Calculamos el tamaño:
      uint32_t num_elementos = file_size_idx/sizeof(uint32_t);
      // Sólo si existe el archivo y no es nulo, lo leemos entero:
      std::vector<char> buffer_str(file_size_str);
      std::vector<char> buffer_idx(file_size_idx);
      in_str.read(buffer_str.data(), file_size_str);
      in_idx.read(buffer_idx.data(), file_size_idx);
      // Ajustamos el vector
      //vec_vals.resize(num_elementos);

      // Añadimos uno a uno los elementos:
      disk_aux::fill_vector_string(num_elementos, buffer_str, buffer_idx, vec_vals);      
   };

   in_str.close();
   in_idx.close();
};



disk_in::read_table_iterator::read_table_iterator(table* tabla){
   /*
   Método constructor en el cual:
   Leemos y registramos lo necesario para realizar las sucesivas lecturas
   */
   Logger::log(LogLevel::DEBUG, "INICIAMOS LA LECTURA DE LOS DATOS DE LA TABLA");

   this->tabla = tabla;
   // Recuperamos el nombre:
   this->nombre_tabla = this->tabla->metadata_ptr->name;
   // El nombre de las columnas:
   this->nombre_columnas = this->tabla->metadata_ptr->column_names;
   this->num_cols = nombre_columnas.size();
   // Recuperamos el tipo de las columnas:
   this->tipo_columnas = this->tabla->metadata_ptr->column_types;
   // Inicializamos la variable por la cual añadiremos los datos venidos del disco:
   if(this->tabla->data_buffer_ptr == nullptr){
      this->tabla->data_buffer_ptr = new table_data_buffer();
   }
   // Ahora apuntamos nuestro puntero interno al que ya tiene (o acaba de recibir) la tabla
   this->ptr_datos_disco = this->tabla->data_buffer_ptr;
   Logger::log(LogLevel::DEBUG, "Recuperdos valores de la tabla y inicializado el apartado del disco en la tabla");
   //std::map<std::string, std::vector<Values>>& buffer_ram_del_disco = ptr_datos_disco->columns;

   // Ahora vemos que particiones hay:
   this->partition_counter = 0;
   this->total_partitions = 0;
   Logger::log(LogLevel::DEBUG, "Pasamos a obtener los archivos een la ruta:");
   Logger::log(LogLevel::DEBUG, "data/" + this->nombre_tabla + "/" + this->nombre_columnas[0]);
   // Ahora vemos si la columa primera o con indice 0 es de tamaño fijo o variable:

   this->particiones_nombres = disk_aux::obtener_archivos_en_ruta("data/" + this->nombre_tabla + "/" + this->nombre_columnas[0], this->tipo_columnas[0]);
   Logger::log(LogLevel::DEBUG, "Nombres de las particiones obtenidos con exito");

   if(!(this->particiones_nombres.empty())){
      Logger::log(LogLevel::DEBUG, "SI que hay archivos en DISCO");
      Logger::log(LogLevel::DEBUG, "Procedemos a ordenarlas de menor a mayor:");
      part_sort::bubble_sort(this->particiones_nombres);
      Logger::log(LogLevel::DEBUG, "Ordenamoento de las particiones realizado con exito");
      Logger::log(LogLevel::DEBUG, "Hallamos el número de particiones:");
      this->total_partitions = this->particiones_nombres.size();
      /*if(this->tipo_columnas[0] == dataType::STRING){
         this->total_partitions = this->total_partitions / 2;
      };*/
      Logger::log(LogLevel::DEBUG, "Numero de particones encontradas: ", false, true);
      Logger::log(LogLevel::DEBUG, this->total_partitions, true, false);
   };


   ////////////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////////////
   Logger::log(LogLevel::DEBUG, "Todo listo, procedemos a entrar en el bucle de las columnas:");

};

bool disk_in::read_table_iterator::obtain_int_partition_rows(bool aux_bool, std::string path_var, std::string partition_current){
   
   if(aux_bool){
      Logger::log(LogLevel::DEBUG, "<<<<<CASO INT VEMOS TAMANO PARTICION>>>>>>>");
      uint32_t size_tmp = 0;
      Logger::log(LogLevel::DEBUG, "Primera ejecucion. Vemos el tamano de la particion: ");
      Logger::log(LogLevel::DEBUG, path_var + partition_current);
      size_tmp = disk_aux::obtener_tamano_archivo(path_var + partition_current + ".dat");
      Logger::log(LogLevel::DEBUG, "Tamano del archivo de particion: ", false, true);
      Logger::log(LogLevel::DEBUG, size_tmp, true, false);
      Logger::log(LogLevel::DEBUG, "Tamano de la particion recuperado, lo registramos");
      this->numero_de_filas_current_partition =  size_tmp / sizeof(int);
      Logger::log(LogLevel::DEBUG, "Queda registrado el numero de filas de la particion: ", false, true);
      Logger::log(LogLevel::DEBUG, this->numero_de_filas_current_partition, true, false);
      aux_bool = false;
   };
   return aux_bool;
};


bool disk_in::read_table_iterator::obtain_float_partition_rows(bool aux_bool, std::string path_var, std::string partition_current){
   
   if(aux_bool){
      Logger::log(LogLevel::DEBUG, "<<<<<CASO FLOAT VEMOS TAMANO PARTICION>>>>>>>");
      uint32_t size_tmp = 0;
      Logger::log(LogLevel::DEBUG, "Primera ejecucion. Vemos el tamano de la particion: ");
      Logger::log(LogLevel::DEBUG, path_var + partition_current);
      size_tmp = disk_aux::obtener_tamano_archivo(path_var + partition_current + ".dat");
      Logger::log(LogLevel::DEBUG, "Tamano del archivo de particion: ", false, true);
      Logger::log(LogLevel::DEBUG, size_tmp, true, false);
      Logger::log(LogLevel::DEBUG, "Tamano de la particion recuperado, lo registramos");
      this->numero_de_filas_current_partition =  size_tmp / sizeof(float);
      Logger::log(LogLevel::DEBUG, "Queda registrado el numero de filas de la particion: ", false, true);
      Logger::log(LogLevel::DEBUG, this->numero_de_filas_current_partition, true, false);
      aux_bool = false;
   };
   return aux_bool;
};


bool disk_in::read_table_iterator::obtain_bool_partition_rows(bool aux_bool, std::string path_var, std::string partition_current){
   
   if(aux_bool){
      uint32_t size_tmp = 0;
      Logger::log(LogLevel::DEBUG, "<<<<<CASO BOOL VEMOS TAMANO PARTICION>>>>>>>");
      Logger::log(LogLevel::DEBUG, "Primera ejecucion. Vemos el tamano de la particion: ");
      Logger::log(LogLevel::DEBUG, path_var + partition_current);
      size_tmp = disk_aux::obtener_tamano_archivo(path_var + partition_current + ".dat");
      Logger::log(LogLevel::DEBUG, "Tamano del archivo de particion: ", false, true);
      Logger::log(LogLevel::DEBUG, size_tmp, true, false);
      Logger::log(LogLevel::DEBUG, "Tamano de la particion recuperado, lo registramos");
      this->numero_de_filas_current_partition =  size_tmp;
      Logger::log(LogLevel::DEBUG, "Queda registrado el numero de filas de la particion: ", false, true);
      Logger::log(LogLevel::DEBUG, this->numero_de_filas_current_partition, true, false);
      aux_bool = false;
   };
   return aux_bool;
};


bool disk_in::read_table_iterator::obtain_string_partition_rows(bool aux_bool, std::string path_var, std::string partition_current){
   
   if(aux_bool){
      uint32_t size_tmp = 0;
      Logger::log(LogLevel::DEBUG, "<<<<<CASO STRING VEMOS TAMANO PARTICION>>>>>>>");
      Logger::log(LogLevel::DEBUG, "Primera ejecucion. Vemos el tamano de la particion: ");
      Logger::log(LogLevel::DEBUG, path_var + partition_current);
      size_tmp = disk_aux::obtener_tamano_archivo(path_var + partition_current + ".idx");
      Logger::log(LogLevel::DEBUG, "Tamano del archivo de particion: ", false, true);
      Logger::log(LogLevel::DEBUG, size_tmp, true, false);
      Logger::log(LogLevel::DEBUG, "Tamano de la particion recuperado, lo registramos");
      this->numero_de_filas_current_partition =  size_tmp / sizeof(uint32_t);
      Logger::log(LogLevel::DEBUG, "Queda registrado el numero de filas de la particion: ", false, true);
      Logger::log(LogLevel::DEBUG, this->numero_de_filas_current_partition, true, false);
      aux_bool = false;
   };
   return aux_bool;
};

bool disk_in::read_table_iterator::read_table(){

   Logger::flush();
   Logger::log(LogLevel::DEBUG, "Dentro de 'read_table'");

   if(this->partition_counter < this->total_partitions){
      Logger::log(LogLevel::DEBUG, "Particiones leidas: ", false, true);
      Logger::log(LogLevel::DEBUG, this->partition_counter, true, false);
      Logger::log(LogLevel::DEBUG, "Particiones totales: ", false, true);
      Logger::log(LogLevel::DEBUG, this->total_partitions, true, false);
      // En este caso preparamos dónde se almacenarán los datos:
      Logger::log(LogLevel::DEBUG, "Accedemos al vector de std::map del disco:");
      std::map<std::string, std::vector<Values>>& buffer_ram_del_disco = this->ptr_datos_disco->columns;
      Logger::log(LogLevel::DEBUG, "std::map del disco hallado");
      Logger::log(LogLevel::DEBUG, "Limpiamos el std::map del disco:");
      buffer_ram_del_disco.clear();
      Logger::log(LogLevel::DEBUG, "std::map del disco limpiado con exito");
      bool aux_bool = true;
   
      for(int i = 0; i< this->num_cols; i++){

         dataType tipo_dato = tipo_columnas[i];
         std::string column_name = this->nombre_columnas[i];
         Logger::log(LogLevel::DEBUG, "Procesamos la columna: ", false, true);
         Logger::log(LogLevel::DEBUG, column_name, true, false);
         Logger::log(LogLevel::DEBUG, "Obtenemos la ruta: ", false, true);
         std::string path_var = "data/" + this->nombre_tabla + "/" + column_name + "/";
         Logger::log(LogLevel::DEBUG, path_var, true, false);
         Logger::log(LogLevel::DEBUG, "Accdemos a la particion actual: ", false, true);
         std::string partition_current = this->particiones_nombres[this->partition_counter];
         // Le quitamos la extension al nombre:
         partition_current = part_sort::quitar_extension_archivo(partition_current);
         Logger::log(LogLevel::DEBUG, partition_current, true, false);
         std::vector<Values> vec_vals;

         switch(tipo_dato){
            case dataType::INT: {
               Logger::log(LogLevel::DEBUG, "Caso INT");
               Logger::log(LogLevel::DEBUG, "++++++ CASE INT DEL SWITCH ++++++");
               aux_bool = this->obtain_int_partition_rows(aux_bool, path_var, partition_current);
               Logger::log(LogLevel::DEBUG, "Pasamos a leer esta particion de INTs:");
               disk_in::read_fixed_len_int_columns(path_var, partition_current, vec_vals);
               // Adicionamos estos datos a la tabla, en la region de datos del disco:
               Logger::log(LogLevel::DEBUG, "Procedemos a adicionar el vector de INTs a la TABLA");
               buffer_ram_del_disco[column_name] = vec_vals;
               Logger::log(LogLevel::DEBUG, "Vector adicionado a la TABLA con exito");
               break;
            };
            case dataType::FLOAT: {
               Logger::log(LogLevel::DEBUG, "++++++ CASE FLOAT DEL SWITCH ++++++");
               aux_bool = this->obtain_float_partition_rows(aux_bool, path_var, partition_current);
               Logger::log(LogLevel::DEBUG, "Caso FLOAT");
               //std::vector<float> vec_vals;
               Logger::log(LogLevel::DEBUG, "Procedemos a adicionar el vector de FLOATs a la TABLA");
               disk_in::read_fixed_len_float_columns(path_var, partition_current, vec_vals);
               // Adicionamos estos datos a la tabla, en la region de datos del disco:
               buffer_ram_del_disco[column_name] = vec_vals;
               Logger::log(LogLevel::DEBUG, "Vector adicionado a la TABLA con exito");
               break;
            };
            case dataType::BOOL: {
               Logger::log(LogLevel::DEBUG, "++++++ CASE BOOL DEL SWITCH ++++++");
               aux_bool = this->obtain_bool_partition_rows(aux_bool, path_var, partition_current);
               //std::vector<uint8_t> vec_vals;
               Logger::log(LogLevel::DEBUG, "Procedemos a adicionar el vector de BOOLs a la TABLA");
               disk_in::read_fixed_len_bool_columns(path_var, partition_current, vec_vals);
               buffer_ram_del_disco[column_name] = vec_vals;
               Logger::log(LogLevel::DEBUG, "Vector adicionado a la TABLA con exito");
               break;
            };
            case dataType::STRING: {
               Logger::log(LogLevel::DEBUG, "++++++ CASE STRING DEL SWITCH ++++++");
               aux_bool = this->obtain_string_partition_rows(aux_bool, path_var, partition_current);
               Logger::log(LogLevel::DEBUG, "Procedemos a adicionar el vector de STRINGs a la TABLA");
               disk_in::read_fixed_len_string_columns(path_var, partition_current, vec_vals);
               buffer_ram_del_disco[column_name] = vec_vals;
               Logger::log(LogLevel::DEBUG, "Vector adicionado a la TABLA con exito");
               break;
            };
         };
      };
      Logger::log(LogLevel::DEBUG, "!!!!!!! Sumamos una a la particion !!!!!!!");
      this->partition_counter += 1;
      return false;
   };
   Logger::log(LogLevel::DEBUG, "No quedan particiones por leer");
   // En el caso de ya haberse quedado sin particiones que leer, limpiamos la region de la tabla para datos leidos del disco:
   std::map<std::string, std::vector<Values>>& buffer_ram_del_disco = this->ptr_datos_disco->columns;
   buffer_ram_del_disco.clear();
   return true; // En caso de no haber leído naada, porque ya no hay datos
   // Ya habríamos aacabado la lectura, avanzamos en uno la partición:

};
