#include <iostream>
#include <variant>
// #include "nodes_for_trees/node_for_trees.h"
#include "node_for_trees.h"
// #include "globals/globals.h"
#include "globals.h"
// #include "execution/execution.h"
#include "execution.h"
#include "execution_planning.h"

///////////////////////////////////////////////////////////
// Cola de ejecución FIFO: determinará las tareas a ejecutar en cada consulta
using NodeVariant = std::variant<NodeType1*, NodeType2*, NodeType3*, QueryNode*>;

execPlan::queueNode1::queueNode1(): nxt_node_queue(nullptr), prv_node_queue(nullptr){};

// en el header
execPlan::Queue::Queue() : first_ptr(nullptr), last_ptr(nullptr) {};

void execPlan::Queue::add_node_to_queue(queueNode1* node_queue_to_add) {
    if (!node_queue_to_add) return;
    node_queue_to_add->nxt_node_queue = nullptr;
    node_queue_to_add->prv_node_queue = nullptr;

    if (!first_ptr) {
        // cola vacía
        first_ptr = last_ptr = node_queue_to_add;
    } else {
        // enlazar al final
        last_ptr->nxt_node_queue = node_queue_to_add;
        node_queue_to_add->prv_node_queue = last_ptr;
        last_ptr = node_queue_to_add;
    }
}

// saca del frente y devuelve el nodo (no lo borra)
execPlan::queueNode1* execPlan::Queue::pop_front_node() {
    if (!first_ptr) return nullptr;
    queueNode1* removed = first_ptr;
    if (first_ptr == last_ptr) {
        first_ptr = last_ptr = nullptr;
    } else {
        first_ptr = first_ptr->nxt_node_queue;
        first_ptr->prv_node_queue = nullptr;
    }
    removed->nxt_node_queue = removed->prv_node_queue = nullptr;
    return removed;
}

// Función para imprimir todos los nodos de la cola y su tipo:
void execPlan::Queue::printNodeTypes() {
    queueNode1* current = first_ptr;
    int index = 0;

    while (current != nullptr) {
        std::cout << "Nodo " << index << ": ";

        // Comprobamos qué tipo de puntero contiene el variant:
        if (std::holds_alternative<NodeType1*>(current->nodePtr)) {
            std::cout << "NodeType1" << std::endl;
recursive_tree_print(std::get<NodeType1*>(current->nodePtr));
        } else if (std::holds_alternative<NodeType2*>(current->nodePtr)) {
            std::cout << "NodeType2";
        } else if (std::holds_alternative<NodeType3*>(current->nodePtr)) {
            std::cout << "NodeType3"<<std::endl;
    insert_data_node_print(std::get<NodeType3*>(current->nodePtr));
    } else if (std::holds_alternative<QueryNode*>(current->nodePtr)) {
            std::cout << "QueryNode"<<std::endl;
        } else {
            std::cout << "Tipo desconocido";
        }

        std::cout << std::endl;
        current = current->nxt_node_queue;
        index++;
    }

    if (index == 0)
        std::cout << "(La cola está vacía)\n";
};


// Funcion para ejecutar la cola:
//
void execPlan::Queue::execute_queue_tasks(){
    int index = 0;
        execPlan::queueNode1* c_q_n = new execPlan::queueNode1;
    c_q_n = first_ptr;
    std::cout << "Nodo " << index << ": ";
    while(c_q_n){
        NodeVariant nodoVariant = c_q_n->nodePtr;
            if (std::holds_alternative<NodeType1*>(c_q_n->nodePtr)) {
                std::cout << "NodeType1: Ejecutamos creacion de tabla" << std::endl;
        NodeType1* nodo;
        nodo = std::get<NodeType1*>(nodoVariant);
        recursive_metadata_fill_lv1(nodo);
        } else if (std::holds_alternative<NodeType2*>(c_q_n->nodePtr)) {                                                                              
            std::cout << "Nos encontramos al ejecutar un: NodeType2";
        }
            else if (std::holds_alternative<NodeType3*>(c_q_n->nodePtr)) {                                                                              
                std::cout << "NodeType3: Ejecutamos insercion de datos"<<std::endl;

        NodeType3* nodo;
                nodo = std::get<NodeType3*>(nodoVariant);
        fill_table_with_values_v4(nodo);
        } else if(std::holds_alternative<QueryNode*>(c_q_n->nodePtr)) {
        std::cout<<"Ejecutamos la consulta: "<<std::endl;

        QueryNode* nodo;
        nodo = std::get<QueryNode*>(nodoVariant);
        mostrar_consulta_v2(nodo);
            } else {
                std::cout << "Tipo desconocido de nodo para operar";
            };
        std::cout<<"Operacion terminada"<<std::endl;
        index++;
        std::cout<<std::endl;
            c_q_n = c_q_n->nxt_node_queue;
    };
    if (index == 0){                                                         
        std::cout << "(La cola está vacía)\n";
    };
};