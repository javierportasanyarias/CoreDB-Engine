#pragma once

namespace textUtils {
   std::string espaciar_texto(const std::string& input);

   // Nodo para la lista enlaza simple de tokens:
   class NodeLista1{

      public:
         std::string val;
         NodeLista1* nxt_node;
         NodeLista1(): nxt_node(nullptr), val(""){};
   };

   class simpleLinkedList{

      public:
         NodeLista1* head;
         NodeLista1* tail; // Se podrá elimniar en el futuro ?

         simpleLinkedList(): head(nullptr), tail(nullptr){};

         // Añade el destructor
         ~simpleLinkedList() {
            clear();
         };

         void add_node(const std::string& valor);

         void print_list();

         void clear();
   };

   //std::string borrar_espacios_repetidos(const std::string& input);

   //std::string borrar_espacios_principio(const std::string& input);

   //void borrar_espacios_final(std::string& input);

   std::string normalize_spaces(const std::string& input);

   simpleLinkedList* crear_lista_tokens(const std::string& input);

   simpleLinkedList* procesar_texto_pipeline(std::string& input);
};
