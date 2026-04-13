#include <iostream>
#include <vector>
// #include "data_structure/data_struct.h"
#include "data_struct.h"
#include <unordered_map>
// #include "globals/globals.h"
#include "globals.h"
#include <variant>
#include "execution.h"
#include "logging.h"

#include <cctype> // Para isprint
#include "disk_io.h"
#include "disk_wal.h"
#include "disk_buffer.h"



///////////////////////////////////////////////////////////////////////
///
///
// Tipos de datos admitidos en el esquema:
dataType transform_str_to_datatype(const std::string& input){
   /*
   Función que dado un string que indique el tipo de dato, devuelve dicho tipo
   */
   if (input == "INT"){
      return dataType::INT;
   } else if(input == "FLOAT"){
      return dataType::FLOAT;
   } else if(input == "BOOL"){
      return dataType::BOOL;
   } else if(input == "STRING"){
      return dataType::STRING;
   } else{
      return dataType::UNKNOWN;
   };
};

////////////////////////////////////////////////////////////////////////////////////////
/// PARA RELLENAR LAS TABLAS:


void fill_table_with_values_v4(NodeType3* nodo_ptr) {

   // 1. Comprobaciones de seguridad
   auto it = global_table_dict.find(nodo_ptr->nombre_tabla);
   if (it == global_table_dict.end()) {
      throw std::runtime_error("No se ha encontrado la tabla en el diccionario global");
   };

   table* tb_recup = it->second;
   if (!tb_recup || !tb_recup->metadata_ptr) {
      throw std::runtime_error("Error: Puntero de tabla o metadatos nulo");
   }
   table_metadata* metadatos = tb_recup->metadata_ptr;

   if (!tb_recup->data_ptr) {
      tb_recup->data_ptr = new table_data;
   };

   // Iteramos para recuperar los valores:
   // En el caso de no haber especificsdo cooumnas, las pillamps de los ketadatos:
   std::vector<std::string> nombres_columnas = nodo_ptr->columnas;
   size_t len_n_cols = nombres_columnas.size();
   if(len_n_cols == 0){
      nombres_columnas = metadatos->column_names;
      len_n_cols = nombres_columnas.size();
   };
   //size_t len_n_cols = nombres_columnas.size();
   std::vector<std::vector<std::string>> filas = nodo_ptr->filas;

   //size_t num_fila = filas.size();
   uint32_t num_fila = filas.size();
   for(int i = 0; i<len_n_cols; i++){
      // Ahora iteramos por las filas:
      for(int j = 0; j<num_fila; j++){
	      // Recuperamos el valor como string:
	      std::string valor_str = filas[j][i];
         if(Logger::level == LogLevel::DEBUG){
            Logger::flush();
         }else{
            Logger::flush(false);
         };
	      Values valor_variante; // Valor variante
         // Hacemos la conversion de valor de acuerdo a los metadatos:
         dataType tipo_dato= metadatos->column_types[i];
         switch(tipo_dato){
            case dataType::INT:
               valor_variante = std::stoi(valor_str);
               break;
            case dataType::FLOAT:
               valor_variante = std::stof(valor_str);
               break;
            case dataType::BOOL:
               if(valor_str == "true" || valor_str == "True" || valor_str == "TRUE"){
                  valor_variante = true;
               }else if(valor_str == "false" || valor_str == "False" || valor_str == "FALSE"){
                  valor_variante = false;
               };
               break;
            case dataType::STRING:
               valor_variante = valor_str;
               break;
         };
         // Ya tenemos el valor variante, ahora rellenamos los datos de la tabla en la RAM viva:
         auto it = tb_recup->data_ptr->columns.find(nombres_columnas[i]);
         if(it != tb_recup->data_ptr->columns.end()){
            it->second.push_back(valor_variante);
         }else{
            tb_recup->data_ptr->columns[nombres_columnas[i]].push_back(valor_variante);
         };
      }; // Cierre de la escritura de cada fila
   };
   tb_recup->metadata_ptr->n_filas_ram += num_fila;
   /*
   Aquí incrementamos el contador de las filas en RAM viva.
   De esta forma se actualizan cuando los valores se añaden y
   no es necesarias contarlas a posteriori
   */
   // Ejecutamos la escritura en el WAL de los archivos
   disk_wal::write_table_data_wal(tb_recup, num_fila);

};

////////////////////////////////////////////////////////////////////////////////////////
/// PARA RELLENAR METADATOS:

