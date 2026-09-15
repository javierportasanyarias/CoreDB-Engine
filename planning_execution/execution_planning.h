#pragma once

#include "node_for_trees.h"
#include "execution.h"

// Forward declarations:
class DropTableNode;
class NodeType1;
class NodeType2;
class NodeType3;
class QueryNode;


namespace execPlan {
    ///////////////////////////////////////////////////////////
    //  FIFO execution queue: it will determine the order of execution tasks.

    // Variant type of variable holding all the task's nodes:
    using NodeVariant = std::variant<NodeType1*, NodeType2*, NodeType3*, QueryNode*, DropTableNode*>;

    class queueNode1 {
        public:
            NodeVariant nodePtr;
            queueNode1* nxt_node_queue;
            queueNode1* prv_node_queue;
            queueNode1();
    };


    class Queue {
        /*
        FIFO queue determines the execution tasks's order
        */
        public:
            queueNode1* first_ptr;
            queueNode1* last_ptr;

            Queue();

            void add_node_to_queue(queueNode1* node_queue_to_add);

            // Method for reasigning the last queue node (It does not delete it):
            queueNode1* pop_front_node();

            // Method for printing all the queue node and types:
            void printNodeTypes() const;


            // Task execution method:
            void execute_queue_tasks();

            // Method for deleting a queue's node.
            void delete_current_queue_node(queueNode1* queue_node_to_del);
    };

    void delete_queue_node(queueNode1* queue_node_to_del);


    void delete_task_queue(Queue* queue);

    // Method for deleting not only the object, but all it contains:
    void delete_whole_task_queue(Queue* queue);
};
