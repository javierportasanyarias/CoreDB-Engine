#include "disk_wal_write.h"
#include "disk_aux.h"
#include "disk_buffer.h"
#include "node_for_trees.h"
#include "execution.h"


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


void fill_table_with_values_v4(NodeType3* node_ptr_input) {

   // 1. Comprobaciones de seguridad
   auto it = global_table_dict.find(node_ptr_input->table_name);
   if (it == global_table_dict.end()) {
      throw std::runtime_error("Error: Table has not been found within the global table dictionary");
   };

   table* tb_recovered = it->second;
   if (!tb_recovered || !tb_recovered->metadata_ptr) {
      throw std::runtime_error("Error: Metadata or table pointer null");
   }
   table_metadata* metadata_ptr = tb_recovered->metadata_ptr;

   if (!tb_recovered->data_ptr) {
      tb_recovered->data_ptr = new table_data;
   };

   // Iteramos para recuperar los valores:
   // En el caso de no haber especificsdo cooumnas, las pillamps de los ketadatos:
   std::vector<std::string> column_names = node_ptr_input->columns;
   column_names = metadata_ptr->column_names;
   Logger::log(LogLevel::DEBUG,  "Quering number of columns");
   uint32_t len_n_cols = metadata_ptr->n_cols;
   Logger::log(LogLevel::DEBUG,  "Nº of columns: ", false, true);
   Logger::log(LogLevel::DEBUG,  len_n_cols, true, false);
   if(len_n_cols == 0){
      len_n_cols = column_names.size();
   };
   std::vector<std::vector<std::string>> rows = node_ptr_input->rows;

   uint32_t row_num = rows.size();


//////////////////////////////////////////////////////////////////////////
   Logger::log(LogLevel::DEBUG, "Starting value insertion loop");
   for(int j = 0; j<row_num; j++){
      for(int i = 0; i<len_n_cols; i++){
	      // Recuperamos el valor como string:
	      std::string value_str = rows[j][i];
         if(Logger::level == LogLevel::DEBUG){
            Logger::flush(LogLevel::DEBUG);
         }else{
            Logger::flush(LogLevel::DEBUG, false);
         };
	      Values variant_value; // Valor variante
         // Hacemos la conversion de valor de acuerdo a los metadatos:
         dataType data_type = metadata_ptr->column_types[i];
         switch(data_type){
            case dataType::INT:
               Logger::log(LogLevel::DEBUG, "Value integer conversion: ", false, true);
               Logger::log(LogLevel::DEBUG, value_str, true, false);
               variant_value = std::stoi(value_str);
               break;
            case dataType::FLOAT:
               Logger::log(LogLevel::DEBUG, "Value float conversion: ", false, true);
               Logger::log(LogLevel::DEBUG, value_str, true, false);
               variant_value = std::stof(value_str);
               break;
            case dataType::BOOL:
               if(value_str == "true" || value_str == "True" || value_str == "TRUE"){
                  variant_value = true;
               }else if(value_str == "false" || value_str == "False" || value_str == "FALSE"){
                  variant_value = false;
               };
               break;
            case dataType::STRING:
               variant_value = value_str;
               break;
            // New case: UNKNOWN -> We will treat these data types as a vector of chars:
            case dataType::UNKNOWN:
               std::vector<char> unknown_bytes(value_str.begin(), value_str.end());
               variant_value = std::move(unknown_bytes);
         };
         Logger::log(LogLevel::DEBUG, "'variant_value' retrieved");
         // Ya tenemos el valor variante, ahora rellenamos los datos de la tabla en la RAM viva:
         Logger::log(LogLevel::DEBUG, "Filling values within the 'RAM' or live session data section");
         auto it = tb_recovered->data_ptr->columns.find(column_names[i]);
         if(it != tb_recovered->data_ptr->columns.end()){
            Logger::log(LogLevel::DEBUG, "Pushing first value");
            it->second.push_back(variant_value);
            Logger::log(LogLevel::DEBUG, "First value push successful");
         }else{
            Logger::log(LogLevel::DEBUG, "Pushing other value");
            tb_recovered->data_ptr->columns[column_names[i]].push_back(variant_value);
            Logger::log(LogLevel::DEBUG, "other value push successful");
         };
         Logger::log(LogLevel::DEBUG, "Valor processed successfully");
      }; // Cierre de la escritura de cada columna
   };
   // We update the rows' metadata counter:
   Logger::log(LogLevel::DEBUG, "Updating metadata row counter");
   metadata_ptr->n_rows_ram += row_num;
   /*
   Aquí incrementamos el contador de las filas en RAM viva.
   De esta forma se actualizan cuando los valores se añaden y
   no es necesarias contarlas a posteriori
   */
   // Ejecutamos la escritura en el WAL de los archivos
   disk_wal_write::write_table_data_wal(tb_recovered, row_num);
   Logger::flush(LogLevel::DEBUG);
   Logger::flush(LogLevel::DEBUG);
   Logger::log(LogLevel::DEBUG, "Data insertion in 'RAM' is finished");
   Logger::log(LogLevel::DEBUG, "Total number of rows inserted are: ", false, true);
   Logger::log(LogLevel::DEBUG, metadata_ptr->n_rows_ram, true, false);
   Logger::flush(LogLevel::DEBUG);
   Logger::flush(LogLevel::DEBUG);
};