void recursive_metadata_fill_lv2(NodeType2* nodo_ptr, table* tb_created){
   if(!nodo_ptr){
      return;
   };
   // Rellenamos los datos del campo a tratar:
   auto& metadata = *tb_created->metadata_ptr;
   (metadata.column_names).push_back(std::move(nodo_ptr->name_campo));
   (metadata.column_types).push_back(transform_str_to_datatype(nodo_ptr->tipo));
   if(nodo_ptr->is_primary){
      (metadata.primary_list).push_back(true);
   };
   // Estos nodos no rienen hijos
   return;
};

void recursive_metadata_fill_lv1(NodeType1* nodo_ptr){

   // Creamos un struct de la tabla:
   table* tb_created = global_table_dict[nodo_ptr->nombre_tabla];
   if(!nodo_ptr){
      return;
   };

   tb_created->metadata_ptr = new table_metadata;

   // Declaramos el nombre de la tabla:
   ((*(tb_created->metadata_ptr)).name) = nodo_ptr->nombre_tabla;
   // Recorremos los hijos:N_
   for(int i = 0; i<(*nodo_ptr).hijos.size(); i++){
      recursive_metadata_fill_lv2((nodo_ptr->hijos)[i], tb_created);
   };
   //global_table_dict[nodo_ptr->nombre_tabla] = tb_created;
   /*
   Ya hemos rellenado los metadatos, ahora los registramos en el WAL
   */
   Logger::log(LogLevel::DEBUG, "YA SE HA CREADO LOS METADATOS DE LA TABLA");
   Logger::log(LogLevel::DEBUG, "Pasamos a escribir los metadatos en el WAL:");

   Logger::log(LogLevel::DEBUG, "<<<<<< ESCRIBIMOS EN EL WAL LA TABLA: ", false, true);
   Logger::log(LogLevel::DEBUG, tb_created->metadata_ptr->name, false, false);
   Logger::log(LogLevel::DEBUG, " >>>>>>>>", true, false);
   Logger::flush();
   disk_wal::write_table_wal_metadata(tb_created);
   return;
};


///////////////////////////////////////////////////////////////
// MOSTRAR TABLA:

// Función auxiliar que imprime los valores de la tabla (CASO TODAS LAS COLUMNAS):
void aux_table_values_print(const std::vector<std::string>& col_list, std::string nombre_tabla) {
   // Creamos el iterador de filas de la tabla:
   disk_buffer::tableRowIterator it(nombre_tabla);
   std::map<std::string, Values> fila;
    
	while(!it.is_eof()){
      // consultamos la proxima fila:
	   fila = it.get_next_row();
	   // Ya tenemos la fila, iteramos por la seleccion de columnas:
	   Logger::log(LogLevel::OUTPUT, " | ", false, false);
	   for(const std::string& nombre_col: col_list){
		   Logger::log(LogLevel::OUTPUT, fila.at(nombre_col), false, false);
		   Logger::log(LogLevel::OUTPUT, " | ", false, false);
	   };
	   Logger::flush(); 
	};
   Logger::flush();
};

// Función auxiliar que imprime los valores de la tabla (SE HAN SELECCIONADO COLUMNAS):
void aux_table_values_print(const std::vector<ItemNode>& col_list, std::string nombre_tabla) {                                                                                                      
   // Creamos el iterador de filas de la tabla:
   disk_buffer::tableRowIterator it(nombre_tabla);
   std::map<std::string, Values> fila;

   while(!it.is_eof()){
      // consultamos la proxima fila:
      fila = it.get_next_row();
      // Ya tenemos la fila, iteramos por la seleccion de columnas:
      Logger::log(LogLevel::OUTPUT, " | ", false, false);
      for(const ItemNode& elemento: col_list){
		   auto elemento_valor = fila.find(elemento.nombre);
		   if(elemento_valor == fila.end()){
		      throw std::runtime_error("Error al mostrar la tabla, la columna: "+ elemento.nombre + " no existe");
		   };
         Logger::log(LogLevel::OUTPUT, elemento_valor->second, false, false);
         Logger::log(LogLevel::OUTPUT, " | ", false, false);                                                                                                                                        
      };                                                                                              
      Logger::flush();
   };
   Logger::flush();
};

// Función auxiliar para imprimir la tabla: PARA TODAS LAS COLUMNAS:
void imprimir_tabla(const std::vector<std::string>& col_list, std::string nombre_tabla){
   // Imprimios los nombres de las columnas:
   for(int  i = 0; i<col_list.size(); i++){
      Logger::log(LogLevel::OUTPUT, " | ", false); // sin flush automático
      Logger::log(LogLevel::OUTPUT, col_list[i], false);
   };
   Logger::log(LogLevel::OUTPUT, " | ", true);
   // Imprimimos los valores:
   aux_table_values_print(col_list, nombre_tabla);
};

