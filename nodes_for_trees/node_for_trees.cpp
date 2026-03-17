#include <iostream>
#include <string>
#include <vector>
// #include "data_structure/data_struct.h"
#include "data_struct.h"
#include "node_for_trees.h"
#include "logging.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Nodos de los árboles:
NodeType2::NodeType2(): is_primary(false){};

NodeType1::NodeType1(): alias(""){};

////////////////////////////////////
/// NODOS PARA CONSULTAS:

FromNode::FromNode(): nombre(""), alias(""){};


ItemNode::ItemNode(): nombre(""), alias(""){};


QueryNode::QueryNode(): nodo_select(nullptr), nodo_from(nullptr){};

DropTableNode::DropTableNode(): nombre_tabla(""){};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Métodos de los árboles:

/////////////////////////////////////////
// Metodos para visualizar árboles:
void recursive_tree_print(const NodeType2* nodo_ptr){
   if(!nodo_ptr){
      return;
   };
   if(nodo_ptr->is_primary){
      // std::cout<<(nodo_ptr->name_campo)<<" "<<(nodo_ptr->tipo)<<" PRIMARY KEY"<<std::endl;
      Logger::log(LogLevel::DEBUG, (nodo_ptr->name_campo), false, false);
      Logger::log(LogLevel::DEBUG, " ", false, false);
      Logger::log(LogLevel::DEBUG, (nodo_ptr->tipo), false, false);
      Logger::log(LogLevel::DEBUG, " PRIMARY KEY", true, false);
   }else {
      // std::cout<<(nodo_ptr->name_campo)<<" "<<(nodo_ptr->tipo)<<std::endl;
      Logger::log(LogLevel::DEBUG, (nodo_ptr->name_campo), false, false);
      Logger::log(LogLevel::DEBUG, " ", false, false);
      Logger::log(LogLevel::DEBUG, (nodo_ptr->tipo), true, false);
   };
   for (auto* hijo : nodo_ptr->hijos) {
      recursive_tree_print(hijo);
   };
};

void recursive_tree_print(const NodeType1* nodo_ptr){
   if(!nodo_ptr){
      return;
   };
   if(nodo_ptr->alias != ""){
      // std::cout<<nodo_ptr->nombre_tabla<<" "<<nodo_ptr->alias<<std::endl;
      Logger::log(LogLevel::DEBUG, (nodo_ptr->nombre_tabla), false, false);
      Logger::log(LogLevel::DEBUG, " ", false, false);
      Logger::log(LogLevel::DEBUG, (nodo_ptr->alias), true, false);
   }else {
      // std::cout<<nodo_ptr->nombre_tabla<<std::endl;
      Logger::log(LogLevel::DEBUG, (nodo_ptr->nombre_tabla), true, false);
   };
   for (auto* hijo : nodo_ptr->hijos) {
      recursive_tree_print(hijo);
   };
};

// Funciones auxiliares a la impresión de columnas:
void imprimir_columnas(const NodeType3*& nodo){
	if((nodo->columnas).size() != 0){
	   // std::cout<<"Columnas a insertar: ";
      Logger::log(LogLevel::DEBUG, "Columnas a insertar: ", false, false);
	   for(int i = 0; i<(nodo->columnas).size(); i++){
         // std::cout<<(nodo->columnas)[i]<<" ";
         Logger::log(LogLevel::DEBUG, (nodo->columnas)[i], false, false);
         Logger::log(LogLevel::DEBUG, " ", false, false);
	   };
	   // std::cout<<std::endl;
      Logger::flush();
	}else{
      // std::cout<<"No se han especificado las columnas"<<std::endl;
      Logger::log(LogLevel::DEBUG, "No se han especificado las columnas", true, false);
	};
};

void imptimir_valores(const NodeType3*& nodo){

	for(int i = 0; i<(nodo->filas).size(); i++){
	   const std::vector<std::string>& fila_current = (nodo->filas)[i];
      // std::cout<<"Valores de la fila "<<i+1<<" : ";
      Logger::log(LogLevel::DEBUG, "Valores de la fila ", false, false);
      Logger::log(LogLevel::DEBUG, i+1, false, false);
      Logger::log(LogLevel::DEBUG, " : ", false, false);
         for(int j= 0; j<(fila_current).size(); j++){
            // std::cout<<fila_current[j]<<" ";
            Logger::log(LogLevel::DEBUG, fila_current[j], false, false);
            Logger::log(LogLevel::DEBUG, " ", false, false);
         };
      // std::cout<<std::endl;
      Logger::flush();
	};
};

// Esta función es puramente auxiliar:
void insert_data_node_print(const NodeType3* nodo){

	// std::cout<<"Nombre de la tabla en la que se inserta: ";
   Logger::log(LogLevel::DEBUG, "Nombre de la tabla en la que se inserta: ", false, false);
	// std::cout<<nodo->nombre_tabla<<std::endl;
   Logger::log(LogLevel::DEBUG, nodo->nombre_tabla, true, false);

	// Imprimimos las columnas:
	imprimir_columnas(nodo);

	// Imprimimos los valores:
	imptimir_valores(nodo);
};


/////////////////////////////////////////
// Métodos para construir árboles:

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Árboles:

tree_for_schema::tree_for_schema() : root(nullptr) {}

// Método imprimir_arbol
void tree_for_schema::imprimir_arbol() {
   recursive_tree_print(root);
};
