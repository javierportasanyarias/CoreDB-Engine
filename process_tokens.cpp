#include <iostream>
#include <string>
#include <vector>
#include "text_utils.h"
#include "execution_planning.h"
#include "node_for_trees.h"
#include <any>
#include "data_struct.h"
#include "globals.h"
#include "process_tokens.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DEFINIMOS LAS FUNCIONES AUXILIARES:


NodeType2* aux_ddl_tree_2(textUtils::NodeLista1& c_l_n){

   NodeType2* nodo = new NodeType2;

   bool definido_nombre = false;
   bool definido_tipo = false;
   bool deninido_primary_key = false;

   while(c_l_n.val != ")" && c_l_n.val != ","){
      if (!c_l_n.nxt_node) {
        break; // Evita avanzar a nullptr
      }
      //std::cout<<"Valor de c_l_n.val: "<<c_l_n.val<<std::endl;

      if(deninido_primary_key){
         deninido_primary_key = false;
         //if(c_l_n.val == "")
         if(c_l_n.val == "PRIMARY KEY"){
            nodo->is_primary = true;
         }else{
            //std::cout<<"Valor de c_l_n.val al ver si esclave primaria: "<<c_l_n.val<<std::endl;
         };
      };
      if(definido_tipo){
         definido_tipo = false;
         deninido_primary_key = true;
         nodo->tipo = c_l_n.val;
      };
      if(!definido_nombre){
         definido_nombre = true;
         definido_tipo = true;
         nodo->name_campo = c_l_n.val;
      };
      c_l_n = *(c_l_n.nxt_node);

   };
   if(c_l_n.val == ","){
      c_l_n = *(c_l_n.nxt_node);
   };

   return nodo;
};

// FUNCIONES PARA DEFINIR EL ESQUEMA:
void crear_hijos_esquema_dado_padre(textUtils::NodeLista1& c_l_n, NodeType1* nodo){
   while(c_l_n.val != ";" && c_l_n.val != ")"){
      NodeType2* nodo_hijo = new NodeType2;
      nodo_hijo = aux_ddl_tree_2(c_l_n);
      //añadimos el nodo hijo al vector de hijos del padre:
      (nodo->hijos).push_back(nodo_hijo);
   };
};


void procesar_lista_para_definir_esquema(execPlan::Queue* excec_queue, textUtils::NodeLista1& c_l_n){

    NodeType1* nodo = new NodeType1;

    if(c_l_n.val != "CREATE TABLE"){
        throw std::runtime_error("Se esperaba 'TABLE' después de 'CREATE'");
        return;
    } else {
        // Creamos el nodos superior de este tipo de árbol:
        NodeType1* nodo = new NodeType1;
    };
    // Avanzamos al siguiente nodo:
    c_l_n = *(c_l_n.nxt_node);

    // Esperamos el nombre:
    nodo->nombre_tabla = c_l_n.val;

    // Avanzamos al siguiente nodo:
    c_l_n = *(c_l_n.nxt_node);

    // Ahora vemos si está el alias o pasa a definir los valores:
    if(c_l_n.val != "("){
        nodo->alias = c_l_n.val;
        // En este caso debemos avanzar un nodo adicinonal:
        c_l_n = *(c_l_n.nxt_node);
    };

    // Avanzamos al siguiente nodo:
    c_l_n = *(c_l_n.nxt_node);

    // Ahora rellenamos con los nodos hijos que corresponden con el esquema del nombre y tipo de todas las variables:
    crear_hijos_esquema_dado_padre(c_l_n, nodo);

    // Creamos y rellenamos el nodo de la cola
    execPlan::queueNode1* excec_queue_node = new execPlan::queueNode1;
    excec_queue_node->nodePtr = nodo;

    // ñadimos el nodo de la cola a la cola:

    excec_queue->add_node_to_queue(excec_queue_node);

    // Ahora creamos la tabla y enlazamos esa tsbla a la consulta:
    table* tb_created = new table;
    //nodo->tb_struct = tb_created;
    //
    // Incluimosl nombre y el alias en el doccoonario:
    //extern std::unordered_map<std::string, table> global_table_dict;
    global_table_dict[nodo->nombre_tabla] = tb_created;
    global_table_dict[nodo->alias] = tb_created;
    
    return;
};


