#include <iostream>
#include <string>
#include <vector>
#include "text_utils.h"
// #include "planning_execution/execution_planning.h"
#include "execution_planning.h"
// #include "nodes_for_trees/node_for_trees.h"
#include "node_for_trees.h"
#include <any>
// #include "data_structure/data_struct.h"
#include "data_struct.h"
// #include "globals/globals.h"
#include "globals.h"
#include "process_tokens.h"
#include "logging.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Auxiliar class for keeping track of the tables created o already present:


void tableCatalog::register_table(std::string name){
    table_catalog.insert(name);
};

bool tableCatalog::ckeck_table(std::string name){
    return table_catalog.find(name) != table_catalog.end();
};

void tableCatalog::delete_table(std::string name){
    // Only if the table exists we erase it:
    if(tableCatalog::ckeck_table(name)){
        table_catalog.erase(name);
    };
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DEFINIMOS LAS FUNCIONES AUXILIARES:


NodeType2* aux_ddl_tree_2(textUtils::NodeLista1*& c_l_n){
    /*
    Función auxliar que se encarga de crear y rellenar los atributos de
    los nodos de tipo 'NodeType2'.
    */

    Logger::flush();
    Logger::log(LogLevel::DEBUG, "+++++++++++++++++++++++++++++++");

    NodeType2* nodo = new NodeType2;

    //bool definido_nombre = false;
    //bool definido_tipo = false;
    //bool deninido_primary_key = false;

    //while(c_l_n->val != ")" && c_l_n->val != ","){
        //if (!c_l_n->nxt_node) {
            //break; // Evita avanzar a nullptr
        //};

        /*if(deninido_primary_key){
            deninido_primary_key = false;
            if(c_l_n->val == "PRIMARY KEY"){
                nodo->is_primary = true;
            };
        };
        if(definido_tipo){
            definido_tipo = false;
            deninido_primary_key = true;
            nodo->tipo = c_l_n->val;
        };
        if(!definido_nombre){
            definido_nombre = true;
            definido_tipo = true;
            nodo->name_campo = c_l_n->val;
        };*/


        // NEW CODE:



    // Next node must be the value:
    Logger::log(LogLevel::DEBUG, "Valor del nombre del campo: ", false, true);
    Logger::log(LogLevel::DEBUG, c_l_n->val, true, false);
    nodo->name_campo = c_l_n->val;
    c_l_n = c_l_n->nxt_node;

    // Safeguard:
    if(!c_l_n) return nodo;

    Logger::log(LogLevel::DEBUG, "Valor del tipo de dato: ", false, true);
    Logger::log(LogLevel::DEBUG, c_l_n->val, true, false);
    // Next node must be data type:
    nodo->tipo = c_l_n->val;
    c_l_n = c_l_n->nxt_node;

    // Safeguard:
    if(!c_l_n) return nodo;

    if(c_l_n->val == "PRIMARY KEY"){
        Logger::log(LogLevel::DEBUG, "Es clave primaria");

        nodo->is_primary = true;
        //c_l_n = c_l_n->nxt_node;
        // Safeguard:
        //if(!c_l_n) return nodo;
    };
    Logger::log(LogLevel::DEBUG, "+++++++++++++++++++++++++++++++");
    Logger::flush();
    //c_l_n = c_l_n->nxt_node;

    //if(c_l_n->val == ","){
        // A conditional beacause it might be the final column, thus lacking a coma at the end of the definition
        //c_l_n = c_l_n->nxt_node;
    //};
    return nodo;
};


// FUNCIONES PARA DEFINIR EL ESQUEMA:
void crear_hijos_esquema_dado_padre(textUtils::NodeLista1*& c_l_n, NodeType1* nodo, execPlan::Queue* excec_queue){
    /*
    Función auxiliar que crea los hijos de 'NodeType1', el nodo que es la raiz del árbol
    para definir el esquema.
    Cada nodo 'NodeType2' es su hijo y corresponde a cada campo que se haya definido al
    crear la tabla.
    */
    if(c_l_n->val != "("){
        // First, we delete the whole task queue:
        execPlan::delete_whole_task_queue(excec_queue);
        // We delete all tehe tables from the global table dict:
        delete_all_tables_dict_only_mem();
        // Then, the other, defined, or half defined objects:
        delete nodo;
        nodo = nullptr;
        throw std::runtime_error("ERROR: Schema columns definition was never oppened with '('");
    };
    c_l_n = c_l_n->nxt_node;
    nodo->hijos.reserve(128);

    // First value lacks precceding coma, thus we keep it out of the loop:
    NodeType2* nodo_hijo = aux_ddl_tree_2(c_l_n);
    (nodo->hijos).push_back(std::move(nodo_hijo)); // Error que daba antes
    c_l_n = c_l_n->nxt_node; // We skip the to the coma
    Logger::log(LogLevel::DEBUG, "ADD 0 :", false, true);
    Logger::log(LogLevel::DEBUG, c_l_n->val, true, false);

    while(c_l_n->val != "EOS" && c_l_n->val == ","){
        c_l_n = c_l_n->nxt_node; // We skip to the value
        Logger::log(LogLevel::DEBUG, "ADD 1 :", false, true);
        Logger::log(LogLevel::DEBUG, c_l_n->val, true, false);
        // NodeType2* nodo_hijo = new NodeType2;
        NodeType2* nodo_hijo = aux_ddl_tree_2(c_l_n);
        //añadimos el nodo hijo al vector de hijos del padre:
        (nodo->hijos).push_back(std::move(nodo_hijo));
        //c_l_n = c_l_n->nxt_node;// We skip to the coma
        Logger::log(LogLevel::DEBUG, "ADD 2 :", false, true);
        Logger::log(LogLevel::DEBUG, c_l_n->val, true, false);
    };

    if(c_l_n->val != ")"){
        Logger::log(LogLevel::DEBUG, "Valor del error: :", false, true);
        Logger::log(LogLevel::DEBUG, c_l_n->val, true, false);
        // First, we delete the whole task queue:
        execPlan::delete_whole_task_queue(excec_queue);
        // We delete all tehe tables from the global table dict:
        delete_all_tables_dict_only_mem();
        // Then, the other, defined, or half defined objects:
        delete nodo;
        nodo = nullptr;
        throw std::runtime_error("ERROR: Schema columns definition was never closed with ')'");
    };
    // In case of ecountering ')' we skip it in order to advance to the next sentence
    c_l_n = c_l_n->nxt_node;
  
};


void procesar_lista_para_definir_esquema(execPlan::Queue* excec_queue, textUtils::NodeLista1*& c_l_n){
    /*
    Función especializada en traducir las instrucciones de definición de una tabla.
    Añade una instancia del tipo 'NodeType1' al contenido de la nueva tarea que crea
    de ejecución (nuevo nodo de la cola de tareas de tipo 'Queue')

    */

    if(c_l_n->val != "CREATE TABLE"){
        // First, we delete the whole task queue:
        execPlan::delete_whole_task_queue(excec_queue);
        // We delete all tehe tables from the global table dict:
        delete_all_tables_dict_only_mem();
        // Then, the other, defined, or half defined objects:
        // ...
        throw std::runtime_error("Se esperaba 'TABLE' después de 'CREATE'");
    };
    NodeType1* nodo = new NodeType1;
    // Avanzamos al siguiente nodo:
    c_l_n = c_l_n->nxt_node;

    // Esperamos el nombre:
    nodo->nombre_tabla = c_l_n->val;

    // Safeguard for not defining an already existing table:
    if((global_table_dict.find(c_l_n->val) != global_table_dict.end()) || (tableCatalog::ckeck_table(c_l_n->val))){
        // First, we delete the whole task queue:
        execPlan::delete_whole_task_queue(excec_queue);
        // We delete all tehe tables from the global table dict:
        delete_all_tables_dict_only_mem();
        // Then, the other, defined, or half defined objects:
        std::string table_name_tmp = nodo->nombre_tabla;
        throw std::runtime_error("Error: Table: " + table_name_tmp + " already exists, can not be defined again");
    };

    // Avanzamos al siguiente nodo:
    c_l_n = c_l_n->nxt_node;

    // Ahora vemos si está el alias o pasa a definir los valores:
    if(c_l_n->val != "("){
        nodo->alias = c_l_n->val;
        // En este caso debemos avanzar un nodo adicinonal:
        c_l_n = c_l_n->nxt_node;
    };

    // Avanzamos al siguiente nodo:
    //c_l_n = c_l_n->nxt_node;

    // Ahora rellenamos con los nodos hijos que corresponden con el esquema del nombre y tipo de todas las variables:
    crear_hijos_esquema_dado_padre(c_l_n, nodo, excec_queue);

    if(c_l_n->val != ";"){
        // First, we delete the whole task queue:
        execPlan::delete_whole_task_queue(excec_queue);
        // We delete all tehe tables from the global table dict:
        delete_all_tables_dict_only_mem();
        // Then, the other, defined, or half defined objects:
        delete nodo;
        nodo = nullptr;
        throw std::runtime_error("ERROR: Sentence clousure clousure ';' for schema definition not found");
    }else{
        // In case of ecountering ';' we skip it in order to advance to the next sentence
        c_l_n = c_l_n->nxt_node;
    };

    // Creamos y rellenamos el nodo de la cola
    execPlan::queueNode1* excec_queue_node = new execPlan::queueNode1;
    excec_queue_node->nodePtr = nodo;

    // ñadimos el nodo de la cola a la cola:

    excec_queue->add_node_to_queue(excec_queue_node);

    // Ahora creamos la tabla y enlazamos esa tsbla a la consulta:
    //table* tb_created = new table;
    //nodo->tb_struct = tb_created;
    //
    // Incluimosl nombre y el alias en el doccoonario:
    //extern std::unordered_map<std::string, table> global_table_dict;
    //global_table_dict[nodo->nombre_tabla] = tb_created;
    // Sólo creamos la entrada del alias si este existe:
    //if(nodo->alias != ""){
       //global_table_dict[nodo->alias] = tb_created;
    //};
    // We include the table in the schema:
    tableCatalog::register_table(nodo->nombre_tabla);
    return;
};


void insert_values_add_columns_to_node(NodeType3* nodo, textUtils::NodeLista1*& c_l_n){
    nodo->columnas.reserve(128);
    // First iteration will be out of the loop:
    (nodo->columnas).push_back(std::move(c_l_n->val));
    c_l_n = c_l_n->nxt_node;

    while(c_l_n->val == ","){
        //if(c_l_n->val != ","){
            //añadimos las columnas
        c_l_n = c_l_n->nxt_node;
        (nodo->columnas).push_back(std::move(c_l_n->val));
        //};
        c_l_n = c_l_n->nxt_node;
   };
};


void aux_iterative_value_filler(textUtils::NodeLista1*& c_l_n, std::vector<std::string>& vector_fila){

    /*
    Function destined to add a single row to the data
    */

    // First value lacks preceeding coma, thus we keep it out of the loop:
    vector_fila.push_back(std::move(c_l_n->val));
    c_l_n = c_l_n->nxt_node; // We skip to the coma

    while(c_l_n->val == "," && c_l_n->val != "EOS"){
        /*if(c_l_n->val != ","){
            // std::cout<<"Insertamos el valor: ";
            Logger::log(LogLevel::DEBUG, "Insertamos el valor: ", false, true);
            // std::cout<<c_l_n->val<<" ";
            Logger::log(LogLevel::DEBUG, c_l_n->val, false, false);
            Logger::log(LogLevel::DEBUG, " ", false, false);
            // std::cout<<"en el vector"<<std::endl;
            Logger::log(LogLevel::DEBUG, "en el vector", true, false);
            vector_fila.push_back(std::move(c_l_n->val));
        };
        // Avanzamos en el bucle:
        c_l_n = c_l_n->nxt_node;*/

        // New code:
        c_l_n = c_l_n->nxt_node; // We skip to the value
        Logger::log(LogLevel::DEBUG, "Valor que añadimos: ", false, true);
        Logger::log(LogLevel::DEBUG, c_l_n->val, true, false);
        vector_fila.push_back(std::move(c_l_n->val));
        // Reglardless of the next node value, we skip one node:
        c_l_n = c_l_n->nxt_node; // We skip to the coma

    };


};



void insert_row_values_in_node(NodeType3* nodo, textUtils::NodeLista1*& c_l_n, execPlan::Queue* excec_queue){

    // Reerving rows inn odo->filas to evade eraly memmory relocations of the array due to insuficient contiguous directions
    nodo->filas.reserve(128);


    // First iteration out of the loop:
    if(c_l_n->val != "("){
        Logger::log(LogLevel::DEBUG, "Valor del error :", false, true);
        Logger::log(LogLevel::DEBUG, c_l_n->val, true, false);
        // First, we delete the whole task queue:
        execPlan::delete_whole_task_queue(excec_queue);
        // We delete all tehe tables from the global table dict:
        delete_all_tables_dict_only_mem();
        // Then, the other, defined, or half defined objects:
        delete nodo;
        nodo = nullptr;
        throw std::runtime_error("ERROR: Parenthesis was never oppened in row addition");
    };
    c_l_n = c_l_n->nxt_node; // We skip '('
    Logger::log(LogLevel::DEBUG, "VAL 1 :", false, true);
    Logger::log(LogLevel::DEBUG, c_l_n->val, true, false);
    // Declaramos un nuevo vector:
    std::vector<std::string> vector_fila;
    // Llamamos a la función auxiliar
    Logger::log(LogLevel::DEBUG, "Entramos en donde se insertan los valores: ");

    aux_iterative_value_filler(c_l_n, vector_fila);
    // std::cout<<"Insertamos el vector en el vector de vectores:"<<std::endl;
    Logger::log(LogLevel::DEBUG, "Insertamos el vector en el vector de vectores:");
    (nodo->filas).push_back(std::move(vector_fila));
    // std::cout<<"Ya fue insertado el vector en el vector de vectores"<<std::endl;
    Logger::log(LogLevel::DEBUG, "Ya fue insertado el vector en el vector de vectores");
    if(c_l_n->val != ")"){
        // First, we delete the whole task queue:
        execPlan::delete_whole_task_queue(excec_queue);
        // We delete all tehe tables from the global table dict:
        delete_all_tables_dict_only_mem();
        // Then, the other, defined, or half defined objects:
        delete nodo;
        nodo = nullptr;
        throw std::runtime_error("ERROR: Parenthesis was never closed in row addition");
    };
    // Avanzamos en el bucle:
    c_l_n = c_l_n->nxt_node; // We skip ')'
    // The next value should be a coma or ;
    // Lanzar posible error aquí si no es ; o coma


    while(c_l_n->val != ";" && c_l_n->val != "EOS"){
        c_l_n = c_l_n->nxt_node; // We skip the coma
        // Each iteration is a row added

        if(c_l_n->val != "("){
            Logger::log(LogLevel::DEBUG, "Valor del error :", false, true);
            Logger::log(LogLevel::DEBUG, c_l_n->val, true, false);
            // First, we delete the whole task queue:
            execPlan::delete_whole_task_queue(excec_queue);
            // We delete all tehe tables from the global table dict:
            delete_all_tables_dict_only_mem();
            // Then, the other, defined, or half defined objects:
            delete nodo;
            nodo = nullptr;
            throw std::runtime_error("ERROR: Parenthesis was never oppened in row addition");
        };
        // Avanzamos un token extra:
        c_l_n = c_l_n->nxt_node; // We skip '('
        Logger::log(LogLevel::DEBUG, "VAL 1 :", false, true);
        Logger::log(LogLevel::DEBUG, c_l_n->val, true, false);
        // Declaramos un nuevo vector:
        std::vector<std::string> vector_fila;
        // Llamamos a la función auxiliar
        Logger::log(LogLevel::DEBUG, "Entramos en donde se insertan los valores: ");
    
        aux_iterative_value_filler(c_l_n, vector_fila);
        // std::cout<<"Insertamos el vector en el vector de vectores:"<<std::endl;
        Logger::log(LogLevel::DEBUG, "Insertamos el vector en el vector de vectores:");
        (nodo->filas).push_back(std::move(vector_fila));
        // std::cout<<"Ya fue insertado el vector en el vector de vectores"<<std::endl;
        Logger::log(LogLevel::DEBUG, "Ya fue insertado el vector en el vector de vectores");
        if(c_l_n->val != ")"){
            // First, we delete the whole task queue:
            execPlan::delete_whole_task_queue(excec_queue);
            // We delete all tehe tables from the global table dict:
            delete_all_tables_dict_only_mem();
            // Then, the other, defined, or half defined objects:
            delete nodo;
            nodo = nullptr;
            throw std::runtime_error("ERROR: Parenthesis was never closed in row addition");
        };
        // Avanzamos en el bucle:
        c_l_n = c_l_n->nxt_node; // We skip ')'
        //c_l_n = c_l_n->nxt_node; // We skip the coma or straight to the ';'
        Logger::log(LogLevel::DEBUG, "VAL 2 :", false, true);
        Logger::log(LogLevel::DEBUG, c_l_n->val, true, false);
    };
    // Ot of the loop, we finish consuming the instruction whole
    if(c_l_n->val != ";"){
        // First, we delete the whole task queue:
        execPlan::delete_whole_task_queue(excec_queue);
        // We delete all tehe tables from the global table dict:
        delete_all_tables_dict_only_mem();
        // Then, the other, defined, or half defined objects:
        delete nodo;
        nodo = nullptr;
        throw std::runtime_error("ERROR: Sentence clousure ';' for row addition was not found");
    }else{
        // In case of ecountering ';' we skip it in order to advance to the next sentence
        c_l_n = c_l_n->nxt_node;
    };
    // std::cout<<"Hemos terminado de crear eo nodo de insertar valores con un valor del nodo: ";
    Logger::log(LogLevel::DEBUG, "Hemos terminado de crear eo nodo de insertar valores con un valor del nodo: ", false, true);
    // std::cout<<c_l_n->val<<std::endl;
    Logger::log(LogLevel::DEBUG, c_l_n->val, true, false);
};

// FUNCIONES PARA INSERTAR VALORES:
void procesar_lista_para_insertar_valores(execPlan::Queue* excec_queue, textUtils::NodeLista1*& c_l_n){

    if(c_l_n->val != "INSERT INTO"){
        // First, we delete the whole task queue:
        execPlan::delete_whole_task_queue(excec_queue);
        // We delete all tehe tables from the global table dict:
        delete_all_tables_dict_only_mem();
        // Then, the other, defined, or half defined objects:
        // ...
        throw std::runtime_error("Se esperaba 'INTO' después de 'INSERT'");
        return;
    };
    
    NodeType3* nodo = new NodeType3;

    // Avanzamos al siguiente nodo:
    c_l_n = c_l_n->nxt_node;

    // Ahora recibiremos el nombre de la tabla:
    nodo->nombre_tabla = c_l_n->val;

    // Avanzamos al siguiente nodo:
    c_l_n = c_l_n->nxt_node;

    // Ahora vemos si hay paréntesis o no: vemos si se definirán las columnas o no:
    if(c_l_n->val == "("){
        // Caso de que se especifiquen las columnas:
        c_l_n = c_l_n->nxt_node;
        // Ahora un pequeño bucle para ir añadiendo los nombres de las columnas a procesar:
        insert_values_add_columns_to_node(nodo, c_l_n);
        if(c_l_n->val != ")"){
            // First, we delete the whole task queue:
            execPlan::delete_whole_task_queue(excec_queue);
            // We delete all tehe tables from the global table dict:
            delete_all_tables_dict_only_mem();
            // Then, the other, defined, or half defined objects:
            delete nodo;
            nodo = nullptr;
            throw std::runtime_error("ERROR: Column definition for row addition was never closed with ')'");
        };
        c_l_n = c_l_n->nxt_node;
    };

     // Ahora viene la parte de añadir valores:
    if(c_l_n->val == "VALUES"){
        Logger::log(LogLevel::DEBUG, "VALUES DETECTADO");
        c_l_n = c_l_n->nxt_node;
    } else{
        // std::cout<<"Este es el valor del nodo que ha dado este error: ";
        Logger::log(LogLevel::DEBUG, "Este es el valor del nodo que ha dado este error: ", false, true);
        // std::cout<<c_l_n->val<<std::endl;
        Logger::log(LogLevel::DEBUG, c_l_n->val, true, false);
        // node deletion in order to evade memory leaks:
        // First, we delete the whole task queue:
        execPlan::delete_whole_task_queue(excec_queue);
        // We delete all tehe tables from the global table dict:
        delete_all_tables_dict_only_mem();
        // Then, the other, defined, or half defined objects:
        delete nodo;
        nodo = nullptr;
        throw std::runtime_error("Se esperaba 'VALUES' despues de definir las columnas");
    };

    // Ahora insertamos los valores:
    // std::cout<<"Insertamos los valores:"<<std::endl;
    Logger::log(LogLevel::DEBUG, "Insertamos los valores:");
    insert_row_values_in_node(nodo, c_l_n, excec_queue);
    // std::cout<<"SE HA SALIDO DE LA FUNCION 'insert_row_values_in_node'"<<std::endl;
    Logger::log(LogLevel::DEBUG, "SE HA SALIDO DE LA FUNCION 'insert_row_values_in_node'");

    // Creamos y rellenamos el nodo de la cola
    execPlan::queueNode1* excec_queue_node = new execPlan::queueNode1;
    excec_queue_node->nodePtr = nodo;

    // ñadimos el nodo de la cola a la cola:

    excec_queue->add_node_to_queue(excec_queue_node);

    // std::cout<<"SALIMOS DE LA FUNCION: 'procesar_lista_para_insertar_valores'"<<std::endl;
    Logger::log(LogLevel::DEBUG, "SALIMOS DE LA FUNCION: 'procesar_lista_para_insertar_valores'");
    
    return;

};

void aux_col_consulta(QueryNode*& nodo_consulta, textUtils::NodeLista1*& c_l_n){
    // Creamos el nodo del Select:
    SelectNode* nodo_seleccionar = new SelectNode;

    /*while(c_l_n->val != "FROM" && c_l_n->val != "EOS"){
        //Creamos un nuevo nodo de item:
        ItemNode nodo_item;
        nodo_item.nombre = c_l_n->val;
        (nodo_seleccionar->items).push_back(std::move(nodo_item));
        c_l_n = c_l_n->nxt_node;
        if(c_l_n->val == ","){
            c_l_n = c_l_n->nxt_node;
        };
   };*/
   //Fist value lacks preceeding coma, thus we keep it out of the loop:
   ItemNode nodo_item;
   nodo_item.nombre = c_l_n->val;
   (nodo_seleccionar->items).push_back(std::move(nodo_item));
    c_l_n = c_l_n->nxt_node; // We skip to the coma
    // While loop base on the fact that the last element won't hace a coma following it
    while(c_l_n->val == "," && c_l_n->val != "EOS"){
        c_l_n = c_l_n->nxt_node; // We skip to the value
        ItemNode nodo_item;
        nodo_item.nombre = c_l_n->val;
        (nodo_seleccionar->items).push_back(std::move(nodo_item));
        c_l_n = c_l_n->nxt_node; // We skip to the coma
    };
   // Al final dwl todo lo enoazamos cob el modo raiz de la consulta:
   nodo_consulta->nodo_select = nodo_seleccionar;
};


// PARA DEFINIR LAS CONSULTAS:
void procesar_lista_para_consulta(execPlan::Queue*& excec_queue, textUtils::NodeLista1*& c_l_n){

    // Avanzamos uno:
    Logger::log(LogLevel::DEBUG, "IINCIAMOS LA CREACION DEL ARBOL DE CONSULTA");
    
    c_l_n = c_l_n->nxt_node; // We skip 'SELECT'
    QueryNode* nodo_consulta = new QueryNode;
    FromNode* nodo_desde = nullptr;

    // Ahora vamos a un bucle para definir las columnas de la consulta:
    if(c_l_n->val != "*"){
        // Funcion auxiliar para recorrer las columnas a consultar:
        aux_col_consulta(nodo_consulta, c_l_n);

    } else{
        c_l_n = c_l_n->nxt_node;
    };


    // Ahora vemos si encontramos el from:
    if(c_l_n->val == "FROM"){
        c_l_n = c_l_n->nxt_node;
        nodo_desde = new FromNode;
        // ahora viene el nombre de la tabla:
	// Antes comprobamos si existe dicha tabla:
        Logger::log(LogLevel::DEBUG, "FROM detectado. Vemos si existe la tabla en el diccionario o no:");
        if(global_table_dict.find(c_l_n->val) == global_table_dict.end()){
            if(!tableCatalog::ckeck_table(c_l_n->val)){
                Logger::log(LogLevel::DEBUG, "NO existe la entrada en el diccionario de tablas");
                // No existe esa tabla:
                // First, we delete the whole task queue:
                execPlan::delete_whole_task_queue(excec_queue);
                // We delete all tehe tables from the global table dict:
                delete_all_tables_dict_only_mem();
                // Then, the other, defined, or half defined objects:
                delete nodo_consulta;
                nodo_consulta = nullptr;
                delete nodo_desde;
                nodo_desde = nullptr;
                throw std::runtime_error("ERROR: Query error. Table: " + c_l_n->val + " does not exist");
            };
        };
        Logger::log(LogLevel::DEBUG, "SI existe la entrada en el diccionario de tablas");	
        nodo_desde->nombre = c_l_n->val;
        nodo_consulta->nodo_from = nodo_desde;
    }else{
        // First, we delete the whole task queue:
        execPlan::delete_whole_task_queue(excec_queue);
        // We delete all tehe tables from the global table dict:
        delete_all_tables_dict_only_mem();
        // Then, the other, defined, or half defined objects:
        delete nodo_desde;
        nodo_desde = nullptr;
        delete nodo_consulta;
        nodo_consulta = nullptr;
        throw std::runtime_error("ERROR: 'FROM' missing from table query");
    };
    c_l_n = c_l_n->nxt_node;
    if(c_l_n->val != ";"){
        Logger::log(LogLevel::DEBUG, "Valor del error: ", false, true);
        Logger::log(LogLevel::DEBUG, c_l_n->val, true, false);
        // First, we delete the whole task queue:
        execPlan::delete_whole_task_queue(excec_queue);
        // We delete all tehe tables from the global table dict:
        delete_all_tables_dict_only_mem();
        // Then, the other, defined, or half defined objects:
        delete nodo_desde;
        nodo_desde = nullptr;
        delete nodo_consulta;
        nodo_consulta = nullptr;
        throw std::runtime_error("ERROR: Sentence clousure ';' for table query operation was not found");
    }else{
        // In case of ecountering ';' we skip it in order to advance to the next sentence
        c_l_n = c_l_n->nxt_node;
    };

    // Creamos y rellenamos el nodo de la cola
    execPlan::queueNode1* excec_queue_node = new execPlan::queueNode1;
    excec_queue_node->nodePtr = nodo_consulta;

    // ñadimos el nodo de la cola a la cola:

    excec_queue->add_node_to_queue(excec_queue_node);

    Logger::log(LogLevel::DEBUG, "Se ha adicionado la consulta al arbol");

    return;
};

// Para eliminar tablas:
void add_drop_table_node_to_queue(execPlan::Queue*& excec_queue, textUtils::NodeLista1*& c_l_n){

    // We skip to the suposed name of the table:
    c_l_n = c_l_n->nxt_node;
    std::string table_nombre = c_l_n->val;

    // In contrast with table creation, table deletion CAN look it up in the global table dictionary:
    std::unordered_map<std::string, table*>::iterator item_pair= global_table_dict.find(table_nombre);
    if(item_pair == global_table_dict.end()){
        // If the table is yet not created, we search it among the defined yet not created tables:
        if(!tableCatalog::ckeck_table(table_nombre)){
            Logger::log(LogLevel::ERROR, "ERROR: La tabla: ", false, true);
            Logger::log(LogLevel::ERROR, table_nombre, false,false);
            Logger::log(LogLevel::ERROR," .No se puede eliminar. NO existe previamente ni ha sido definida todavía");
            throw std::runtime_error("DROP TABLE ERROR: Table '" + table_nombre + "' does not exist.");

        };
    };

    // From here we assume the name was valid, thus we skip one node farther:
    c_l_n = c_l_n->nxt_node;

    if(c_l_n->val != ";"){
        // First, we delete the whole task queue:
        execPlan::delete_whole_task_queue(excec_queue);
        // We delete all the tables from the global table dict:
        delete_all_tables_dict_only_mem();
        // Then, the other, defined, or half defined objects:
        // ...
        throw std::runtime_error("ERROR: Sentence clousure ';' for table deletion operation was not found");
    }else{
        // In case of ecountering ';' we skip it in order to advance to the next sentence
        c_l_n = c_l_n->nxt_node;
    };
    // En caso de existir la clave procedemos a crear un nodo de elimonacion de tabla:
    DropTableNode* nodo_drop = new DropTableNode;
    nodo_drop->nombre_tabla = table_nombre;

    // Creamos y rellenamos el nodo de la cola
    execPlan::queueNode1* excec_queue_node = new execPlan::queueNode1;
    excec_queue_node->nodePtr = nodo_drop;

    // ñadimos el nodo de la cola a la cola:                                
    excec_queue->add_node_to_queue(excec_queue_node);

    // We delete the table from the table catalog:
    tableCatalog::delete_table(table_nombre);

};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
execPlan::Queue* procesar_lista_tokens(textUtils::simpleLinkedList*& lista){
    /*
    Función que toma la lista de tokens y devuelve una cola de ejecución.
    Esta cola luego se ejecutará siendo recorrida y ejecutando cada uno de sus nodos.

    */

    // Obtenemos le primer nodo de la lista:
    textUtils::NodeLista1* c_l_n = lista->head;

    // Creamos la cola:
    execPlan::Queue* excec_queue = new execPlan::Queue;

    while(c_l_n->val != "EOS"){

        // std::cout<<"Valor del token: ";
        Logger::log(LogLevel::DEBUG, "Valor del token: ", false, true);
        Logger::log(LogLevel::DEBUG, c_l_n->val, true, false);


        if(c_l_n->val == "CREATE TABLE"){
            Logger::log(LogLevel::DEBUG, "Procedemos a definir el esquema:");
            procesar_lista_para_definir_esquema(excec_queue, c_l_n);
        } else if(c_l_n->val == "INSERT INTO"){
            Logger::log(LogLevel::DEBUG, "Procedemos a insertar valores:");
            procesar_lista_para_insertar_valores(excec_queue, c_l_n);
        } else if(c_l_n->val == "SELECT"){
            procesar_lista_para_consulta(excec_queue, c_l_n);
        } else if(c_l_n->val == "DROP TABLE"){
            Logger::log(LogLevel::DEBUG, "<<<<<ELIMINACION DE LA TABLA >>>>>>: " + c_l_n->nxt_node->val);
            add_drop_table_node_to_queue(excec_queue, c_l_n); //Aqui no se elimina lq tabla
            Logger::log(LogLevel::DEBUG, "<<<<<TABLA PLANEADA PARA ELIMINAR >>>>>>: ");
        /*/} else if(c_l_n->val == ";"){
            // Ahora, si encontramos un ";" avanzamos un nodo adicinoal en la lista de tokens:
            Logger::log(LogLevel::DEBUG, "saltamos los valores ';' :");
            c_l_n = c_l_n->nxt_node;*/
        } else{
            // Avanzamos si o si al siguienre nodo:
            Logger::log(LogLevel::ERROR, "ERROR: Comando no reconocido: " + c_l_n->val);
            c_l_n = c_l_n->nxt_node;
        };
    };
    return excec_queue;
};
