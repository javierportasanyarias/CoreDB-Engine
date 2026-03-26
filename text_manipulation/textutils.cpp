#include "text_utils.h"
#include <iostream>
#include <string>
#include <vector>
#include <cctype>  // para std::isspace
#include "logging.h"

std::string textUtils::espaciar_texto(const std::string& input){
   /*
   Atendiendo a carácteres especiales, añade un espacio entre medias.
   Estos carácteres son:
   -> Paréntesis, tanto de apaertira como de cierre
   -> Punto y coma
   -> Coma
   En caso de no toparse con alguno de estos elementos, no se hace nada
   */
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
   /*
   Método de la lista enlazada simple para añadir un nodo (al final de la lista enlazada).
   */
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
   /*
   Función únicamente destinada a imprimir la lista de tokens.
   Usada exclusivamente con propósitos de debugging.
   */
   if (!head) {
      Logger::log(LogLevel::DEBUG, "(empty list)");
      return;
   };

   textUtils::NodeLista1* current_node = head;
   Logger::log(LogLevel::DEBUG, current_node->val);

   // Recorre e imprime el resto con el separador ->
   while (current_node->nxt_node) {
      current_node = current_node->nxt_node;
      Logger::log(LogLevel::DEBUG, "->", false, false);
      Logger::log(LogLevel::DEBUG, current_node->val, false, false);
   };
   Logger::flush();
};


// Metodo para borrar la lista de tokens:
void textUtils::simpleLinkedList::clear() {
   /*
   Función encargada de la gestión de memoria de la lista enlaza simple de tokens.
   Esta elimina todo el contenido de esta listta enlaza.
   Se invocará esta función cuando ya se haya leído toda sta y por tanto ya esté
   contruido el 'árbol de ejecución'.
   */
   NodeLista1* current = head;
   while (current != nullptr) {
      NodeLista1* next = current->nxt_node; // Guardamos el puntero al siguiente
      delete current;                       // Borramos el nodo actual
      current = next;                       // Saltamos al siguiente
    };
   head = nullptr;
   tail = nullptr;
};


std::string textUtils::borrar_espacios_repetidos(const std::string& input){
   /*
   Función que borra los espacios 'repetidos'.
   Se considera espacios repetidos cuando hay dos o más espacios entre caracteres
   no vacíos. En dicho caso sólo se deja un único espacio.
   De esta forma se convierte a mucho más regular y predecible el espacio entre
   palabras, o lo que serán los tokens.
   */

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
   result += " ";
   return result;
};

std::string textUtils::borrar_espacios_principio(const std::string& input){
   /*
   Función que borra los espacios al principio de la cadena del input.
   */

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
   return result;
};


void textUtils::borrar_espacios_final(std::string& input) {
   /*
   Función que elimina los espacios al final.
   Esta función es 'inplace'. Osea, no necesita retornar
   output, ya que modifica el propio input.
   */
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
   /*
   Función que toma el input como cadena de texto ya limpia
   y 'tokeniza' o crea una lista enlaza simple, donde cada nodo
   es una parte indivisible de una instrucción de SQL.
   */
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
   /*
   Función destinada a eliminar los carácteres al principio que
   no coincidan con los presentados más abajo.
   La única finalidad de esta función es la de aumentar la
   estabilidad del input como input para crear la lista
   enlazada simple de tokens.
   */
   size_t start = input.find_first_of(" ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
      if (start != std::string::npos) {
         input = input.substr(start);
      };
   return input;
};


textUtils::simpleLinkedList* textUtils::procesar_texto_pipeline(std::string& input){
   /*
   Función que engloba el procesado del texto:
   Estos son los inputs/outputs:
   Input: Instrucciones/comandos brutos del usuario
   Output: Lista enlaza simple, es el input "tokenizado", los comandos se tokenizan
   o dividen en unidades indivisibles e interpetables por el intérpete.

   Estos son los pasos por los que pasa el input:
   -> espaciar_texto: Añade espacios alrededor de: '(', ')', ';' y ','
   -> borrar_espacios_repetidos: Borra los espacios innecesarios. Estos son los que ocupan
      dos o más espacios
   -> borrar_espacios_principio: Borra los espacios al principio
   -> borrar_espacios_final: Borra los espacios al final. Es además 'inplace'
   -> limpiar_comienzo_input: Borra carácteres 'extraños' al principio de la cadena de texto del input
   -> crear_lista_tokens: Última función en el proceso. Toma el texto de input ya limpio y
      crea la lista de tokens de la que se creará el 'árbol de ejecución'
   */

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
