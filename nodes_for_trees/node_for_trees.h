#pragma once


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
      -> filed_name: nombre de la columna.
      -> type: type de datos al que pertenece la columna.
      -> is_primary: booleano indiciador de si la columna es clave primaria (true) o no (false).
      -> children: Aunque esta clae podría tener "children" de su mismo type, en la práctica no se usa.
         (atributo candidato a ser eliminado?)
   */
   public:
      std::string filed_name;
      std::string type;
      bool is_primary;
      std::vector<NodeType2*> children;

      NodeType2();
};


class NodeType1{
   /*
   Nodo que alamacena los metadatos básicos de un tabla.
   Estos atributos son:
      -> table_name: nombre de la tabla.
      -> alias: alias de la tabla
         (actualmente en uso, será sustituido por otor sistema de alias?).
      -> children: Enlaza con nodos de la clase 'NodeType2'. Estos corresponden
         a cada una de las columnas e la tabla.
      -> tb_struct: la tabla en sí.
   */
   public:
      std::string table_name;
      std::string alias;
      std::vector<NodeType2*> children;
      table tb_struct;  

      NodeType1();
};


class NodeType3 {
   /*
   Nodo reservado para la inserción de datos.
   Este posee los siguientes atributos:
      -> table_name: nombre de la tabla sobre la que se desea insertar los valores.
      -> columns: columns sobre las cuales realizar la inserción.
      -> Un vector de los valores a insertar. Es una lista de listas realmente, donde el
         número de "sublistas" es igual al número de filas a insertar.
   Esta clae permite insertar una o más filas en la misma instrucción.
   */
   public:
      std::string table_name;              // Ej: "clientes"
      std::vector<std::string> columns;     // Ej: ["id", "nombre", "edad"]
      std::vector<std::vector<std::string>> rows;  // Ej: [[1, "Alice", 20], [2, "Bob", 30]]
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
      std::string name;
      std::string alias;

      FromNode();
};


class ItemNode {
   /*
   Clase que almacena cada campo seleccionado y su alias.
   */
   public:
      std::string name;
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
      SelectNode* select_node;
      FromNode* from_node;
      QueryNode();
};

////////////////////////////////////
/// NODOS PARA ELIMINAR TABLAS:

class DropTableNode {
   /*
   Clase destinada a ser el contenido de una tarea que elimine la tabla en cuestión.
   */
   public:
      std::string table_name;
      DropTableNode();
      
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Métodos de los árboles:

/////////////////////////////////////////
// Metodos para visualizar árboles:
void recursive_tree_print(const NodeType2* nodo_ptr);

void recursive_tree_print(const NodeType1* nodo_ptr);

void insert_data_node_print(const NodeType3* nodo);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
