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

#include "disk_io.h"
#include "bateria_tests.h"

int main(){

   /////////////////////////////////////////////////////////////////
   // BUCLE DE EJECUCION:

   std::string input = "";
   std::string handshake = "";
   //Pequeño cambio para probsr push desde termux
   // Cambio realizado desde otro dispositivo

   // Fijamos el nivel de logs a debug:
   Logger::level = LogLevel::DEBUG;
   FIFO* fifo_obj = nullptr;
   FifoNode* c_n_ptr = nullptr;
   bool bool_aux = false;
   int aux_counter = 0;
   //std::setvbuf(stdin, NULL, _IONBF, 0); // Desactiva el buffering de entrada

   while(true){

      Logger::log(LogLevel::DEBUG, "Comienza una iteracion del bucle principal:");
      Logger::log(LogLevel::OUTPUT, "\n> ", false, false);
      Logger::flush();
      if(bool_aux){
         if(!c_n_ptr){
            input = "exit";
	 };
      };
      //std::cin.clear();
      //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      //if (!std::getline(std::cin, input)) {
          //break;
      //};
      if(!bool_aux){
         Logger::login(input);
      };
      //Logger::clear_in();
      Logger::log(LogLevel::OUTPUT, "COMANDO LEIDO EN LA APP:");
      Logger::flush();
      Logger::log(LogLevel::OUTPUT, input);
      Logger::flush();

      // Condicion de salida:
      if(input == "exit" || input == "exit\n"){
	 Logger::log(LogLevel::DEBUG, "EXIT");
         break;
      } else{
	 if(!bool_aux){
	    if (input == "test1" || input == "test1\n"){
	       fifo_obj = define_test_1();
	       bool_aux = true;
	    }else if(input == "test2" || input == "test2\n"){
	       fifo_obj = define_test_2();
	       bool_aux = true;
	    }else if(input == "test3" || input == "test3\n"){
	       fifo_obj = define_test_3();
	       bool_aux = true;
	    };
	    if(bool_aux){
	       c_n_ptr = fifo_obj->head;
	    };

         };
      };
      if(bool_aux){
         input = c_n_ptr->comando;
      };
      Logger::flush();

      // Ahora procesamos el texto:
      //textUtils::simpleLinkedList* lista1 = new textUtils::simpleLinkedList;
      textUtils::simpleLinkedList* lista1 = textUtils::procesar_texto_pipeline(input);
      Logger::log(LogLevel::DEBUG, "procesar_texto_pipeline SE HA EJECUTADO");
      //if(aux_counter == 1){
         //break;
      //};
      Logger::flush();
      lista1->add_node("EOS");
      if(Logger::level == LogLevel::DEBUG){
         lista1->print_list();
      };
      Logger::log(LogLevel::DEBUG, "", true, false);
      Logger::log(LogLevel::DEBUG, "", true, false);
      Logger::flush();

      // Construimos la cola de ejecucion:
      Logger::log(LogLevel::DEBUG, "Construimos la cola de ejecucion:");                                                                              
    execPlan::Queue* excec_queue = new execPlan::Queue;                   
    excec_queue = procesar_lista_tokens(lista1);
    // Liberamos la lista dectokens ya quw n9 la usamos ya:
    delete lista1;
    lista1 = nullptr;
    Logger::flush();
    Logger::log(LogLevel::DEBUG, "", true, false);
    Logger::log(LogLevel::DEBUG, "", true, false);
    Logger::log(LogLevel::DEBUG, "", true, false);
    Logger::flush();
    
    if(Logger::level == LogLevel::DEBUG){
	   excec_queue->printNodeTypes();
    };

    Logger::log(LogLevel::DEBUG, "", true, false);
    Logger::flush();

	 // Ejecutamos la cola de ejecucion:
    Logger::log(LogLevel::DEBUG, "Ejecutamos la cola de tareas: ");
    excec_queue->execute_queue_tasks();
    Logger::flush();
    // Eliminamos la cola de tareas una vez ejecutada:
    execPlan::delete_task_queue(excec_queue);
    excec_queue = nullptr;
    Logger::flush();
	 
    Logger::log(LogLevel::DEBUG, "", true, false);                     
    Logger::log(LogLevel::DEBUG, "", true, false);
    Logger::log(LogLevel::DEBUG, "", true, false);
    Logger::flush();

    Logger::log(LogLevel::DEBUG, "Escribimos los datos del diccionario global en disco: ");
    //disk_io::write_dump();
    Logger::log(LogLevel::DEBUG, "Tablas escritas en disco ");
    //Logger::log(LogLevel::OUTPUT, "__END__", true, false);
    Logger::flush();
    //Logger::log(LogLevel::DEBUG, "handshake: ", true, true);
    //std::getline(std::cin, handshake);
    if(!bool_aux){
       Logger::login(handshake);
    };
    Logger::log(LogLevel::OUTPUT, "Se ha realizado el handshake", true, false);
    //Logger::clear_in();
    //Logger::log(LogLevel::DEBUG, "__END__", true, false);
    Logger::log(LogLevel::OUTPUT, "__END__", true, false);
    Logger::flush();

    if(bool_aux){
       c_n_ptr = c_n_ptr->nxt_node;
    };
    aux_counter += 1;


   };
   Logger::log(LogLevel::DEBUG, "Terminada TODA LA EJECUCION");
   Logger::flush();
   Logger::log(LogLevel::OUTPUT, "__END__", true, false);                      Logger::flush();

   return 0;
};
