#ifndef PLAN_EXECUTION_H
#define PLAN_EXECUTION_H

#include <iostream>
#include <vector>
// #include "data_structure/data_struct.h"
#include "data_struct.h"
#include <unordered_map>
// #include "globals/globals.h"
#include "globals.h"
#include <variant>
// #include "nodes_for_trees/node_for_trees.h"
#include "node_for_trees.h"

///////////////////////////////////////////////////////////////////////
///
///
// Tipos de datos admitidos en el esquema:
dataType transform_str_to_datatype(const std::string& input);

////////////////////////////////////////////////////////////////////////////////////////
/// PARA RELLENAR LAS TABLAS:


void fill_table_with_values_v4(NodeType3* nodo_ptr);

////////////////////////////////////////////////////////////////////////////////////////
/// PARA RELLENAR METADATOS:

void recursive_metadata_fill_lv2(NodeType2* nodo_ptr, table* tb_created);

void recursive_metadata_fill_lv1(NodeType1* nodo_ptr);


///////////////////////////////////////////////////////////////
// MOSTRAR TABLA:

void mostrar_consulta_v1(QueryNode*& nodo_root);


void mostrar_tabla_query(QueryNode* nodo_root);

////////////////////////////////////////////////////////////////
// ELIMINACIÓN:
void liberar_tabla(table*& tb);

void drop_table_from_global_dict(DropTableNode*& nodo_ptr);

void sanitize_global_dict();

#endif