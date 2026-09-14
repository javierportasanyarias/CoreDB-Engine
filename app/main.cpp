#include "text_utils.h"
#include "execution_planning.h"
#include "process_tokens.h"
#include "disk_io.h"
#include "disk_metadata.h"
#include "disk_wal_read.h"

int main(){

   /////////////////////////////////////////////////////////////////
   // BUCLE DE EJECUCION:

   std::string input = "";
   std::string handshake = "";

   // Fijamos el nivel de logs a debug:
   Logger::level = LogLevel::OUTPUT;
   
   // Reding all table's metadata within disk beforehand:
   disk_metadata::read_all_tables_metadata();

   // Reading WAL data (if there is any):
   disk_wal_read::read_wal();


   while(true){

      // Mian program loop

      Logger::log(LogLevel::OUTPUT, "\n> ", false, false);
      Logger::login(input);
      Logger::flush();

      // Exit condition:
      if(input == "exit" || input == "exit\n"){
         break;
      };

      // Input text processing (token list construction):
      textUtils::simpleLinkedList* token_list = textUtils::process_text_pipeline(input);
      Logger::flush(LogLevel::DEBUG);
      {
         std::string tmp_str = "EOS";
         token_list->add_node(tmp_str);
      };

      // Constructiong execution queue;                                                                           
      execPlan::Queue* excec_queue = new execPlan::Queue;                   
      excec_queue = process_token_list(token_list);
      // We no longer will be needing the token list, thus we erase it:
      delete token_list;
      token_list = nullptr;
    
      if(Logger::level == LogLevel::DEBUG){
         excec_queue->printNodeTypes();
      };

      // Carrying execution queue;
      excec_queue->execute_queue_tasks();
      // Deleting execution queue;
      execPlan::delete_task_queue(excec_queue);
      excec_queue = nullptr;
      if(Logger::level == LogLevel::DEBUG){
         Logger::log(LogLevel::DEBUG, "handshake: ", false, true);
         Logger::login(handshake);
      };
      Logger::flush(LogLevel::DEBUG);
      // only printing '__END__' if DEBUG mode is active
      Logger::log(LogLevel::DEBUG, "__END__", true, false);


   };  // Main loop end
   // Outside the main loop, program will be soon be terminated

   // Writing all table data defined within session:
   disk_io::write_dump();
   // only printing '__END__' if DEBUG mode is active
   Logger::log(LogLevel::DEBUG, "__END__", true, false);

   return 0;
}; // Aquí acaba el main
