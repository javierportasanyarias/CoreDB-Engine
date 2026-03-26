#ifndef NODE_FOR_TREES_H
#define NODE_FOR_TREES_H

#include <iostream>
#include <string>
#include <vector>
// #include "data_structure/data_struct.h"
#include "data_struct.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Todos estos elementos sirven para dotar al programa de una forma más sencilla de entender
las instrucciones que debe realizar. Si pusiéramos los 'tokens' directamente venidos de
procesar el input sería demasiado complicado y confuso ejecutar las instrucciones que ahí
se exponen. Para ello totammos otro enfoque: cada elemento de la cola de ejecución tendrá
en su contenido los nodos recogidos en este documento. Estos aportan una forma mucho más
clara para poder ser ejecutados. Con mirar sus atributos o recorrerlos en caso de los
árboles, podemos saber perfectamente como definir los metadatos de la tabla y cómo rellenarla.
*/


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


class NodeType3 {
   /*
   Nodo reservado para la inserción de datos.
   Este posee los siguientes atributos:
      -> nombre_tabla: nombre de la tabla sobre la que se desea insertar los valores.
      -> columnas: columnas sobre las cuales realizar la inserción.
      -> Un vector de los valores a insertar. Es una lista de listas realmente, donde el
         número de "sublistas" es igual al número de filas a insertar.
   Esta clae permite insertar una o más filas en la misma instrucción.
   */
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
   /*
   Clase que hace referencia al nombre de la tabla a la que apunta la instrucción 'SELECT'.
   */
   public:
      std::string nombre;
      std::string alias;

      FromNode();
};


class ItemNode {
   /*
   Clase que almacena cada campo seleccionado y su alias.
   */
   public:
      std::string nombre;
      std::string alias;
      ItemNode();
};


class SelectNode {
   /*
   Clase que guarda los campos seleccionados, cada uno de estos como un objeto de la clase 'ItemNode'.
   */
   public:
      std::vector<ItemNode> items;
};


class QueryNode {
   /*
   Clase que engloba todo lo necesario para la consulta.
   En concreto, alberga un árbol de consulta, osea un árbol que
   posteriormente se recorrerá para ejecutar la consulta.
   */
   public:
      SelectNode* nodo_select;
      FromNode* nodo_from;
      QueryNode();
};

////////////////////////////////////
/// NODOS PARA ELIMINAR TABLAS:

class DropTableNode {
   /*
   Clase destinada a ser el contenido de una tarea que elimine la tabla en cuestión.
   */
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

   /*
   Clase que estructura los elementos necesarios para crear el esquema, o al menos la instrucción para ello.
   Al recorrer este árbol se tienen todos los elementos necesarios para definir el esquema de la tabla.
   ATENCIÓN:
   Actualmente en desuso, se usa NodeType1
   */
   public:
      NodeType1*  root;
      tree_for_schema();
      void imprimir_arbol();

};

#endif
