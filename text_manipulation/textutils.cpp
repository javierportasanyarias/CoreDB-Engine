#include "text_utils.h"


std::string textUtils::space_text(const std::string& input){

   /*
   Adds extra space after there spcific characters:
   -> Parenthesis
   -> Dot
   -> Semicolon
   */

   std::string result = "";
   uint32_t input_size = input.size();
   const char* ptr_current = input.data();
   const char* ptr_end = ptr_current + input_size;
   result.reserve(input_size *2);

   while(ptr_current < ptr_end){

      char c = *ptr_current;
      switch(c){
         case ')':
         case '(':
         case ';':
         case ',':
            result += ' ';
            result += c;
            result += ' ';
            break;
         default:
            result += c;
            break;
      };
      ++ptr_current;
   };
   result += " ";
   return  result;
};


void textUtils::simpleLinkedList::add_node(const std::string& value){

   /*
   Adds a node to the end of it's simple linked list
   */

   textUtils::NodeList1* new_node = new textUtils::NodeList1();
   new_node->val = value;
   Logger::log(LogLevel::DEBUG, "Inside the add_node method: ", false, true);
   Logger::log(LogLevel::DEBUG, new_node->val, true, false);
   new_node->nxt_node = nullptr; 

   if(!head){
      Logger::log(LogLevel::DEBUG, "Head node has not been yet been defined, Adding the first node");   
      head = new_node;
      tail = new_node;
   } else {
      Logger::log(LogLevel::DEBUG, "Head node has already been defined, adding the lastes node to list");
      tail->nxt_node = new_node; 
      tail = new_node;
   };
};


void textUtils::simpleLinkedList::print_list() {

   /*
   Method for printing the token list.
   Solely used for debuggung purposes
   */

   if (!head) {
      Logger::log(LogLevel::DEBUG, "(empty list)");
      return;
   };

   textUtils::NodeList1* current_node = head;
   Logger::log(LogLevel::DEBUG, current_node->val);

   while (current_node->nxt_node) {
      current_node = current_node->nxt_node;
      Logger::log(LogLevel::DEBUG, "->", false, false);
      Logger::log(LogLevel::DEBUG, current_node->val, false, false);
   };
};


// Metodo para borrar la lista de tokens:
void textUtils::simpleLinkedList::clear() {

   /*
   Method for managing safe memory deletion of all the list nodes.
   Should only be invoked once the execution tree has been constructed.
   */

   NodeList1* current = head;
   while (current != nullptr) {
      NodeList1* next = current->nxt_node;
      delete current;
      current = next;
    };
   head = nullptr;
};


std::string textUtils::normalize_spaces(const std::string& input){

   /*
   This function erases repeted whitespaces, leaving only one between SQL tokens
   */

   bool allow_space = false;
   std::string result = "";
   uint32_t input_size = input.size();
   const char* ptr_current = input.data();
   const char* ptr_end = ptr_current + input_size;
   result.reserve(input_size);

   while(ptr_current < ptr_end && *ptr_current == ' '){
      ++ptr_current;
   };

   while(ptr_current < ptr_end){

      char current_char = *ptr_current;

      if(current_char == ' '){
         if(allow_space){
	         result += current_char;
            allow_space = false;
         };
      }else{
	      result += current_char;
         allow_space = true;
      };
      ptr_current++;
   };

   if(!result.empty() && result.back() == ' '){
      result.pop_back();
   };

   return result;
};


textUtils::simpleLinkedList* textUtils::create_token_list(const std::string& input){

   /*
   Given a clean/processed SQL code, it tokenized it into a simple linked list, used later
   for instruction interpetation and excetion trees construction.
   Each token corresponds to a SQL one.
   */


   std::string buffer;
   buffer.reserve(64);
   uint32_t input_size = input.size();
   const char* ptr_current = input.data();
   const char* ptr_end = ptr_current + input_size;

   textUtils::simpleLinkedList* list = new textUtils::simpleLinkedList();

   while(ptr_current < ptr_end){
      char c = *ptr_current;

      if(c == ' '){
         if(!buffer.empty()){
            if(buffer == "CREATE" || buffer == "PRIMARY" || buffer == "INSERT" || buffer == "DROP"){
               buffer += c;
            }else{
               Logger::log(LogLevel::DEBUG, "Added to list: ", false, true);
               Logger::log(LogLevel::DEBUG, buffer, true, false);
               list->add_node(buffer);
               buffer.clear();
            };
         };
      }else{
         buffer += c;
      };
      ++ptr_current;
   };
   // Just inn case some elements from the buffer were left unnadded, we append them here at the end:
   if(!buffer.empty()) {
      list->add_node(buffer);
   };
   return list;
};


std::string clean_input_start(std::string& input){
   
   /*
   Function destined for deleting 'garbage' characters. Thus raising input stability
   */

   size_t start = input.find_first_of(" ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
      if (start != std::string::npos) {
         input = input.substr(start);
      };
   return input;
};


textUtils::simpleLinkedList* textUtils::process_text_pipeline(std::string& input){

   /*
   Method for encapsulating input processing and token list creation.
   These are the inputs:
   -> Input: raw SQL instructions
   -> Output: simple linked list, each SQL tokeen as a single node.

   For achieving this, the input folows the folloeing steps:
   1º) space_text: adds a whitespace after the following charactes: '(', ')', ';' and ','.
   2º) normalize_spaces: ensures only one whitespace between SQl tokens.
   3º) clean_input_start: cleans the beggining of the string input, leaving only alphabetic characters at the start.
   4º) create_token_list: Token list creation.

   The token list given as an output will be employed in the excetion trees construction.
   */

   Logger::log(LogLevel::DEBUG, "Raw input text:", true, false);
   Logger::log(LogLevel::DEBUG, input, true, false);
   input = space_text(input);
   Logger::log(LogLevel::DEBUG, "After text spacing:", true, false);
   Logger::log(LogLevel::DEBUG, input, true, false);

   input = textUtils::normalize_spaces(input);
   Logger::log(LogLevel::DEBUG, "After normalizing whitespaces:", true, false);
   Logger::log(LogLevel::DEBUG, input, true, false);

   input = clean_input_start(input);
   Logger::log(LogLevel::DEBUG, "After beginning cleanup:", true, false);
   Logger::log(LogLevel::DEBUG, input, true, false);
   textUtils::simpleLinkedList* list_to_return;
   list_to_return = create_token_list(input);
   Logger::log(LogLevel::DEBUG, "Token list:");
   list_to_return->print_list();
   Logger::flush(LogLevel::DEBUG);
   return list_to_return;
};
