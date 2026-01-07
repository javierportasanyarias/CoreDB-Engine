#ifndef PROCESS_TOKENS_H
#define PROCESS_TOKENS_H


#include <iostream>
#include <string>
#include <vector>
#include "text_utils.h"
// #include "planning_execution/execution_planning.h"
#include "execution_planning.h"
// #include "nodes_for_trees/node_for_trees.h"
#include "node_for_trees.h"
#include <any>
// #include "data_structure/data_struct.h"
#include "data_struct.h"
// #include "globals/globals.h"
#include "globals.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DEFINIMOS LAS FUNCIONES AUXILIARES:


NodeType2* aux_ddl_tree_2(textUtils::NodeLista1& c_l_n);

// FUNCIONES PARA DEFINIR EL ESQUEMA:
void crear_hijos_esquema_dado_padre(textUtils::NodeLista1& c_l_n, NodeType1* nodo);


void procesar_lista_para_definir_esquema(execPlan::Queue* excec_queue, textUtils::NodeLista1& c_l_n);


void insert_values_add_columns_to_node(NodeType3* nodo, textUtils::NodeLista1& c_l_n);


void aux_iterative_value_filler(textUtils::NodeLista1& c_l_n, std::vector<std::string>& vector_fila);

void insert_row_values_in_node(NodeType3* nodo, textUtils::NodeLista1& c_l_n);

// FUNCIONES PARA INSERTAR VALORES:
void procesar_lista_para_insertar_valores(execPlan::Queue* excec_queue, textUtils::NodeLista1& c_l_n);

void aux_col_consulta(QueryNode*& nodo_consulta, textUtils::NodeLista1& c_l_n);

// PARA DEFINIR LAS CONSULTAS:
void procesar_lista_para_consulta(execPlan::Queue* excec_queue, textUtils::NodeLista1& c_l_n);

////////////////////////////////////////////////////////////////////////////////////////////////////////////
execPlan::Queue* procesar_lista_tokens(textUtils::simpleLinkedList& lista);



#endif