////////////////////////////////////////////////////////////////////////////////////////
/// PARA RELLENAR METADATOS:

void recursive_metadata_fill_lv2(NodeType2* node_ptr_input, table* tb_created){
   if(!node_ptr_input){
      return;
   };
   // Filed data fill:
   auto& metadata = *tb_created->metadata_ptr;
   (metadata.column_names).push_back(std::move(node_ptr_input->filed_name));
   (metadata.column_types).push_back(transform_str_to_datatype(node_ptr_input->type));
   // Node push, wether it is a primary key or not:
   (metadata.primary_list).push_back(node_ptr_input->is_primary);

   return;
};

void recursive_metadata_fill_lv1(NodeType1* node_ptr_input){

   if(!node_ptr_input){
      return;
   };

   // Before creating table metadata, we make sure it does not exist previously:
   auto it = global_table_dict.find(node_ptr_input->table_name);
   if(it != global_table_dict.end()){
      Logger::log(LogLevel::ERROR, "Error while creating the table: '" + node_ptr_input->table_name + "' . It already exists");
      throw std::runtime_error("CREATE TABLE ERROR: Can not define an already existing table");
   };


   // We crate the table and add it's entry to the dictionay:
   table* tb_created = new table;
   global_table_dict[node_ptr_input->table_name] = tb_created;
   // We initialize the table's metadata struct:
   table_metadata* metadata_pointer = new table_metadata;
   tb_created->metadata_ptr = metadata_pointer;
   // Just in case, we set the table's data pointers to null:
   tb_created->data_ptr = nullptr;
   tb_created->data_buffer_ptr = nullptr;

   // Declaramos el nombre de la tabla:
   metadata_pointer->name = node_ptr_input->table_name;
   // We set the number of cols in he metadata:
   uint32_t& num_cols = metadata_pointer->n_cols;
   num_cols = (*node_ptr_input).children.size();
   // Recorremos los hijos:N_
   for(int i = 0; i<num_cols; i++){
      recursive_metadata_fill_lv2((node_ptr_input->children)[i], tb_created);
   };

   Logger::log(LogLevel::DEBUG, "TABLE METADATA HAS BEEN CREATED");
   Logger::log(LogLevel::DEBUG, "Proceeding to write table metadata in WAL file:");

   Logger::log(LogLevel::DEBUG, "<<<<<< Writing WAL metadata file from table: ", false, true);
   Logger::log(LogLevel::DEBUG, metadata_pointer->name, false, false);
   Logger::log(LogLevel::DEBUG, " >>>>>>>>", true, false);
   Logger::flush(LogLevel::DEBUG);
   disk_wal_write::write_table_wal_metadata(tb_created);
   return;
};


///////////////////////////////////////////////////////////////
// SHOW TABLE:

