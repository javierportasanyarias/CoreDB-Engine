#include "text_utils.h"
#include <iostream>
#include <string>
#include <vector>
#include <cctype>  // para std::isspace

std::string textUtils::espaciar_texto(std::string input){

   std::string* buffer = new std::string("");
   std::string* result = new std::string;

   for(int i = 0; i<input.size(); i++){

      *buffer += input[i];
      switch(input[i]){

      case ')':
         *result = *result + " " + input[i] + " ";
         break;
      case '(':
         *result = *result + " " + input[i] + " ";
         break;
      case ';':
         *result = *result + " " + input[i]+ " ";
         break;
      case ',':
*result = *result + " " + input[i] + " ";
         break;
      default:
         *result += input[i];
      };
   };

   *result += " ";
   return  *result;
};


//class textUtils::NodeLista1{
//
//   public:
//   std::string val;
//   textUtils::NodeLista1* nxt_node;
//   NodeLista1(): nxt_node(nullptr){};
//};


void textUtils::simpleLinkedList::add_node(std::string valor){
   textUtils::NodeLista1* nodo = new textUtils::NodeLista1;
   nodo->val = valor;
   if(head){
      textUtils::NodeLista1* current_node = head;
while(current_node->nxt_node){
         current_node = current_node->nxt_node;
      };
      current_node->nxt_node = nodo;
      tail = nodo;
   }else{
      head = nodo;
      tail = head;
   };
};

void textUtils::simpleLinkedList::print_list() {
   if (!head) {
      std::cout << "(empty list)" << std::endl;
      return;
   };

   textUtils::NodeLista1* current_node = head;
   std::cout << current_node->val;  // imprime el primer nodo

   // Recorre e imprime el resto con el separador ->
   while (current_node->nxt_node) {
      current_node = current_node->nxt_node;
      std::cout << "->" << current_node->val;
   };

   std::cout << std::endl;
};

std::string textUtils::borrar_espacios_repetidos(const std::string& input){

   //std::string* result = new std::string("");
   std::string result = "";
   bool auxbool = true;

   for(int i = 0; i<input.size(); i++){

      //*buffer += input[i];

      if(input[i] == ' '){
         if(auxbool){
            //*result += input[i];
	    result += input[i];
            auxbool = false;
         };

      }else{
         //*result += input[i];
	 result += input[i];
         auxbool = true;
      };

   };
   //*result += " ";
   result += " ";
   //return *result;
   return result;
};

std::string textUtils::borrar_espacios_principio(const std::string& input){

   //std::string* result = new std::string("");
   std::string result = "";
   //std::string* buffer = new std::string("");

   bool auxbool = false;
    for(int i = 0; i<input.size(); i++){
       if(input[i] != ' '){
          auxbool = true;
       };
       if(auxbool){
          //*result += input[i];
	  result += input[i];
       };
    };

    //return *result;
    return result;
};

void textUtils::borrar_espacios_final(std::string& input) {
    if (input.empty()) return;

    int i = static_cast<int>(input.size()) - 1; // índice del último carácter

    // Retrocede mientras haya espacios al final
    while (i >= 0 && std::isspace(static_cast<unsigned char>(input[i]))) {
        i--;
    };

    // Devuelve la subcadena sin los espacios del final
    input = input.substr(0, i + 1);
};

textUtils::simpleLinkedList* textUtils::crear_lista_tokens(const std::string& input){

   std::string* buffer = new std::string("");
   textUtils::simpleLinkedList* lista = new textUtils::simpleLinkedList;
   for(int i = 0; i<input.size(); i++){
      //*buffer += input[i];
      if(input[i] == ' '){
         if(*buffer == "CREATE" || *buffer == "PRIMARY" || *buffer == "INSERT"){
            *buffer += input[i];
         }else {
            lista->add_node(*buffer);
            *buffer = "";
         };
}else{
         *buffer += input[i];
      };
   };

   return lista;
};

textUtils::simpleLinkedList* textUtils::procesar_texto_pipeline(std::string& input){

   input = espaciar_texto(input);
   input = borrar_espacios_repetidos(input);
   input = borrar_espacios_principio(input);
   borrar_espacios_final(input);
   return crear_lista_tokens(input);
};
