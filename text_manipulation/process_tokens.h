#pragma once

#include "text_utils.h"
#include "execution_planning.h"
#include "node_for_trees.h"
#include "execution.h"

// Forward declarations:
class NodeType1;
class NodeType2;
class NodeType3;
class QueryNode;
namespace execPlan {
class Queue;
};
namespace textUtils {
class NodeLista1;
class simpleLinkedList;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Auxiliar class for keeping track of the tables created o already present:
class tableCatalog {
    private:
        // Default constructor:
        tableCatalog() = default;
    public:
        // Attribute:
        inline static std::unordered_set<std::string> table_catalog;

        //Method for inserting a table:
        static void register_table(std::string name);

        // Method for checking if a table is already set:
        static bool ckeck_table(std::string name);

        // Table deletion:
        static void delete_table(std::string name);
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DEFINIMOS LAS FUNCIONES AUXILIARES:


NodeType2* aux_ddl_tree_2(textUtils::NodeLista1*& c_l_n);

// FUNCIONES PARA DEFINIR EL ESQUEMA:
void crear_hijos_esquema_dado_padre(textUtils::NodeLista1*& c_l_n, NodeType1* nodo, execPlan::Queue* excec_queue);


void procesar_lista_para_definir_esquema(execPlan::Queue* excec_queue, textUtils::NodeLista1*& c_l_n);


void insert_values_add_columns_to_node(NodeType3* nodo, textUtils::NodeLista1*& c_l_n);


void aux_iterative_value_filler(textUtils::NodeLista1*& c_l_n, std::vector<std::string>& vector_fila);

void insert_row_values_in_node(NodeType3* nodo, textUtils::NodeLista1*& c_l_n, execPlan::Queue* excec_queue);

// FUNCIONES PARA INSERTAR VALORES:
void procesar_lista_para_insertar_valores(execPlan::Queue* excec_queue, textUtils::NodeLista1*& c_l_n);

void aux_col_consulta(QueryNode*& nodo_consulta, textUtils::NodeLista1*& c_l_n);

// PARA DEFINIR LAS CONSULTAS:
void procesar_lista_para_consulta(execPlan::Queue*& excec_queue, textUtils::NodeLista1*& c_l_n);

// PARA ELIMINAR TABLAS:
void drop_table(execPlan::Queue*& excec_queue, textUtils::NodeLista1*& table_nombre_ptr);

////////////////////////////////////////////////////////////////////////////////////////////////////////////
execPlan::Queue* procesar_lista_tokens(textUtils::simpleLinkedList*& lista);
