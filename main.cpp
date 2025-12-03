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

   // Antes de nada creamos el doccionario con las tablas:
   //std::unordered_map<std::string, table > global_table_dict;

   //std::string cadena = "    CREATE    TABLE  t1    ( ID STRING PRIMARY KEY , Edad INT, Producto STRING) ; INSERT INTO t1 VALUES( 'Carlos', 49, 'Secadora'); INSERT INTO t1 VALUES( 'Andrea', 34, 'Lavadora'); INSERT INTO t1 ( ID, Edad, Producto) VALUES( 'Edu', 29, 'Plancha'); INSERT INTO t1 VALUES ('Cassandra', 47, 'Batidora'), ( 'Lucas', 19, 'Correa');";
   //std::string re1;
   //std::string re2;
   //std::string re3;
   //std::string re4;
   //re1 = textUtils::espaciar_texto(cadena);
   //std::cout<<"Cadena original:"<<std::endl;
   //std::cout<<re1<<std::endl;
   //std::cout<<std::endl;

   //re2 = textUtils::borrar_espacios_repetidos(re1);
   //re3 = textUtils::borrar_espacios_principio(re2);
   //re4 = textUtils::borrar_espacios_final(re3);
   //std::cout<<"Cadena procesada: "<<std::endl;
   //std::cout<<re4<<std::endl;
   //std::cout<<std::endl;

   //textUtils::simpleLinkedList lista1;
   //lista1 = textUtils::crear_lista_tokens(re4);
   //(lista1.tail)->val = "EOS";
   //lista1.add_node("EOS");
   //lista1.print_list();
   //std::cout<<std::endl;
   //std::cout<<std::endl;

   //std::cout<<"Valor de head"<<std::endl;
   //std::cout<<((lista1.head)->val)<<std::endl;

   //std::cout<<std::endl;
   //std::cout<<std::endl;
   //std::cout<<std::endl;

   //std::cout<<"Creamos el objeto del arbol:"<<std::endl;
   //tree tree_for_schema;
   //std::cout<<"Construimos el arbol:"<<std::endl;
   //arbol1.constrir_arbol(lista1);
   //std::cout<<"Imprimimps la raiz del arbol"<<std::endl;
   //std::cout<<(arbol1.root)->nombre_tabla<<std::endl;
   //std::cout<<"Imprimimos el arbol DDL:"<<std::endl;
   //arbol1.imprimir_arbol();

   //std::cout<<"Construimos la cola de ejecucion:"<<std::endl;

   //execPlan::Queue* excec_queue = new execPlan::Queue;
   //excec_queue = procesar_lista_tokens(lista1);

   //std::cout<<std::endl;
   //std::cout<<std::endl;
   //std::cout<<std::endl;

   //NodeType3* nodo = std::get<NodeType3*>((excec_queue->first_ptr)->nodePtr);
   //std::cout<<"Nombre de la tabla en la que se insertan los valores:"<<std::endl;
   //std::cout << nodo->nombre_tabla << std::endl;
   //excec_queue->printNodeTypes();


   //std::cout<<std::endl;
   //std::cout<<std::endl;
   //std::cout<<std::endl;

   //std::cout<<"Ejecutamos la cola de tareas: "<<std::endl;

   //excec_queue->execute_queue_tasks();

   //execPlan::NodeVariant nodoVariant = excec_queue->first_ptr->nodePtr;

   //NodeType1* primer_nodo;
   //table tb_retornada;

   //primer_nodo = std::get<NodeType1*>(nodoVariant);

   // Construimos el esquema:
   //recursive_metadata_fill_lv1(primer_nodo);

   //std::cout<<std::endl;
   //std::cout<<std::endl;
   //std::cout<<std::endl;



   //std::cout<<"Ahora realizamos la primera tarea de insercion:"<<std::endl;

   //NodeType3* seg_nodo;

   //execPlan::NodeVariant nodoVariant3 = excec_queue->first_ptr->nxt_node_queue->nodePtr;

   //seg_nodo = std::get<NodeType3*>(nodoVariant3);

   //Rellenamos los datos:
   
   //fill_table_with_values_v4(seg_nodo);
   //
   //
   /////////////////////////////////////////////////////////////////
   // BUCLE DE EJECUCION:

   bool cond1 = true;
   std::string input = "";
   std::string re1 = "";
   std::string re2 = "";
   std::string re3 = "";
   std::string re4 = "";
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
         re1 = textUtils::espaciar_texto(input);
         std::cout<<"Cadena original:"<<std::endl;
         std::cout<<re1<<std::endl;
         std::cout<<std::endl;

         re2 = textUtils::borrar_espacios_repetidos(re1);                      re3 = textUtils::borrar_espacios_principio(re2);
         re4 = textUtils::borrar_espacios_final(re3);
         std::cout<<"Cadena procesada: "<<std::endl;
         std::cout<<re4<<std::endl;
         std::cout<<std::endl;

         textUtils::simpleLinkedList lista1;                                   lista1 = textUtils::crear_lista_tokens(re4);
         //(lista1.tail)->val = "EOS";
         lista1.add_node("EOS");
         lista1.print_list();
         std::cout<<std::endl;
         std::cout<<std::endl;

	 // Construimos la cola de ejecucion:
	 std::cout<<"Construimos la cola de ejecucion:"<<std::endl;                                                                                  execPlan::Queue* excec_queue = new execPlan::Queue;                   excec_queue = procesar_lista_tokens(lista1);

	 std::cout<<std::endl;                                                 std::cout<<std::endl;
         std::cout<<std::endl;

	 excec_queue->printNodeTypes();

	 std::cout<<std::endl;

	 // Ejecutamos la cola de ejecucion:
	 std::cout<<"Ejecutamos la cola de tareas: "<<std::endl;                                                                                     excec_queue->execute_queue_tasks();
	 
	 std::cout<<std::endl;                                                 std::cout<<std::endl;
         std::cout<<std::endl;


   };

   return 0;
};
