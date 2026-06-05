#ifndef EXECUTION_PLANNING_H
#define EXECUTION_PLANNING_H

#include <iostream>
#include <variant>
// #include "nodes_for_trees/node_for_trees.h"
#include "node_for_trees.h"
// #include "globals/globals.h"
#include "globals.h"
// #include "execution/execution.h"
#include "execution.h"

namespace execPlan {
    ///////////////////////////////////////////////////////////
    // Cola de ejecución FIFO: determinará las tareas a ejecutar en cada consulta
    using NodeVariant = std::variant<NodeType1*, NodeType2*, NodeType3*, QueryNode*, DropTableNode*>;
    class queueNode1 {
        public:
            NodeVariant nodePtr;
            // Proximo nodo de la cola:
            queueNode1* nxt_node_queue;
            queueNode1* prv_node_queue;
            queueNode1();
    };

    // en el header
    class Queue {
        public:
            queueNode1* first_ptr;
            queueNode1* last_ptr;

            Queue();

            void add_node_to_queue(queueNode1* node_queue_to_add);

            // saca del frente y devuelve el nodo (no lo borra)
            queueNode1* pop_front_node();

            // Función para imprimir todos los nodos de la cola y su tipo:
            void printNodeTypes() const;


            // Funcion para ejecutar la cola:
            void execute_queue_tasks();

            // Función para eliminar nodos de la cola:
            void delete_current_queue_node(queueNode1* nodo_cola_a_eliminar);
    };

    void delete_queue_node(queueNode1* nodo_cola_a_eliminar);
    // Método a parte de la cola para eliminarla:
    void delete_task_queue(Queue* cola);

    // Method for deleting not only the object, but all it contains:
    void delete_whole_task_queue(Queue* cola);
};


# endif
