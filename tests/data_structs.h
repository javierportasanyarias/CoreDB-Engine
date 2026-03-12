#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

#include<iostream>

class FifoNode {

   public:
   std::string comando;
   FifoNode* nxt_node;

   FifoNode();

};

class FIFO {

   public:
      FifoNode* head;

      FIFO();

      void delete_fifo_content(FifoNode*& head);

};

// Para eliminar toda la fifo:
void delete_fifo(FIFO*& fifo_obj);

#endif