void insert_values_add_columns_to_node(NodeType3* nodo, textUtils::NodeLista1& c_l_n){
   while(c_l_n.val != ")"){
        if(c_l_n.val != ","){
            //añadimos las columnas
            (nodo->columnas).push_back(c_l_n.val);
        }
        c_l_n = *(c_l_n.nxt_node);
   };
};


void aux_iterative_value_filler(textUtils::NodeLista1& c_l_n, std::vector<std::string>& vector_fila){

    while(c_l_n.val != ")" && c_l_n.val != "EOS" && c_l_n.val != ";"){
        if(c_l_n.val != ","){
	    std::cout<<"Insertamos el valor: ";
	    std::cout<<c_l_n.val<<" ";
	    std::cout<<"en el vector"<<std::endl;
            vector_fila.push_back(c_l_n.val);
        };
        // Avanzamos en el bucle:
        c_l_n = *(c_l_n.nxt_node);
    };

    //return;

};

void insert_row_values_in_node(NodeType3* nodo, textUtils::NodeLista1& c_l_n){

    while(c_l_n.val != ";" && c_l_n.val != "EOS"){

        if(c_l_n.val == "("){
            // Avanzamos un token extra:
            c_l_n = *(c_l_n.nxt_node);
            // Declaramos un nuevo vector:
            std::vector<std::string> vector_fila;
            // Llamamos a la función auxiliar
	    std::cout<<"Entramos en donde se insertan los valores: "<<
std::endl;
            aux_iterative_value_filler(c_l_n, vector_fila);
            std::cout<<"Insertamos el vector en el vector de vectores:"<<std::endl;
            (nodo->filas).push_back(vector_fila);
	    std::cout<<"Ya fue insertado el vector en el vector de vectores"<<std::endl;
        };
        // Avanzamos en el bucle:
        c_l_n = *(c_l_n.nxt_node);
    };
    // Avanzamos un token adicional si encontramos el cierre de la sentencia:
    if(c_l_n.val == ")"){
        c_l_n = *(c_l_n.nxt_node);
    };
    std::cout<<"Hemos terminado de crear eo nodo de insertar valores con un valor del nodo: ";
    std::cout<<c_l_n.val<<std::endl;
};

// FUNCIONES PARA INSERTAR VALORES:
void procesar_lista_para_insertar_valores(execPlan::Queue* excec_queue, textUtils::NodeLista1& c_l_n){

    NodeType3* nodo = new NodeType3;

    if(c_l_n.val != "INSERT INTO"){
        throw std::runtime_error("Se esperaba 'INTO' después de 'INSERT'");
        return;
    } else {
        // Creamos el nodos superior de este tipo de árbol:
        NodeType3* nodo = new NodeType3;
    };

    // Avanzamos al siguiente nodo:
    c_l_n = *(c_l_n.nxt_node);

    // Ahora recibiremos el nombre de la tabla:
    nodo->nombre_tabla = c_l_n.val;

    // Avanzamos al siguiente nodo:
    c_l_n = *(c_l_n.nxt_node);

    // Ahora vemos si hay paréntesis o no: vemos si se definirán las columnas o no:
    if(c_l_n.val == "("){
        // Caso de que se especifiquen las columnas:
        c_l_n = *(c_l_n.nxt_node);
        // Ahora un pequeño bucle para ir añadiendo los nombres de las columnas a procesar:
        insert_values_add_columns_to_node(nodo, c_l_n);

    };


     // Si econtramos un ')' avanzamos un token adicional:
    if(c_l_n.val == ")"){
        c_l_n = *(c_l_n.nxt_node);
    };




     // Ahora viene la parte de añadir valores:
    if(c_l_n.val == "VALUES"){
        c_l_n = *(c_l_n.nxt_node);
    } else{
        std::cout<<"Este es el valor del nodo que ha dado este error: ";
        std::cout<<c_l_n.val<<std::endl;
        throw std::runtime_error("Se esperaba 'VALUES' despues de definir las columnas");
    };

    // Ahora insertamos los valores:
    std::cout<<"Insertamos los valores:"<<std::endl;
    insert_row_values_in_node(nodo, c_l_n);
    std::cout<<"SE HA SALIDO DE LA FUNCION 'insert_row_values_in_node'"<<std::endl;

    // Creamos y rellenamos el nodo de la cola
    execPlan::queueNode1* excec_queue_node = new execPlan::queueNode1;
    excec_queue_node->nodePtr = nodo;

    // ñadimos el nodo de la cola a la cola:

    excec_queue->add_node_to_queue(excec_queue_node);

    std::cout<<"SALIMOS DE LA FUNCION: 'procesar_lista_para_insertar_valores'"<<std::endl;
    
    return;

};