// Función auxiliar que imprime los valores de la tabla (CASO TODAS LAS COLUMNAS):
void aux_table_values_print(const std::vector<std::string>& col_list, std::string table_name) {
   // Creamos el iterador de filas de la tabla:
   disk_buffer::tableRowIterator it(table_name);
   std::map<std::string, Values> row_map;

	while(!it.is_eof()){
      // consultamos la proxima row_map:
	   row_map = it.get_next_row();
      // Antes vemos si está vacía o no:
      if (row_map.empty()) {
         // Escudo contra filas vacías antess de marcar que se ha llegado a EOF
         continue;
      };
	   // Ya tenemos la row_map, iteramos por la seleccion de columnas:
	   Logger::log(LogLevel::OUTPUT, " | ", false, false);
	   for(const std::string& col_name: col_list){
		   Logger::log(LogLevel::OUTPUT, row_map.at(col_name), false, false);
		   Logger::log(LogLevel::OUTPUT, " | ", false, false);
	   };
	   Logger::flush(LogLevel::OUTPUT); 
	};
   Logger::flush(LogLevel::DEBUG);
};

// Función auxiliar que imprime los valores de la tabla (SE HAN SELECCIONADO COLUMNAS):
void aux_table_values_print(const std::vector<ItemNode>& col_list, std::string table_name) {                                                                                                      
   // Creamos el iterador de filas de la tabla:
   disk_buffer::tableRowIterator it(table_name);
   std::map<std::string, Values> row_map;

   while(!it.is_eof()){
      // consultamos la proxima row_map:
      row_map = it.get_next_row();
      // Antes vemos si está vacía o no:
      if (row_map.empty()) {
         // Escudo contra filas vacías antess de marcar que se ha llegado a EOF
         continue;
      };
      // Ya tenemos la row_map, iteramos por la seleccion de columnas:
      Logger::log(LogLevel::OUTPUT, " | ", false, false);
      for(const ItemNode& element: col_list){
		   auto element_value = row_map.find(element.name);
		   if(element_value == row_map.end()){
		      throw std::runtime_error("Error while showing table data, column: "+ element.name + " does not exist");
		   };
         Logger::log(LogLevel::OUTPUT, element_value->second, false, false);
         Logger::log(LogLevel::OUTPUT, " | ", false, false);                                                                                                                                        
      };                                                                                              
      Logger::flush(LogLevel::OUTPUT);
   };
   Logger::flush(LogLevel::DEBUG);
};

// Función auxiliar para imprimir la tabla: PARA TODAS LAS COLUMNAS:
void print_table(const std::vector<std::string>& col_list, std::string table_name, uint32_t n_cols){
   // Imprimios los nombres de las columnas:
   Logger::flush(LogLevel::OUTPUT);

   for(int  i = 0; i<n_cols; i++){
      Logger::log(LogLevel::OUTPUT, " | ", false); // sin flush automático
      Logger::log(LogLevel::OUTPUT, col_list[i], false);
   };
   Logger::log(LogLevel::OUTPUT, " | ", true);
   // Imprimimos los valores:
   aux_table_values_print(col_list,table_name);
};

// impresión de toda la tabla, pero habiendo seleccionado clumnas
void print_table(const std::vector<ItemNode>& col_list, std::string table_name, uint32_t n_cols){
   // Imprimios los nombres de las columnas:
   Logger::flush(LogLevel::OUTPUT);

   for(int i = 0; i<n_cols; i++){
   Logger::log(LogLevel::OUTPUT, " | ", false); // sin flush automático
   Logger::log(LogLevel::OUTPUT, col_list[i].name, false);
   };
   Logger::log(LogLevel::OUTPUT, " | ", true);
   // Imprimimos los valores:
   aux_table_values_print(col_list, table_name);
};

