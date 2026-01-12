#include <iostream>
#include <string>
#include <vector>
// #include "text_manipulation/text_utils.h"
#include "text_utils.h"
// #include "planning_execution/execution_planning.h"
#include "execution_planning.h"
// #include "nodes_for_trees/node_for_trees.h"
#include "node_for_trees.h"
// #include "text_manipulation/process_tokens.h"
#include "process_tokens.h"
// #include "data_struct.h"
// #include "execution/execution.h"
#include "execution.h"
#include <variant>
#include <unordered_map>
// #include "globals/globals.h"
#include "globals.h"
#include "logging.h"
#include "tests_1.h"

int main(){

   /////////////////////////////////////////////////////////////////
   // BUCLE DE EJECUCION:

   bool cond1 = true;
   std::string input = "";
   //Pequeño cambio para probsr push desde termux
   // Cambio realizado desde otro dispositivo

   // Fijamos el nivel de logs a debug:
   Logger::level = LogLevel::OUTPUT;


   while(cond1){

      Logger::log(LogLevel::OUTPUT, "\n> ", false, false);
      std::getline(std::cin, input);

      // Condicion de salida:
      if(input == "exit"){
         break;
      };
      // Ahora una funcion ddcidira si deja intacto el input o dara codigo de testeo de forma automatica:
      input = define_tests(input);

      // Ahora procesamos el texto:
      textUtils::simpleLinkedList* lista1 = new textUtils::simpleLinkedList;
      lista1 = textUtils::procesar_texto_pipeline(input);
      lista1->add_node("EOS");
      if(Logger::level == LogLevel::DEBUG){
         lista1->print_list();
      };
      Logger::log(LogLevel::DEBUG, "", true, false);
      Logger::log(LogLevel::DEBUG, "", true, false);

	 // Construimos la cola de ejecucion:
    Logger::log(LogLevel::DEBUG, "Construimos la cola de ejecucion:");                                                                              
    execPlan::Queue* excec_queue = new execPlan::Queue;                   
    excec_queue = procesar_lista_tokens(*lista1);

	 Logger::log(LogLevel::DEBUG, "", true, false);
    Logger::log(LogLevel::DEBUG, "", true, false);
    Logger::log(LogLevel::DEBUG, "", true, false);
    
    if(Logger::level == LogLevel::DEBUG){
	   excec_queue->printNodeTypes();
    };

	 Logger::log(LogLevel::DEBUG, "", true, false);

	 // Ejecutamos la cola de ejecucion:
    Logger::log(LogLevel::DEBUG, "Ejecutamos la cola de tareas: ");                                                                                   
    excec_queue->execute_queue_tasks();
    // Eliminamos la cola de tareas una vez ejecutada:
    execPlan::delete_task_queue(excec_queue);
    excec_queue = nullptr;
	 
    Logger::log(LogLevel::DEBUG, "", true, false);                     
    Logger::log(LogLevel::DEBUG, "", true, false);
    Logger::log(LogLevel::DEBUG, "", true, false);


   };

   Logger::log(LogLevel::DEBUG, "Terminada TODA LA EJECUCION");

   return 0;
};