// impresión de toda la tabla, pero habiendo seleccionado clumnas
void imprimir_tabla(QueryNode*& nodo_root, const std::vector<ItemNode>& col_list, std::string nombre_tabla){
   // Imprimios los nombres de las columnas:
       for(int i = 0; i<(nodo_root->nodo_select->items).size(); i++){
         Logger::log(LogLevel::OUTPUT, " | ", false); // sin flush automático
         Logger::log(LogLevel::OUTPUT, (nodo_root->nodo_select)->items[i].nombre, false);
       };
       Logger::log(LogLevel::OUTPUT, " | ", true);
   // Imprimimos los valores:
   aux_table_values_print(col_list, nombre_tabla);
};

void mostrar_tabla_query(QueryNode* nodo_root){

   //recuperamos el nombre de la tabla
   std::string nombre_tabla = nodo_root->nodo_from->nombre;
   // Antes de nada vemos si es una columna valida:
   if(!global_table_dict.at(nombre_tabla)){
      Logger::log(LogLevel::ERROR, "La tabla a consultar no existe");
      return;
   };

   if (nodo_root->nodo_select == nullptr) {
      // La lista no existe o está vacía:
      // Guardamos en memoria valores del diccionario accedidos con regularidad:
      const std::vector<std::string>& col_list = (global_table_dict.at(nombre_tabla)->metadata_ptr)->column_names;
      imprimir_tabla(col_list, nombre_tabla);
   } else {
      //Ahora imprimimos los valores:
      const std::vector<ItemNode>& col_list = (nodo_root->nodo_select->items);
      imprimir_tabla(nodo_root, col_list, nombre_tabla);
   };
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Eliminación de tablas:
void liberar_tabla(table*& tb){
   /*
   Función encargada de eliminar la tabla y sus datos:
   La función se dividirá en:
   1) Eliminación de los archivos en disco, si los hay
   2) Eliminación de los datos en la memoria volátil
   */
   //////////////////////////
   //1) Eliminación de los datos en disco:
   disk_io::eliminar_archivo_binario_metadatos(tb);
   disk_io::eliminar_archivo_binario_datos(tb);
   //////////////////////////
   // Eliminación de los datos en RAM:

   // Eliminamos los metadatos:
   if (!tb) return;
   delete tb->metadata_ptr;
   tb->metadata_ptr = nullptr;
   // Eliminamos los datos en RAM viva:
   if(tb->data_ptr){
      delete tb->data_ptr;
      tb->data_ptr = nullptr;
   };
   // Eliminamos los datos en RAM venidos del disco:
   if(tb->data_buffer_ptr){
      delete tb->data_buffer_ptr;
      tb->data_buffer_ptr = nullptr;
   };
   delete tb;
};

void drop_table_from_global_dict(DropTableNode*& nodo_ptr){

   auto it = global_table_dict.find(nodo_ptr->nombre_tabla);
   // Ahora vemos si estq vacio o no:
   if(it == global_table_dict.end()){
      // No existe la entrada:
      Logger::log(LogLevel::OUTPUT, "NO EXISTE LA TABLA, NO SE PUEDE ELIMINAR");
      return;
   }else{
      table* tabla = it->second;
      global_table_dict.erase(nodo_ptr->nombre_tabla);
      liberar_tabla(tabla);
      tabla = nullptr;
   };
   
};
// =================================
// == ELIMNAR ENTRADAS NULAS:
// =================================

void sanitize_global_dict() {
   auto it = global_table_dict.begin();
   while (it != global_table_dict.end()) {
      const std::string& nombre = it->first;
      table* ptr = it->second;
      bool eliminar = false;

      // 1. Validación de Longitud
      if (nombre.length() == 0) {
         Logger::log(LogLevel::WARN, "Sanitizador: Nombre de longitud 0 detectado.");
         eliminar = true;
      }
      // 2. Validación de Contenido (¿Es imprimible?)
      else if (!std::isprint(static_cast<unsigned char>(nombre[0]))) {
         Logger::log(LogLevel::WARN, "Sanitizador: Carácter no imprimible detectado en la clave.");
         eliminar = true;
      }
      // 3. Validación de Puntero
      else if (ptr == nullptr) {
         Logger::log(LogLevel::WARN, "Sanitizador: Puntero nulo para tabla '" + nombre + "'.");
         eliminar = true;
      }

      if (eliminar) {
         // Importante: liberar la memoria del contenido si el puntero no es nulo
         // pero la clave está corrupta, para evitar leaks.
         if (ptr != nullptr) {
            liberar_tabla(ptr);
         }
         it = global_table_dict.erase(it);
      } else {
         ++it;
      }
    }
};
