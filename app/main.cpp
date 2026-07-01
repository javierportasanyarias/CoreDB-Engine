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
#include "disk_metadata.h"
#include "disk_wal_read.h"
#include "bateria_tests.h"

int main(){

   /////////////////////////////////////////////////////////////////
   // BUCLE DE EJECUCION:

   std::string input = "";
   std::string handshake = "";
   //Pequeño cambio para probsr push desde termux
   // Cambio realizado desde otro dispositivo

   // Fijamos el nivel de logs a debug:
   Logger::level = LogLevel::OUTPUT;
   
   // Antes de nada, vemos laa tablas en disco y escribimos sus metadatos en memoria
   disk_metadata::lectura_metadatos_todas_tablas();

   // Leemos también los datos recuperados del WAL:
   disk_wal_read::read_wal();
   //return 0;


   while(true){

      Logger::log(LogLevel::OUTPUT, "\n> ", false, false);
      Logger::login(input);
      Logger::flush();

      // Condicion de salida:
      if(input == "exit" || input == "exit\n"){
         break;
      };

      // Ahora procesamos el texto:
      textUtils::simpleLinkedList* lista1 = textUtils::procesar_texto_pipeline(input);
      Logger::flush(LogLevel::DEBUG);
      {
         std::string tmp_str = "EOS";
         lista1->add_node(tmp_str);
      };

      // Construimos la cola de ejecucion:                                                                            
      execPlan::Queue* excec_queue = new execPlan::Queue;                   
      excec_queue = procesar_lista_tokens(lista1);
      // Liberamos la lista dectokens ya quw n9 la usamos ya:
      delete lista1;
      lista1 = nullptr;
    
      if(Logger::level == LogLevel::DEBUG){
         excec_queue->printNodeTypes();
      };

      // Ejecutamos la cola de ejecucion:
      excec_queue->execute_queue_tasks();
      // Eliminamos la cola de tareas una vez ejecutada:
      execPlan::delete_task_queue(excec_queue);
      excec_queue = nullptr;
      if(Logger::level == LogLevel::DEBUG){
         Logger::log(LogLevel::DEBUG, "handshake: ", false, true);
         Logger::login(handshake);
      };
      // Solo imprimimos el '__END__' si estamos en modo DEBUG
      if(Logger::level == LogLevel::DEBUG){
         Logger::flush(LogLevel::DEBUG);
         Logger::log(LogLevel::DEBUG, "__END__", true, false);
      }else{
         Logger::flush(LogLevel::DEBUG, false);
      };


   };  // Aquí termina el bucle principal
   disk_io::write_dump();
   Logger::log(LogLevel::DEBUG, "__END__", true, false);
   if(Logger::level == LogLevel::DEBUG){
      Logger::log(LogLevel::DEBUG, "__END__", true, false);
   };

   return 0;
}; // Aquí acaba el main
