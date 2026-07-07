#include "data_structs.h"


//============================
//== NODOS: ==================
//============================

// Construimos el nodo:
FifoNode::FifoNode(): comando(""), nxt_node(nullptr){};

//============================
//== COLA: ===================
//============================

// Construimos la cola:
FIFO::FIFO(): head(nullptr){};

// Metodo auxilisr para eliminar nodos recursivamente:
void delete_fifo_node_recursive_helper(FifoNode*& c_n_ptr){

   while(c_n_ptr->nxt_node){
      delete_fifo_node_recursive_helper(c_n_ptr->nxt_node);
   };

   delete c_n_ptr;
   c_n_ptr = nullptr;
};

// Eliminamos todos sus nodos:
void FIFO::delete_fifo_content(FifoNode*& head){

   delete_fifo_node_recursive_helper(head); 

};

// Eliminamos la FIFO entera:
void delete_fifo(FIFO*& fifo_obj){

   delete fifo_obj;
   fifo_obj = nullptr;
};
