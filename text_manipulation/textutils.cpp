#include "text_utils.h"
#include <iostream>
#include <string>
#include <vector>
#include <cctype>  // para std::isspace
#include "logging.h"

std::string textUtils::espaciar_texto(const std::string& input){

   //std::string buffer = "";
   std::string result = "";

   for(int i = 0; i<input.size(); i++){

      //buffer += input[i];
      switch(input[i]){

      case ')':
         result = result + " " + input[i] + " ";
         break;
      case '(':
         result = result + " " + input[i] + " ";
         break;
      case ';':
         result = result + " " + input[i]+ " ";
         break;
      case ',':
         result = result + " " + input[i] + " ";
         break;
      default:
         result += input[i];
      };
   };

   result += " ";
   return  result;
};


void textUtils::simpleLinkedList::add_node(std::string valor){
   textUtils::NodeLista1* nuevo = new textUtils::NodeLista1();
   nuevo->val = valor;
   Logger::log(LogLevel::DEBUG, "Dentro de add_node: ", false, true);
   Logger::log(LogLevel::DEBUG, nuevo->val, true, false);
   nuevo->nxt_node = nullptr; 

   if(!head){
      Logger::log(LogLevel::DEBUG, "Se ha entrado a modificar head y tail");   
      head = nuevo;
      tail = nuevo;
   } else {
      // EN LUGAR DE RECORRER DESDE EL PRINCIPIO (que puede tener ciclos)
      // USAMOS EL PUNTERO DIRECTO AL FINAL
      tail->nxt_node = nuevo; 
      tail = nuevo;
      if (head->val != "CREATE TABLE"){
         Logger::log(LogLevel::DEBUG, "ERROR: SE HA MODIFICADO EL VALOR DEL HEAD. Se ha cambiado por el valor: ", false, true);
	 Logger::log(LogLevel::DEBUG, head->val, true, false);

      };
   };
};




void textUtils::simpleLinkedList::print_list() {
   if (!head) {
      // std::cout << "(empty list)" << std::endl;
      Logger::log(LogLevel::DEBUG, "(empty list)");
      return;
   };

   textUtils::NodeLista1* current_node = head;
   // std::cout << current_node->val;  // imprime el primer nodo
   Logger::log(LogLevel::DEBUG, current_node->val);

   // Recorre e imprime el resto con el separador ->
   while (current_node->nxt_node) {
      current_node = current_node->nxt_node;
      // std::cout << "->" << current_node->val;
      Logger::log(LogLevel::DEBUG, "->", false, false);
      Logger::log(LogLevel::DEBUG, current_node->val, false, false);
   };

   // std::cout << std::endl;
   Logger::flush();
};

// Metodo para borrar la lista de tokens:
void textUtils::simpleLinkedList::clear() {
    NodeLista1* current = head;
    while (current != nullptr) {
        NodeLista1* next = current->nxt_node; // Guardamos el puntero al siguiente
        delete current;                       // Borramos el nodo actual
        current = next;                       // Saltamos al siguiente
    }
    head = nullptr;
    tail = nullptr;
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
   std::string buffer = "";
   textUtils::simpleLinkedList* lista = new textUtils::simpleLinkedList();

   for(int i = 0; i < input.size(); i++){
      if(input[i] == ' '){
         // Solo entramos aquí si hay algo que procesar
         if(!buffer.empty()){
            if(buffer == "CREATE" || buffer == "PRIMARY" || buffer == "INSERT" || buffer == "DROP"){
               buffer += input[i];
            } else {
               Logger::log(LogLevel::DEBUG, "Add to lista: ", false, true);
	       Logger::log(LogLevel::DEBUG, buffer, true, false);
               lista->add_node(buffer);
               buffer = "";
            };
         };
      } else {
         buffer += input[i];
      };
   };
   // Añadir el último token si quedó algo
   if(!buffer.empty()) {
      lista->add_node(buffer);
   };
   return lista;
};


std::string limpiar_comienzo_input(std::string& input){
    size_t start = input.find_first_of(" ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
       if (start != std::string::npos) {
          input = input.substr(start);
       };
    return input;
};

textUtils::simpleLinkedList* textUtils::procesar_texto_pipeline(std::string& input){

   Logger::log(LogLevel::DEBUG, "Vemos como llega el input de texto:", true, false);
   Logger::log(LogLevel::DEBUG, input, true, false);
   // Ahora realizamos la limpieza:
   //input = limpiar_comienzo_input(input);
   //Logger::log(LogLevel::DEBUG, "Despues de limpiar el comienzo:", true, false);
   //Logger::log(LogLevel::DEBUG, input, true, false);
   input = espaciar_texto(input);
   Logger::log(LogLevel::DEBUG, "Despues de espaciar texto:", true, false);
   Logger::log(LogLevel::DEBUG, input, true, false);
   input = borrar_espacios_repetidos(input);
   Logger::log(LogLevel::DEBUG, "Despues de borrar espacios repetidos:", true, false);
   Logger::log(LogLevel::DEBUG, input, true, false);
   input = borrar_espacios_principio(input);
   Logger::log(LogLevel::DEBUG, "Despues de borrar espacios al principio:", true, false);
   Logger::log(LogLevel::DEBUG, input, true, false);
   borrar_espacios_final(input);
   Logger::log(LogLevel::DEBUG, "Texto formateado:", true, false);
   Logger::log(LogLevel::DEBUG, input, true, false);
   // Ahora realizamos la limpieza:
   input = limpiar_comienzo_input(input);
   Logger::log(LogLevel::DEBUG, "Despues de limpiar el comienzo:", true, false);
   Logger::log(LogLevel::DEBUG, input, true, false);
   textUtils::simpleLinkedList* lista_a_retornar;
   lista_a_retornar = crear_lista_tokens(input);
   Logger::log(LogLevel::DEBUG, "head de la lista:", true, false);
   Logger::log(LogLevel::DEBUG, lista_a_retornar->head->val, true, false);
   return lista_a_retornar;
};
