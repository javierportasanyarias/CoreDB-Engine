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
    };
};

// saca del frente y devuelve el nodo (no lo borra)
execPlan::queueNode1* execPlan::Queue::pop_front_node() {
    if (!first_ptr) return nullptr;
    queueNode1* removed = first_ptr;
    if (first_ptr == last_ptr) {
        first_ptr = last_ptr = nullptr;
    } else {
        first_ptr = first_ptr->nxt_node_queue;
        first_ptr->prv_node_queue = nullptr;
    };
    removed->nxt_node_queue = removed->prv_node_queue = nullptr;
    return removed;
};

// Función para imprimir todos los nodos de la cola y su tipo:
void execPlan::Queue::printNodeTypes() const{
    queueNode1* current = first_ptr;
    int index = 0;

    while (current != nullptr) {
        std::cout << "Nodo " << index << ": ";

        // Comprobamos qué tipo de puntero contiene el variant:
        if (std::holds_alternative<NodeType1*>(current->nodePtr)) {
            std::cout << "NodeType1" << std::endl;
            const NodeType1* nodo_puntero = std::get<NodeType1*>(current->nodePtr);
            recursive_tree_print(nodo_puntero);
        } else if (std::holds_alternative<NodeType2*>(current->nodePtr)) {
            std::cout << "NodeType2";
        } else if (std::holds_alternative<NodeType3*>(current->nodePtr)) {
            std::cout << "NodeType3"<<std::endl;
            const NodeType3* nodo_puntero = std::get<NodeType3*>(current->nodePtr);
            insert_data_node_print(nodo_puntero);
        } else if (std::holds_alternative<QueryNode*>(current->nodePtr)) {
            std::cout << "QueryNode"<<std::endl;
        } else {
            std::cout << "Tipo desconocido";
        };

        std::cout << std::endl;
        current = current->nxt_node_queue;
        index++;
    }

    if (index == 0)
        std::cout << "(La cola está vacía)\n";
};

/////////////////////////////////////////////////////////////////////////

// Función sobrecargada auxiliar para eliminar el contenido de los nodos de la cola de ejecución:


void aux_delete_que_node_content(QueryNode* nodo){

    // Este nodo es especial, porque en vez de tener hijos en un vector, cada "hijo" está en una variable concreta y con nombre
    delete nodo->nodo_select;
    nodo->nodo_select = nullptr;
    delete nodo->nodo_from;
    nodo->nodo_from = nullptr;

    // Ya podemos eliminar el nodo:
    delete nodo;
    //nodo = nullptr;
};


void aux_delete_que_node_content(NodeType3* nodo){

    // Este nodo es uno solo, no tiene hijos ni otros nodos enlazadas, tan sólo es una estructura de datos

    // Ya podemos eliminar el nodo:
    delete nodo;
    //nodo = nullptr;
};


void aux_delete_que_node_content(NodeType2* nodo){

    // Acción para recorrer sus hijos:
    
    // No hacemos nada porque suponemos que este tipo de nodos no tienen hijos

    // Ya podemos eliminar el nodo:
    delete nodo;
    //nodo = nullptr;

};


void aux_delete_que_node_content(NodeType1* nodo){

    // Acción para recorrer sus hijos:
    for (auto& nodo_hijo_ptr : nodo->hijos) {
        aux_delete_que_node_content(nodo_hijo_ptr);
    };
    // Ya podemos eliminar el nodo:
    delete nodo;
    nodo = nullptr;
};

////////////////////////////////////////////////////////////////////////
// Función auxiliar para elimimnar nodos de la cola:
void execPlan::Queue::delete_current_queue_node(queueNode1* nodo_cola_a_eliminar) {

    if (!nodo_cola_a_eliminar) return;
    std::visit(
        [](auto* nodo_ptr) {
            aux_delete_que_node_content(nodo_ptr);
            nodo_ptr = nullptr; 
        },
        nodo_cola_a_eliminar->nodePtr
    );
    delete nodo_cola_a_eliminar;
};

////////////////////////////////////////////////////////////////////////
//Función para eliminar la cola entera:
void execPlan::delete_task_queue(execPlan::Queue* cola){
    delete cola->first_ptr;
    cola->first_ptr = nullptr;
    delete cola->last_ptr;
    cola->last_ptr = nullptr;
    delete cola;

};


// Funcion para ejecutar la cola:
//
void execPlan::Queue::execute_queue_tasks(){

    int index = 0;
    execPlan::queueNode1* c_q_n = first_ptr; // Sin crear nada con new

    std::cout << "Nodo " << index << ": ";
    while(c_q_n){
        //NodeVariant nodoVariant = c_q_n->nodePtr;
            if (std::holds_alternative<NodeType1*>(c_q_n->nodePtr)) {
                std::cout << "NodeType1: Ejecutamos creacion de tabla" << std::endl;
        //NodeType1* nodo;
        //nodo = std::get<NodeType1*>(nodoVariant);
        NodeType1* nodo = std::get<NodeType1*>(c_q_n->nodePtr);  // Directo desde el variant
        recursive_metadata_fill_lv1(nodo);
        } else if (std::holds_alternative<NodeType2*>(c_q_n->nodePtr)) {                                                                              
            std::cout << "Nos encontramos al ejecutar un: NodeType2";
        }
            else if (std::holds_alternative<NodeType3*>(c_q_n->nodePtr)) {                                                                              
                std::cout << "NodeType3: Ejecutamos insercion de datos"<<std::endl;

        //NodeType3* nodo;
        //nodo = std::get<NodeType3*>(nodoVariant);
        NodeType3* nodo = std::get<NodeType3*>(c_q_n->nodePtr);  // Directo desde el variant
        fill_table_with_values_v4(nodo);
        } else if(std::holds_alternative<QueryNode*>(c_q_n->nodePtr)) {
        std::cout<<"Ejecutamos la consulta: "<<std::endl;

        //QueryNode* nodo;
        //nodo = std::get<QueryNode*>(nodoVariant);
        QueryNode* nodo = std::get<QueryNode*>(c_q_n->nodePtr);  // Directo desde el variant
        mostrar_tabla_query(nodo);
            } else {
                std::cout << "Tipo desconocido de nodo para operar";
            };
        std::cout<<"Operacion terminada"<<std::endl;
        index++;
        std::cout<<std::endl;
        c_q_n = c_q_n->nxt_node_queue;
        // Ahora eliminamos el nodo que acabamos de ejecutar:
        execPlan::Queue::delete_current_queue_node(c_q_n->prv_node_queue);
        c_q_n->prv_node_queue = nullptr;
    };
    if (index == 0){                                                         
        std::cout << "(La cola está vacía)\n";
    };
    // Una vez terminado debemos ejecutar de nuevo la eliminación del último nodo a parte:
    execPlan::Queue::delete_current_queue_node(c_q_n);
    c_q_n = nullptr;

};