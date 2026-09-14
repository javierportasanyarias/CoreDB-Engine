#include "node_for_trees.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Nodos de los árboles:
NodeType2::NodeType2(): is_primary(false){};

NodeType1::NodeType1(): alias(""){};

////////////////////////////////////
/// NODOS PARA CONSULTAS:

FromNode::FromNode(): name(""), alias(""){};


ItemNode::ItemNode(): name(""), alias(""){};


QueryNode::QueryNode(): select_node(nullptr), from_node(nullptr){};

DropTableNode::DropTableNode(): table_name(""){};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Métodos de los árboles:

/////////////////////////////////////////
// Metodos para visualizar árboles:
void recursive_tree_print(const NodeType2* node_ptr){
   if(!node_ptr){
      return;
   };
   if(node_ptr->is_primary){
      Logger::log(LogLevel::DEBUG, (node_ptr->filed_name), false, false);
      Logger::log(LogLevel::DEBUG, " ", false, false);
      Logger::log(LogLevel::DEBUG, (node_ptr->type), false, false);
      Logger::log(LogLevel::DEBUG, " PRIMARY KEY", true, false);
   }else {
      Logger::log(LogLevel::DEBUG, (node_ptr->filed_name), false, false);
      Logger::log(LogLevel::DEBUG, " ", false, false);
      Logger::log(LogLevel::DEBUG, (node_ptr->type), true, false);
   };
   for (auto* child : node_ptr->children) {
      recursive_tree_print(child);
   };
};

void recursive_tree_print(const NodeType1* node_ptr){
   if(!node_ptr){
      return;
   };
   if(node_ptr->alias != ""){
      Logger::log(LogLevel::DEBUG, (node_ptr->table_name), false, false);
      Logger::log(LogLevel::DEBUG, " ", false, false);
      Logger::log(LogLevel::DEBUG, (node_ptr->alias), true, false);
   }else {
      Logger::log(LogLevel::DEBUG, (node_ptr->table_name), true, false);
   };
   for (auto* child: node_ptr->children) {
      recursive_tree_print(child);
   };
};

// Funciones auxiliares a la impresión de columnas:
void print_columns(const NodeType3*& node_ptr){
	if((node_ptr->columns).size() != 0){
      Logger::log(LogLevel::DEBUG, "Columns to insert: ", false, false);
	   for(int i = 0; i<(node_ptr->columns).size(); i++){
         Logger::log(LogLevel::DEBUG, (node_ptr->columns)[i], false, false);
         Logger::log(LogLevel::DEBUG, " ", false, false);
	   };
      Logger::flush(LogLevel::DEBUG);
	}else{
      Logger::log(LogLevel::DEBUG, "Columns have not been specified", true, false);
	};
};

void print_values(const NodeType3*& node_ptr){

	for(int i = 0; i<(node_ptr->rows).size(); i++){
	   const std::vector<std::string>& current_row = (node_ptr->rows)[i];
      Logger::log(LogLevel::DEBUG, "Row value ", false, false);
      Logger::log(LogLevel::DEBUG, i+1, false, false);
      Logger::log(LogLevel::DEBUG, " : ", false, false);
         for(int j= 0; j<(current_row).size(); j++){
            Logger::log(LogLevel::DEBUG, current_row[j], false, false);
            Logger::log(LogLevel::DEBUG, " ", false, false);
         };
      Logger::flush(LogLevel::DEBUG);
	};
};

// Esta función es puramente auxiliar:
void insert_data_node_print(const NodeType3* node_ptr){

   Logger::log(LogLevel::DEBUG, "Table name where it has been inserted: ", false, false);
   Logger::log(LogLevel::DEBUG, node_ptr->table_name, true, false);

	// Imprimimos las columnas:
	print_columns(node_ptr);

	// Imprimimos los valores:
	print_values(node_ptr);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