void aux_col_consulta(QueryNode*& nodo_consulta, textUtils::NodeLista1& c_l_n){
   // Creamos el nodo del Select:
   SelectNode* nodo_seleccionar = new SelectNode;

   while(c_l_n.val != "FROM"){
      //Creamos un nuevo nodo de item:
      ItemNode nodo_item;
      nodo_item.nombre = c_l_n.val;
      (nodo_seleccionar->items).push_back(nodo_item);
      c_l_n = *(c_l_n.nxt_node);
      if(c_l_n.val == ","){
         c_l_n = *(c_l_n.nxt_node);
      };
   };

   // Al final dwl todo lo enoazamos cob el modo raiz de la consulta:
   nodo_consulta->nodo_select = nodo_seleccionar;
};

// PARA DEFINIR LAS CONSULTAS:
void procesar_lista_para_consulta(execPlan::Queue* excec_queue, textUtils::NodeLista1& c_l_n){

   // Avanzamos uno:
   std::cout<<"IINCIAMOS LA CREACION DEL ARBOL DE CONSULTA"<<std::endl;
   c_l_n = *(c_l_n.nxt_node);
   QueryNode* nodo_consulta = new QueryNode;

   // Ahora vamos a un bucle para definir las columnas de la consulta:
   if(c_l_n.val != "*"){
      // Funcion auxiliar para recorrer las columnas a consultar:
      aux_col_consulta(nodo_consulta, c_l_n);

   } else{
      c_l_n = *(c_l_n.nxt_node);
   };

   // Ahora vemos si encontramos el from:
   if(c_l_n.val == "FROM"){
      c_l_n = *(c_l_n.nxt_node);
      FromNode* nodo_desde = new FromNode;
      // ahora viene el nombre de la tabla:
      nodo_desde->nombre = c_l_n.val;
      nodo_consulta->nodo_from = nodo_desde;

   };

   // Creamos y rellenamos el nodo de la cola
    execPlan::queueNode1* excec_queue_node = new execPlan::queueNode1;
    excec_queue_node->nodePtr = nodo_consulta;

    // ñadimos el nodo de la cola a la cola:

    excec_queue->add_node_to_queue(excec_queue_node);

    std::cout<<"Se ha adicionado la consulta al arbol"<<std::endl;

   return;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
execPlan::Queue* procesar_lista_tokens(textUtils::simpleLinkedList& lista){

    // Obtenemos le primer nodo de la lista:
    textUtils::NodeLista1 c_l_n = *(lista.head);

    // Creamos la cola:
    execPlan::Queue* excec_queue = new execPlan::Queue;

    while(c_l_n.val != "EOS"){

        std::cout<<"Valor del token: ";
        std::cout<<c_l_n.val<<std::endl;

        if(c_l_n.val == "CREATE TABLE"){
            std::cout<<"Procedemos a definir el esquema:"<<std::endl;
            procesar_lista_para_definir_esquema(excec_queue, c_l_n);
        } else if(c_l_n.val == "INSERT INTO"){
            std::cout<<"Procedemos a insertar valores:"<<std::endl;
            procesar_lista_para_insertar_valores(excec_queue, c_l_n);
        } else if(c_l_n.val == "SELECT"){
            procesar_lista_para_consulta(excec_queue, c_l_n);
	};
    
        // Ahora, si encontramos un ";" avanzamos un nodo adicinoal en la lista de tokens:
        if(c_l_n.val == ";"){
            std::cout<<"saltamos los valores ';' :"<<std::endl;
            c_l_n = *(c_l_n.nxt_node);
        };

        // Avanzamos si o si al siguienre nodo:
        if(c_l_n.val != "CREATE TABLE" && c_l_n.val !="INSERT INTO" && c_l_n.val != "EOS" && c_l_n.val != "SELECT"){
            c_l_n = *(c_l_n.nxt_node);
        };
    };

    return excec_queue;

};