void show_table_query(QueryNode* root_node_input){

   //recuperamos el nombre de la tabla
   std::string table_name = root_node_input->from_node->name;

   // Before continuing, we make sure the table is already registered in the global table dictionary
   auto it = global_table_dict.find(table_name);
   if(it == global_table_dict.end()){
      Logger::log(LogLevel::ERROR, "Table queried does not exist");
      return;
   };
   table* table_ptr = it->second;

   if (root_node_input->select_node == nullptr) {
      // User has used '*' in selection:
      // Guardamos en memoria valores del diccionario accedidos con regularidad:
      const std::vector<std::string>& col_list = table_ptr->metadata_ptr->column_names;
      uint32_t n_cols = table_ptr->metadata_ptr->n_cols;
      print_table(col_list, table_name, n_cols);
   } else {
      // User has specified columns to retrieve:
      const std::vector<ItemNode>& col_list = root_node_input->select_node->items;
      uint32_t n_cols = col_list.size();
      print_table(col_list, table_name, n_cols);
   };
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Eliminación de tablas:
void table_deletion(table*& tb){
   /*
   Función encargada de eliminar la tabla y sus datos:
   La función se dividirá en:
   1) Eliminación de los archivos en disco, si los hay
   2) Eliminación de los datos en la memoria volátil
   */
   //////////////////////////
   //1) Eliminación de los datos en disco:
   disk_aux::delete_bin_metadata_file(tb);
   disk_aux::delete_bin_data_file(tb);
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
   tb = nullptr;
};


void table_deletion_only_memory(table*& tb){
   /*
   Función encargada de eliminar la tabla y sus datos:
   La función se dividirá en:
      Eliminación de los datos en la memoria volátil
   */
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
   tb = nullptr;
};



void drop_table_from_global_dict(DropTableNode*& node_ptr_input){

   auto it = global_table_dict.find(node_ptr_input->table_name);
   // Ahora vemos si estq vacio o no:
   if(it == global_table_dict.end()){
      // No existe la entrada:
      Logger::log(LogLevel::OUTPUT, "TABLE DOES NOT EXIST, THUS CAN NOT BE DELETED");

      return;
   }else{
      table* table_ptr = it->second;
      global_table_dict.erase(node_ptr_input->table_name);
      table_deletion(table_ptr);
      table_ptr = nullptr;
   };
   
};
// =================================
// == ELIMNAR ENTRADAS NULAS:
// =================================

void sanitize_global_dict() {
   auto it = global_table_dict.begin();
   while (it != global_table_dict.end()) {
      const std::string& name_str = it->first;
      table* ptr = it->second;
      bool delete_bool = false;

      // 1. Validación de Longitud
      if (name_str.length() == 0) {
         Logger::log(LogLevel::WARN, "Sanitizer: zero length name detected.");
         delete_bool = true;
      }
      // 2. Validación de Contenido (¿Es imprimible?)
      else if (!std::isprint(static_cast<unsigned char>(name_str[0]))) {
         Logger::log(LogLevel::WARN, "Sanitizer: non printable character found in table map key.");
         delete_bool = true;
      }
      // 3. Validación de Puntero
      else if (ptr == nullptr) {
         Logger::log(LogLevel::WARN, "Sanitizer: null pointer for table '" + name_str + "'.");
         delete_bool = true;
      }

      if (delete_bool) {
         // Importante: liberar la memoria del contenido si el puntero no es nulo
         // pero la clave está corrupta, para evitar leaks.
         if (ptr != nullptr) {
            table_deletion(ptr);
         }
         it = global_table_dict.erase(it);
      } else {
         ++it;
      }
    }
};



// Function destined to erase all tables, both from memory and disk
void delete_all_tables_dict(){
   auto it = global_table_dict.begin();

   while (it != global_table_dict.end()){
      table* ptr = it->second;
      if(ptr){
         table_deletion(ptr);
      };
      ++it;
   };
   // Libreamos las entradas del diccionario:
   global_table_dict.clear();
};


// Function destined to erase all tables only from disk
void delete_all_tables_dict_only_mem(){
   auto it = global_table_dict.begin();

   while (it != global_table_dict.end()){
      table* ptr = it->second;
      if(ptr){
         table_deletion_only_memory(ptr);
      };
      ++it;
   };
   // Libreamos las entradas del diccionario:
   global_table_dict.clear();
};
