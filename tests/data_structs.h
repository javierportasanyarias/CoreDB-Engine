#pragma once


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
