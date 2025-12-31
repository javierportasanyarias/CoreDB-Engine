#ifndef TEXT_UTILS_H
#define TEXT_UTILS_H

#include <iostream>
#include <string>
#include <vector>

namespace textUtils {
   std::string espaciar_texto(std::string input);


   class NodeLista1{

      public:
      std::string val;
      NodeLista1* nxt_node;
      NodeLista1(): nxt_node(nullptr){};
   };

   class simpleLinkedList{

      public:
      NodeLista1* head;
      NodeLista1* tail;

      simpleLinkedList(): head(nullptr), tail(nullptr){};

      void add_node(std::string valor);

      void print_list();
   };

   std::string borrar_espacios_repetidos(const std::string& input);

   std::string borrar_espacios_principio(const std::string& input);

   void borrar_espacios_final(std::string& input);

   simpleLinkedList* crear_lista_tokens(const std::string& input);

   simpleLinkedList* procesar_texto_pipeline(std::string& input);
};

# endif
