#pragma once

#include "execution.h"
#include "execution_planning.h"
#include "node_for_trees.h"
#include "text_utils.h"

// Forward declarations:
class NodeType1;
class NodeType2;
class NodeType3;
class QueryNode;
namespace execPlan {
class Queue;
};
namespace textUtils {
class NodeList1;
class simpleLinkedList;
};  // namespace textUtils

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Auxiliar class for keeping track of the tables created o already present:
class tableCatalog {
 private:
  // Default constructor:
  tableCatalog() = default;

 public:
  // Attribute:
  inline static std::unordered_set<std::string> table_catalog;

  // Method for inserting a table:
  static void register_table(std::string name);

  // Method for checking if a table is already set:
  static bool ckeck_table(std::string& name);

  // Table deletion:
  static void delete_table(std::string name);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AUXILIARY FUNCTIONS:

NodeType2* aux_ddl_tree_2(textUtils::NodeList1*& c_l_n);

// FUNCTIONS FOR SCHEMA DEFINITION:
void create_children_by_parent(textUtils::NodeList1*& c_l_n,
                               NodeType1*& node_ptr,
                               execPlan::Queue* excec_queue);

void process_list_to_define_schema(execPlan::Queue* excec_queue,
                                   textUtils::NodeList1*& c_l_n);

void insert_values_add_columns_to_node(NodeType3* node_ptr,
                                       textUtils::NodeList1*& c_l_n);

void aux_iterative_value_filler(textUtils::NodeList1*& c_l_n,
                                std::vector<std::string>& vector_fila);

void insert_row_values_in_node(NodeType3*& node_ptr,
                               textUtils::NodeList1*& c_l_n,
                               execPlan::Queue* excec_queue);

// FUNCTIONS FOR VALUE INSERTION:
void process_list_to_insert_values(execPlan::Queue* excec_queue,
                                   textUtils::NodeList1*& c_l_n);

void aux_col_query(QueryNode*& query_node, textUtils::NodeList1*& c_l_n);

// FUNCTIONS FOR QUERIES:
void process_list_for_selection(execPlan::Queue*& excec_queue,
                                textUtils::NodeList1*& c_l_n);

////////////////////////////////////////////////////////////////////////////////////////////////////////////
execPlan::Queue* process_token_list(textUtils::simpleLinkedList*& list_ptr);
