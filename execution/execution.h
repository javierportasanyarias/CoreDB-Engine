#pragma once

#include "node_for_trees.h"

// Forward declarations:
class DropTableNode;
class NodeType1;
class NodeType2;
class NodeType3;
class QueryNode;


///////////////////////////////////////////////////////////////////////

// Tipos de datos admitidos en el esquema:
dataType transform_str_to_datatype(const std::string& input);

////////////////////////////////////////////////////////////////////////////////////////
/// PARA RELLENAR LAS TABLAS:


void fill_table_with_values_v4(NodeType3* node_ptr_input);

////////////////////////////////////////////////////////////////////////////////////////
/// PARA RELLENAR METADATOS:

void recursive_metadata_fill_lv2(NodeType2* node_ptr_input, table* tb_created);

void recursive_metadata_fill_lv1(NodeType1* node_ptr_input);


///////////////////////////////////////////////////////////////
// MOSTRAR TABLA:

void show_table_query(QueryNode* root_node_input);

////////////////////////////////////////////////////////////////
// ELIMINACIÓN:
void table_deletion(table*& tb);

void table_deletion_only_memory(table*& tb);

void drop_table_from_global_dict(DropTableNode*& node_ptr_input);

void sanitize_global_dict();

void delete_all_tables_dict();

void delete_all_tables_dict_only_mem();
