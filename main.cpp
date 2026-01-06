#include <iostream>
#include <string>
#include <vector>
#include "text_utils.h"
#include "execution_planning.h"
#include "node_for_trees.h"
#include "process_tokens.h"
// #include "data_struct.h"
#include "plan_execution.h"
#include <variant>
#include <unordered_map>
#include "globals.h"

int main(){

   /////////////////////////////////////////////////////////////////
   // BUCLE DE EJECUCION:

   bool cond1 = true;
   std::string input = "";
   //Pequeño cambio para probsr push desde termux
   // Cambio realizado desde otro dispositivo


   while(cond1){

      std::cout<<"\n> ";
      std::getline(std::cin, input);

      // Condicion de salida:
      if(input == "exit"){
         break;
      };
      if(input == "default"){
         input = "    CREATE    TABLE  t1    ( ID STRING PRIMARY KEY , Edad INT, Producto STRING) ; INSERT INTO t1 VALUES( 'Carlos', 49, 'Secadora'); INSERT INTO t1 VALUES( 'Andrea', 34, 'Lavadora'); INSERT INTO t1 ( ID, Edad, Producto) VALUES( 'Edu', 29, 'Plancha'); INSERT INTO t1 VALUES ('Cassandra', 47, 'Batidora'), ( 'Lucas', 19, 'Correa'); SELECT * FROM t1; SELECT ID, Producto FROM t1; ";
      };

      // Ahora procesamos el texto:
      textUtils::simpleLinkedList* lista1 = new textUtils::simpleLinkedList;
      lista1 = textUtils::procesar_texto_pipeline(input);
      lista1->add_node("EOS");
      lista1->print_list();
      std::cout<<std::endl;
      std::cout<<std::endl;

	 // Construimos la cola de ejecucion:
	 std::cout<<"Construimos la cola de ejecucion:"<<std::endl;                                                                                  
    execPlan::Queue* excec_queue = new execPlan::Queue;                   
    excec_queue = procesar_lista_tokens(*lista1);

	 std::cout<<std::endl;                                                 
    std::cout<<std::endl;
    std::cout<<std::endl;

	 excec_queue->printNodeTypes();

	 std::cout<<std::endl;

	 // Ejecutamos la cola de ejecucion:
	 std::cout<<"Ejecutamos la cola de tareas: "<<std::endl;                                                                                     
    excec_queue->execute_queue_tasks();
	 
	 std::cout<<std::endl;                                                 
    std::cout<<std::endl;
    std::cout<<std::endl;


   };

   return 0;
};
