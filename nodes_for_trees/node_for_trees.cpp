#include <iostream>
#include <string>
#include <vector>
// #include "data_structure/data_struct.h"
#include "data_struct.h"
#include "node_for_trees.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Nodos de los árboles:
NodeType2::NodeType2(): is_primary(false){};

NodeType1::NodeType1(): alias(""){};

////////////////////////////////////
/// NODOS PARA CONSULTAS:

FromNode::FromNode(): nombre(""), alias(""){};


ItemNode::ItemNode(): nombre(""), alias(""){};


QueryNode::QueryNode(): nodo_select(nullptr), nodo_from(nullptr){};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Métodos de los árboles:

/////////////////////////////////////////
// Metodos para visualizar árboles:
void recursive_tree_print(NodeType2* nodo_ptr){
   if(!nodo_ptr){
      return;
   };
   if(nodo_ptr->is_primary){
      std::cout<<(nodo_ptr->name_campo)<<" "<<(nodo_ptr->tipo)<<" PRIMARY KEY"<<std::endl;
   }else {
      std::cout<<(nodo_ptr->name_campo)<<" "<<(nodo_ptr->tipo)<<std::endl;
   };
   for(int i = 0; i<(*nodo_ptr).hijos.size(); i++){
      //std::cout<<nodo_ptr->name_campo<<std::endl;
      recursive_tree_print((nodo_ptr->hijos)[i]);
   };
};

void recursive_tree_print(NodeType1* nodo_ptr){
   if(!nodo_ptr){
      return;
   };
   if(nodo_ptr->alias != ""){
      std::cout<<nodo_ptr->nombre_tabla<<" "<<nodo_ptr->alias<<std::endl;
   }else {
       std::cout<<nodo_ptr->nombre_tabla<<std::endl;
   };
   for(int i = 0; i<(*nodo_ptr).hijos.size(); i++){
      //std::cout<<nodo_ptr->nombre_tabla<<std::endl;
      recursive_tree_print((nodo_ptr->hijos)[i]);
   };
};

void insert_data_node_print(NodeType3* nodo){

	std::cout<<"Nombre de la tabla en la que se inserta: ";
	std::cout<<nodo->nombre_tabla<<std::endl;

	// Imprimimos las columnas:
	if((nodo->columnas).size() != 0){
	   std::cout<<"Columnas a insertar: ";
	   for(int i = 0; i<(nodo->columnas).size(); i++){
              std::cout<<(nodo->columnas)[i]<<" ";

	   };
	   std::cout<<std::endl;
	}else{
           std::cout<<"No se han especificado las columnas"<<std::endl;
	};

	// Imprimimos los valores:
	for(int i = 0; i<(nodo->filas).size(); i++){
	   std::vector<std::string> fila_current = (nodo->filas)[i];
           std::cout<<"Valores de la fila "<<i+1<<" : ";
	   for(int j= 0; j<(fila_current).size(); j++){
              std::cout<<fila_current[j]<<" ";
	   };
           std::cout<<std::endl;
	};
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