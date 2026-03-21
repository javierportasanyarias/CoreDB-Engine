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
   /*
   Nodo que registra los metadatos de una columna concreta.
   Posee los siguientes atributos:
      -> name_campo: nombre de la columna.
      -> tipo: tipo de datos al que pertenece la columna.
      -> is_primary: booleano indiciador de si la columna es clave primaria (true) o no (false).
      -> hijos: Aunque esta clae podría tener "hijos" de su mismo tipo, en la práctica no se usa.
         (atributo candidato a ser eliminado?)
   */
   public:
      std::string name_campo;
      std::string tipo;
      bool is_primary;
      std::vector<NodeType2*> hijos;

      NodeType2();
};

class NodeType1{
   /*
   Nodo que alamacena los metadatos básicos de un tabla.
   Estos atributos son:
      -> nombre_tabla: nombre de la tabla.
      -> alias: alias de la tabla
         (actualmente en uso, será sustituido por otor sistema de alias?).
      -> hijos: Enlaza con nodos de la clase 'NodeType2'. Estos corresponden
         a cada una de las columnas e la tabla.
      -> tb_struct: la tabla en sí.
   */
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
