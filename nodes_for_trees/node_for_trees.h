#pragma once

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
All these elements serve to provide the program with a simpler way to understand
the instructions it must execute. If we were to use the 'tokens' directly obtained
from processing the input, it would be too complicated and confusing to execute
the instructions there. For this, we adopt another approach: each element in the
execution queue will contain the nodes collected in this document. These provide a
much clearer way to be executed. By looking at their attributes or traversing them
in the case of trees, we can perfectly define the table metadata and how to fill it.
*/


// Tree nodes:
class NodeType2{
   /*
   Node that records the metadata of a specific column.
   It has the following attributes:
      -> field_name: column name.
      -> type: data type of the column.
      -> is_primary: boolean indicating whether the column is a primary key (true) or not (false).
      -> children: Although this class could have "children" of the same type, it is not used in practice.
         (candidate attribute to be removed?)
   */
   public:
      std::string field_name;
      std::string type;
      bool is_primary;
      std::vector<NodeType2*> children;

      NodeType2();
};


class NodeType1{
   /*
   Node that stores the basic metadata of a table.
   These attributes are:
      -> table_name: table name.
      -> alias: table alias
         (currently in use, will be replaced by another alias system?).
      -> children: Links to nodes of the class 'NodeType2'. These correspond
         to each of the table's columns.
      -> tb_struct: the table itself.
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
   Node reserved for data insertion.
   It has the following attributes:
      -> table_name: name of the table on which values are to be inserted.
      -> columns: columns on which to perform the insertion.
      -> A vector of values to insert. It is actually a list of lists, where the
         number of "sublists" is equal to the number of rows to insert.
   This class allows inserting one or more rows in a single statement.
   */
   public:
      std::string table_name;              // Ex: "customers"
      std::vector<std::string> columns;     // Ex: ["id", "name", "age"]
      std::vector<std::vector<std::string>> rows;  // Ex: [["1", "Alice", "20"], ["2", "Bob", "30"]]
};
////////////////////////////////////
/// QUERY NODES:

//class WhereNode {
//};

class FromNode{
   /*
   Class that refers to the table name pointed to by the 'SELECT' instruction.
   */
   public:
      std::string name;
      std::string alias;

      FromNode();
};


class ItemNode {
   /*
   Class that stores each selected field and its alias.
   */
   public:
      std::string name;
      std::string alias;
      ItemNode();
};


class SelectNode {
   /*
   Class that holds the selected fields, each as an object of the class 'ItemNode'.
   */
   public:
      std::vector<ItemNode> items;
};


class QueryNode {
   /*
   Class that encompasses everything needed for the query.
   Specifically, it holds a query tree, i.e., a tree that
   will be traversed later to execute the query.
   */
   public:
      SelectNode* select_node;
      FromNode* from_node;
      QueryNode();
};

////////////////////////////////////
/// TABLE DELETION NODES:

class DropTableNode {
   /*
   Class intended to be the content of a task that deletes the specified table.
   */
   public:
      std::string table_name;
      DropTableNode();
      
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tree methods:

/////////////////////////////////////////
// Methods for visualizing trees:
void recursive_tree_print(const NodeType2* nodo_ptr);

void recursive_tree_print(const NodeType1* nodo_ptr);

void insert_data_node_print(const NodeType3* nodo);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////