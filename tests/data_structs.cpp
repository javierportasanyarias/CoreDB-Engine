#include "data_structs.h"


//============================
//== NODES: ==================
//============================

// Construimos el nodo:
FifoNode::FifoNode(): comando(""), nxt_node(nullptr){};

//============================
//== QUEUEU: ===================
//============================

// Constructing queue:
FIFO::FIFO(): head(nullptr){};

// Auxiliar method for recursive node deletion:
void delete_fifo_node_recursive_helper(FifoNode*& c_n_ptr){

   while(c_n_ptr->nxt_node){
      delete_fifo_node_recursive_helper(c_n_ptr->nxt_node);
   };

   delete c_n_ptr;
   c_n_ptr = nullptr;
};

// For deleting all the queue's nodes:
void FIFO::delete_fifo_content(FifoNode*& head){

   delete_fifo_node_recursive_helper(head); 

};

// For deleting FIFO queue:
void delete_fifo(FIFO*& fifo_obj){

   delete fifo_obj;
   fifo_obj = nullptr;
};
