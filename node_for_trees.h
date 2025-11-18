#ifndef NODE_FOR_TREES_H
#define NODE_FOR_TREES_H

#include <iostream>
#include <string>
#include <vector>
#include "data_struct.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Nodos de los árboles:
class NodeType2{

   public:
      std::string name_campo;
      std::string tipo;
      bool is_primary;
      std::vector<NodeType2*> hijos;

      NodeType2(): is_primary(false){};
};

class NodeType1{

   public:
      std::string nombre_tabla;
      std::string alias;
      std::vector<NodeType2*> hijos;
      table tb_struct;  

      NodeType1(): alias(""){};
};

//using Valor = std::variant<int, float, std::string>;

class NodeType3 {
    public:
        std::string nombre_tabla;              // Ej: "clientes"
        std::vector<std::string> columnas;     // Ej: ["id", "nombre", "edad"]
        std::vector<std::vector<std::string>> filas;  // Ej: [[1, "Alice", 20], [2, "Bob", 30]]
};
////////////////////////////////////
/// NODOS PARA CONSULTAS:

//class WhereNode {
//};

class FromNode{
   public:
      std::string nombre;
      std::string alias;

      FromNode(): nombre(""), alias(""){};
};

class ItemNode {
   public:
      std::string nombre;
      std::string alias;

      ItemNode(): nombre(""), alias(""){};
};

class SelectNode {
   public:
      std::vector<ItemNode> items;
};

class QueryNode {
   public:
      SelectNode* nodo_select;
      FromNode* nodo_from;

      QueryNode(): nodo_select(nullptr), nodo_from(nullptr){};
};
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

class tree_for_schema{

   public:
      NodeType1*  root;

      tree_for_schema(): root(nullptr){};

      void imprimir_arbol(){
         recursive_tree_print(root);
      };

      //void constrir_arbol(simpleLinkedList lista){

      //   NodeType1* nodo_padre = new NodeType1;
      //   NodeLista1 cabeza = *(lista.head);
      //   nodo_padre = aux_ddl_tree_1(cabeza);
      //   root = nodo_padre;
      //};


};

#endif
