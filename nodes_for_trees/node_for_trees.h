#ifndef NODE_FOR_TREES_H
#define NODE_FOR_TREES_H

#include <iostream>
#include <string>
#include <vector>
// #include "data_structure/data_struct.h"
#include "data_struct.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Nodos de los árboles:
class NodeType2{

   public:
      std::string name_campo;
      std::string tipo;
      bool is_primary;
      std::vector<NodeType2*> hijos;

      NodeType2();
};

class NodeType1{

   public:
      std::string nombre_tabla;
      std::string alias;
      std::vector<NodeType2*> hijos;
      table tb_struct;  

      NodeType1();
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

      FromNode();
};

class ItemNode {
   public:
      std::string nombre;
      std::string alias;

      ItemNode();
};

class SelectNode {
   public:
      std::vector<ItemNode> items;
};

class QueryNode {
   public:
      SelectNode* nodo_select;
      FromNode* nodo_from;

      QueryNode();
};
////////////////////////////////////
/// NODOS PARA ELIMINAR TABLAS:
class DropTableNode {
   public:
      std::string nombre_tabla;

      DropTableNode();
      
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Métodos de los árboles:

/////////////////////////////////////////
// Metodos para visualizar árboles:
void recursive_tree_print(const NodeType2* nodo_ptr);

void recursive_tree_print(const NodeType1* nodo_ptr);

void insert_data_node_print(const NodeType3* nodo);


/////////////////////////////////////////
// Métodos para construir árboles:

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Árboles:

class tree_for_schema{

   public:
      NodeType1*  root;

      tree_for_schema();

      void imprimir_arbol();

};

#endif
