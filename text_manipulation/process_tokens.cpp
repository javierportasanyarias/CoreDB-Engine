#include "execution.h"
#include "execution_planning.h"
#include "node_for_trees.h"
#include "text_utils.h"
#include "process_tokens.h"

// Forward declaration:
struct table;


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Auxiliar class (tableCatalog) for keeping track of the tables created o already present:


void tableCatalog::register_table(std::string name){
    table_catalog.insert(name);
};

bool tableCatalog::ckeck_table(std::string name){
    return table_catalog.find(name) != table_catalog.end();
};

void tableCatalog::delete_table(std::string name){
    // Only if the table exists, we erase it:
    if(tableCatalog::ckeck_table(name)){
        table_catalog.erase(name);
    };
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AUXILIARY FUNCTION DEFINITION:


NodeType2* aux_ddl_tree_2(textUtils::NodeList1*& c_l_n){

    /*
    Auxiliary function fro creating and filling 'NodeType2' nodes.
    */

    Logger::flush(LogLevel::DEBUG);
    Logger::log(LogLevel::DEBUG, "+++++++++++++++++++++++++++++++");

    NodeType2* node = new NodeType2;

    // Adding the value:
    Logger::log(LogLevel::DEBUG, "Field's name value: ", false, true);
    Logger::log(LogLevel::DEBUG, c_l_n->val, true, false);
    node->field_name = c_l_n->val;
    c_l_n = c_l_n->nxt_node;

    // Safeguard for no frther nodes in token list:
    if(!c_l_n) return node;

    Logger::log(LogLevel::DEBUG, "Field's type value: ", false, true);
    Logger::log(LogLevel::DEBUG, c_l_n->val, true, false);
    // Next node must be data type:
    node->type = c_l_n->val;
    c_l_n = c_l_n->nxt_node;

    // Safeguard for no frther nodes in token list:
    if(!c_l_n) return node;

    if(c_l_n->val == "PRIMARY KEY"){
        Logger::log(LogLevel::DEBUG, "Is primary key");
        node->is_primary = true;
    };
    Logger::log(LogLevel::DEBUG, "+++++++++++++++++++++++++++++++");
    Logger::flush(LogLevel::DEBUG);
    return node;

};


// FUNCTIONS FOR SCHEMA DEFINITION:
void create_children_by_parent(textUtils::NodeList1*& c_l_n, NodeType1* node_ptr, execPlan::Queue* excec_queue){

    /*
    Function destined to create the input 'NodeType1' child nodes.
    Each child node 'NodeType2' corresponds to a table's field/column.
    */

    if(c_l_n->val != "("){
        // First, we delete the whole task queue:
        execPlan::delete_whole_task_queue(excec_queue);
        // We delete all the tables from the global table dict:
        delete_all_tables_dict_only_mem();
        // Then, the other, defined, or half defined objects:
        delete node_ptr;
        node_ptr = nullptr;
        throw std::runtime_error("ERROR: Schema columns definition was never oppened with '('");
    };
    c_l_n = c_l_n->nxt_node;
    node_ptr->children.reserve(128);


    NodeType2* child_node = aux_ddl_tree_2(c_l_n);
    (node_ptr->children).push_back(std::move(child_node));
    c_l_n = c_l_n->nxt_node; 
    // We skip to the coma:
    Logger::log(LogLevel::DEBUG, "ADD 0 :", false, true);
    Logger::log(LogLevel::DEBUG, c_l_n->val, true, false);

    while(c_l_n->val != "EOS" && c_l_n->val == ","){
        c_l_n = c_l_n->nxt_node; 
        // We skip to the value:
        Logger::log(LogLevel::DEBUG, "ADD 1 :", false, true);
        Logger::log(LogLevel::DEBUG, c_l_n->val, true, false);

        NodeType2* child_node = aux_ddl_tree_2(c_l_n);

        (node_ptr->children).push_back(std::move(child_node));

        Logger::log(LogLevel::DEBUG, "ADD 2 :", false, true);
        Logger::log(LogLevel::DEBUG, c_l_n->val, true, false);
    };

    if(c_l_n->val != ")"){
        Logger::log(LogLevel::DEBUG, "Error value :", false, true);
        Logger::log(LogLevel::DEBUG, c_l_n->val, true, false);
        // First, we delete the whole task queue:
        execPlan::delete_whole_task_queue(excec_queue);
        // We delete all the tables from the global table dict:
        delete_all_tables_dict_only_mem();
        // Then, the other, defined, or half defined objects:
        delete node_ptr;
        node_ptr = nullptr;
        throw std::runtime_error("ERROR: Schema columns definition was never closed with ')'");
    };
    // In case of ecountering ')', we skip it in order to advance to the next sentence:
    c_l_n = c_l_n->nxt_node;
  
};


void process_list_to_define_schema(execPlan::Queue* excec_queue, textUtils::NodeList1*& c_l_n){

    /*
    Function specialized in trannslating table definition instructions into a 'NodeType1' node,
    and adding it to the task queue.
    */


    Logger::log(LogLevel::DEBUG, "Inside the 'process_list_to_define_schema' function");
    if(c_l_n->val != "CREATE TABLE"){
        Logger::log(LogLevel::DEBUG, "There is no 'CREATE TABLE' clause");
        // First, we delete the whole task queue:
        execPlan::delete_whole_task_queue(excec_queue);
        // We delete all the tables from the global table dict:
        delete_all_tables_dict_only_mem();
        // There are no objects left to delete
        throw std::runtime_error("'TABLE' was expected after 'CREATE' clause");
    };
    Logger::log(LogLevel::DEBUG, "There IS 'CREATE TABLE' clause");
    NodeType1* node = new NodeType1;
    // We skip to the next node:
    c_l_n = c_l_n->nxt_node;

    Logger::log(LogLevel::DEBUG, "Accessing table's name:");
    node->table_name = c_l_n->val;

    Logger::log(LogLevel::DEBUG, "Checking if newly defined table already exists:");
    // Safeguard for not defining an already existing table:
    if((global_table_dict.find(c_l_n->val) != global_table_dict.end()) || (tableCatalog::ckeck_table(c_l_n->val))){
        Logger::log(LogLevel::DEBUG, "Fail!, table already exists");
        // First, we delete the whole task queue:
        execPlan::delete_whole_task_queue(excec_queue);
        // We delete all the tables from the global table dict:
        delete_all_tables_dict_only_mem();
        // There are no objects left to delete
        std::string table_name_tmp = node->table_name;
        throw std::runtime_error("Error: Table: " + table_name_tmp + " already exists, can not be defined again");
    };

    Logger::log(LogLevel::DEBUG, "Defined table has not yet been defined");
 
    // Advancing to the next node:
    c_l_n = c_l_n->nxt_node;

    // Declaring node alias, if such a value has been declared by the user:
    if(c_l_n->val != "("){
        node->alias = c_l_n->val;
        // An additional node must be skipped in this case:
        c_l_n = c_l_n->nxt_node;
    };

    // Now we fill all child nodes corresponding to the name and type of the table's columns:
    create_children_by_parent(c_l_n, node, excec_queue);

    if(c_l_n->val != ";"){
        // First, we delete the whole task queue:
        execPlan::delete_whole_task_queue(excec_queue);
        // We delete all the tables from the global table dict:
        delete_all_tables_dict_only_mem();
        // Then, the other, defined, or half defined objects:
        delete node;
        node = nullptr;
        throw std::runtime_error("ERROR: Sentence closure ';' for schema definition not found");
    }else{
        // In case of ecountering ';' we skip it in order to advance to the next sentence
        c_l_n = c_l_n->nxt_node;
    };

    // Creating and filling the task queue node:
    execPlan::queueNode1* excec_queue_node = new execPlan::queueNode1;
    excec_queue_node->nodePtr = node;

    // We add the task to the queue:
    excec_queue->add_node_to_queue(excec_queue_node);

    // Registering the table's name in the table registry:
    tableCatalog::register_table(node->table_name);
    return;

};


void insert_values_add_columns_to_node(NodeType3* node_ptr, textUtils::NodeList1*& c_l_n){
    node_ptr->columns.reserve(128);
    // First iteration will be out of the loop:
    (node_ptr->columns).push_back(std::move(c_l_n->val));
    c_l_n = c_l_n->nxt_node;

    while(c_l_n->val == ","){
        c_l_n = c_l_n->nxt_node;
        (node_ptr->columns).push_back(std::move(c_l_n->val));
        //};
        c_l_n = c_l_n->nxt_node;
   };
};


void aux_iterative_value_filler(textUtils::NodeList1*& c_l_n, std::vector<std::string>& row_vector){

    /*
    Function destined to add a single row to the data
    */

    // First value lacks preceeding coma, thus we keep it out of the loop:
    row_vector.push_back(std::move(c_l_n->val));
    c_l_n = c_l_n->nxt_node; // We skip to the coma:

    while(c_l_n->val == "," && c_l_n->val != "EOS"){

        c_l_n = c_l_n->nxt_node; // We skip to the value
        Logger::log(LogLevel::DEBUG, "Added value: ", false, true);
        Logger::log(LogLevel::DEBUG, c_l_n->val, true, false);
        row_vector.push_back(std::move(c_l_n->val));
        // Reglardless of the next node value, we skip one node:
        c_l_n = c_l_n->nxt_node; // We skip to the coma

    };
};


void insert_row_values_in_node(NodeType3* node_ptr, textUtils::NodeList1*& c_l_n, execPlan::Queue* excec_queue){

    // Reserving rows inn node->rows to evade early memmory relocations of the array due to insuficient contiguous addresses:
    node_ptr->rows.reserve(128);

    // First iteration out of the loop:
    if(c_l_n->val != "("){
        Logger::log(LogLevel::DEBUG, "Error value:", false, true);
        Logger::log(LogLevel::DEBUG, c_l_n->val, true, false);
        // First, we delete the whole task queue:
        execPlan::delete_whole_task_queue(excec_queue);
        // We delete all the tables from the global table dict:
        delete_all_tables_dict_only_mem();
        // Then, the other, defined, or half defined objects:
        delete node_ptr;
        node_ptr = nullptr;
        throw std::runtime_error("ERROR: Parenthesis was never oppened in row addition");
    };
    c_l_n = c_l_n->nxt_node; // We skip '('
    Logger::log(LogLevel::DEBUG, "VAL 1 :", false, true);
    Logger::log(LogLevel::DEBUG, c_l_n->val, true, false);

    std::vector<std::string> row_vector;

    Logger::log(LogLevel::DEBUG, "Value insertion: ");

    aux_iterative_value_filler(c_l_n, row_vector);

    Logger::log(LogLevel::DEBUG, "Vector insertion within the vector of vectors:");
    (node_ptr->rows).push_back(std::move(row_vector));

    Logger::log(LogLevel::DEBUG, "Vector already inserted");
    if(c_l_n->val != ")"){
        // First, we delete the whole task queue:
        execPlan::delete_whole_task_queue(excec_queue);
        // We delete all the tables from the global table dict:
        delete_all_tables_dict_only_mem();
        // Then, the other, defined, or half defined objects:
        delete node_ptr;
        node_ptr = nullptr;
        throw std::runtime_error("ERROR: Parenthesis was never closed in row addition");
    };

    // Advancing to the next node:
    c_l_n = c_l_n->nxt_node; // We skip ')'

    while(c_l_n->val != ";" && c_l_n->val != "EOS"){
        c_l_n = c_l_n->nxt_node; // We skip the coma
        // Each iteration corresponds with an added row

        if(c_l_n->val != "("){
            Logger::log(LogLevel::DEBUG, "Error value :", false, true);
            Logger::log(LogLevel::DEBUG, c_l_n->val, true, false);
            // First, we delete the whole task queue:
            execPlan::delete_whole_task_queue(excec_queue);
            // We delete all the tables from the global table dict:
            delete_all_tables_dict_only_mem();
            // Then, the other, defined, or half defined objects:
            delete node_ptr;
            node_ptr = nullptr;
            throw std::runtime_error("ERROR: Parenthesis was never oppened in row addition");
        };

        c_l_n = c_l_n->nxt_node; // We skip '('
        Logger::log(LogLevel::DEBUG, "VAL 1 :", false, true);
        Logger::log(LogLevel::DEBUG, c_l_n->val, true, false);

        std::vector<std::string> row_vector;

        Logger::log(LogLevel::DEBUG, "Value insertion: ");
    
        aux_iterative_value_filler(c_l_n, row_vector);

        Logger::log(LogLevel::DEBUG, "Vector insertion within the vector of vectors:");
        (node_ptr->rows).push_back(std::move(row_vector));

        Logger::log(LogLevel::DEBUG, "Vector already inserted");
        if(c_l_n->val != ")"){
            // First, we delete the whole task queue:
            execPlan::delete_whole_task_queue(excec_queue);
            // We delete all the tables from the global table dict:
            delete_all_tables_dict_only_mem();
            // Then, the other, defined, or half defined objects:
            delete node_ptr;
            node_ptr = nullptr;
            throw std::runtime_error("ERROR: Parenthesis was never closed in row addition");
        };
        // Advancing to the next node:
        c_l_n = c_l_n->nxt_node; // We skip ')'
        Logger::log(LogLevel::DEBUG, "VAL 2 :", false, true);
        Logger::log(LogLevel::DEBUG, c_l_n->val, true, false);
    };
    // Ot of the loop, we finish consuming the whole instruction:
    if(c_l_n->val != ";"){
        // First, we delete the whole task queue:
        execPlan::delete_whole_task_queue(excec_queue);
        // We delete all the tables from the global table dict:
        delete_all_tables_dict_only_mem();
        // Then, the other, defined, or half defined objects:
        delete node_ptr;
        node_ptr = nullptr;
        throw std::runtime_error("ERROR: Sentence closure ';' for row addition was not found");
    }else{
        // In case of ecountering ';' we skip it in order to advance to the next sentence
        c_l_n = c_l_n->nxt_node;
    };
    Logger::log(LogLevel::DEBUG, "Node for value insertion has successfully been creadted. Value: ", false, true);
    Logger::log(LogLevel::DEBUG, c_l_n->val, true, false);
};

// FUNCTIONS FOR VALUE INSERTION:
void process_list_to_insert_values(execPlan::Queue* excec_queue, textUtils::NodeList1*& c_l_n){

    if(c_l_n->val != "INSERT INTO"){
        // First, we delete the whole task queue:
        execPlan::delete_whole_task_queue(excec_queue);
        // We delete all the tables from the global table dict:
        delete_all_tables_dict_only_mem();
        // There are no objects left to delete
        throw std::runtime_error("'INTO' was expected after 'INSERT' clause");
        return;
    };
    
    NodeType3* node = new NodeType3;

    c_l_n = c_l_n->nxt_node;

    node->table_name = c_l_n->val;

    c_l_n = c_l_n->nxt_node;

    if(c_l_n->val == "("){
        // Column's names are defined
        c_l_n = c_l_n->nxt_node;
        insert_values_add_columns_to_node(node, c_l_n);
        if(c_l_n->val != ")"){
            // First, we delete the whole task queue:
            execPlan::delete_whole_task_queue(excec_queue);
            // We delete all the tables from the global table dict:
            delete_all_tables_dict_only_mem();
            // Then, the other, defined, or half defined objects:
            delete node;
            node = nullptr;
            throw std::runtime_error("ERROR: Column definition for row addition was never closed with ')'");
        };
        c_l_n = c_l_n->nxt_node;
    };

     // Column values addition:
    if(c_l_n->val == "VALUES"){
        Logger::log(LogLevel::DEBUG, "VALUES DETECTED");
        c_l_n = c_l_n->nxt_node;
    } else{
        Logger::log(LogLevel::DEBUG, "Node value that originated the error: ", false, true);
        Logger::log(LogLevel::DEBUG, c_l_n->val, true, false);
        // Node deletion in order to evade memory leaks:
        // First, we delete the whole task queue:
        execPlan::delete_whole_task_queue(excec_queue);
        // We delete all the tables from the global table dict:
        delete_all_tables_dict_only_mem();
        // Then, the other, defined, or half defined objects:
        delete node;
        node = nullptr;
        throw std::runtime_error("'VALUES' was expected after column name/s definition");
    };

    // Value insertion:
    Logger::log(LogLevel::DEBUG, "Value insertion:");
    insert_row_values_in_node(node, c_l_n, excec_queue);

    // Creating and filling the task queue node:
    execPlan::queueNode1* excec_queue_node = new execPlan::queueNode1;
    excec_queue_node->nodePtr = node;

    // Adding task to the queue:
    excec_queue->add_node_to_queue(excec_queue_node);
    
    return;

};

void aux_col_query(QueryNode*& query_node, textUtils::NodeList1*& c_l_n){

    SelectNode* select_node = new SelectNode;

   //Fist value lacks preceeding coma, thus we keep it out of the loop:
   ItemNode node_item;
   node_item.name = c_l_n->val;
   (select_node->items).push_back(std::move(node_item));
    c_l_n = c_l_n->nxt_node; // We skip to the coma
    // While loop based on the fact that the last element won't hace a coma following it:
    while(c_l_n->val == "," && c_l_n->val != "EOS"){
        c_l_n = c_l_n->nxt_node; // We skip to the value
        ItemNode node_item;
        node_item.name = c_l_n->val;
        (select_node->items).push_back(std::move(node_item));
        c_l_n = c_l_n->nxt_node; // We skip to the coma
    };
   // Linking column selection with query node:
   query_node->select_node = select_node;
};


void process_list_for_selection(execPlan::Queue*& excec_queue, textUtils::NodeList1*& c_l_n){

    Logger::log(LogLevel::DEBUG, "Query tree creation has been started");
    
    c_l_n = c_l_n->nxt_node; // We skip 'SELECT'
    QueryNode* query_node_ptr = new QueryNode;
    FromNode* from_node_ptr = nullptr;

    // All columns selection check:
    if(c_l_n->val != "*"){
        // Aux function to traverse all the table's columns:
        aux_col_query(query_node_ptr, c_l_n);

    } else{
        c_l_n = c_l_n->nxt_node;
    };


    if(c_l_n->val == "FROM"){
        // 'FROM' token has ben encountered:
        c_l_n = c_l_n->nxt_node;
        from_node_ptr = new FromNode;
        Logger::log(LogLevel::DEBUG, "FROM deteted. Checking if table is present in the global dictionary:");
        if(global_table_dict.find(c_l_n->val) == global_table_dict.end()){
            if(!tableCatalog::ckeck_table(c_l_n->val)){
                Logger::log(LogLevel::DEBUG, "NO entry with that name was found within the global table registry");
                // Table does not exist
                // First, we delete the whole task queue:
                execPlan::delete_whole_task_queue(excec_queue);
                // Deleting all the tables from the global table dict:
                delete_all_tables_dict_only_mem();
                // Then, the other, defined, or half defined objects:
                delete query_node_ptr;
                query_node_ptr = nullptr;
                delete from_node_ptr;
                from_node_ptr = nullptr;
                throw std::runtime_error("ERROR: Query error. Table: " + c_l_n->val + " does not exist");
            };
        };
        Logger::log(LogLevel::DEBUG, "Table name does exist within the global dictionary");	
        from_node_ptr->name = c_l_n->val;
        query_node_ptr->from_node = from_node_ptr;
    }else{
        // First, we delete the whole task queue:
        execPlan::delete_whole_task_queue(excec_queue);
        // Deleting all the tables from the global table dict:
        delete_all_tables_dict_only_mem();
        // Then, the other, defined, or half defined objects:
        delete from_node_ptr;
        from_node_ptr = nullptr;
        delete query_node_ptr;
        query_node_ptr = nullptr;
        throw std::runtime_error("ERROR: 'FROM' missing from table query");
    };
    c_l_n = c_l_n->nxt_node;
    if(c_l_n->val != ";"){
        Logger::log(LogLevel::DEBUG, "Error value: ", false, true);
        Logger::log(LogLevel::DEBUG, c_l_n->val, true, false);
        // First, we delete the whole task queue:
        execPlan::delete_whole_task_queue(excec_queue);
        // Deleting all the tables from the global table dict:
        delete_all_tables_dict_only_mem();
        // Then, the other, defined, or half defined objects:
        delete from_node_ptr;
        from_node_ptr = nullptr;
        delete query_node_ptr;
        query_node_ptr = nullptr;
        throw std::runtime_error("ERROR: Sentence closure ';' for table query operation was not found");
    }else{
        // In case of ecountering ';' we skip it in order to advance to the next sentence
        c_l_n = c_l_n->nxt_node;
    };

    // Creating and filling the task queue node:
    execPlan::queueNode1* excec_queue_node = new execPlan::queueNode1;
    excec_queue_node->nodePtr = query_node_ptr;

    // Adding the task to the queue:
    excec_queue->add_node_to_queue(excec_queue_node);

    Logger::log(LogLevel::DEBUG, "Query has been added to the task queue");

    return;
};



void add_drop_table_node_to_queue(execPlan::Queue*& excec_queue, textUtils::NodeList1*& c_l_n){

    /*
    Function destined to create a 'DropTableNode' node. This will be used for table erasure later on
    during instructions execution.
    */

    // We skip to the suposed name of the table:
    c_l_n = c_l_n->nxt_node;
    std::string table_name = c_l_n->val;

    // In contrast with table creation, table deletion CAN look it up in the global table dictionary:
    std::unordered_map<std::string, table*>::iterator item_pair= global_table_dict.find(table_name);
    if(item_pair == global_table_dict.end()){
        // If the table is yet not created, we search it among the defined yet not created tables:
        if(!tableCatalog::ckeck_table(table_name)){
            Logger::log(LogLevel::ERROR, "ERROR: Table: ", false, true);
            Logger::log(LogLevel::ERROR, table_name, false,false);
            Logger::log(LogLevel::ERROR," .Can not be deleted. It does not exist yet, hit has not been defined as of this moment");
            throw std::runtime_error("DROP TABLE ERROR: Table '" + table_name + "' does not exist.");

        };
    };

    // From here we assume the name was valid, thus we skip one node farther:
    c_l_n = c_l_n->nxt_node;

    if(c_l_n->val != ";"){
        // First, we delete the whole task queue:
        execPlan::delete_whole_task_queue(excec_queue);
        // Deleting all the tables from the global table dict:
        delete_all_tables_dict_only_mem();
        // There are no objects left to delete
        throw std::runtime_error("ERROR: Sentence closure ';' for table deletion operation was not found");
    }else{
        // In case of ecountering ';' we skip it in order to advance to the next sentence
        c_l_n = c_l_n->nxt_node;
    };
    // Proceeding to table deletion:
    DropTableNode* drop_node = new DropTableNode;
    drop_node->table_name = table_name;

    // Creating and filling the task queue node:
    execPlan::queueNode1* excec_queue_node = new execPlan::queueNode1;
    excec_queue_node->nodePtr = drop_node;

    // Adding the task to the queue:                               
    excec_queue->add_node_to_queue(excec_queue_node);

    // Deleting table from the table catalog:
    tableCatalog::delete_table(table_name);

};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
execPlan::Queue* process_token_list(textUtils::simpleLinkedList*& list_ptr){

    /*
    Main function for constructing a task queue containing all the execution trees.
    This queue then will be executed in an orderly manner, running each of the node's
    execution trees within.
    */

    textUtils::NodeList1* c_l_n = list_ptr->head;
    execPlan::Queue* excec_queue = new execPlan::Queue;

    while(c_l_n->val != "EOS"){

        Logger::log(LogLevel::DEBUG, "Token value: ", false, true);
        Logger::log(LogLevel::DEBUG, c_l_n->val, true, false);


        if(c_l_n->val == "CREATE TABLE"){
            Logger::log(LogLevel::DEBUG, "Schema definition:");
            process_list_to_define_schema(excec_queue, c_l_n);
        } else if(c_l_n->val == "INSERT INTO"){
            Logger::log(LogLevel::DEBUG, "Value insertion::");
            process_list_to_insert_values(excec_queue, c_l_n);
        } else if(c_l_n->val == "SELECT"){
            process_list_for_selection(excec_queue, c_l_n);
        } else if(c_l_n->val == "DROP TABLE"){
            Logger::log(LogLevel::DEBUG, "<<<<< TABLE DELETION >>>>>>: " + c_l_n->nxt_node->val);
            add_drop_table_node_to_queue(excec_queue, c_l_n); // WARINING: Not the actual table deletion.
            Logger::log(LogLevel::DEBUG, "<<<<< TABLE SCHEDULED FOR DELETION >>>>>>: ");
        } else{
            // Skip to the next node:
            Logger::log(LogLevel::ERROR, "ERROR: Command not recognized: " + c_l_n->val);
            c_l_n = c_l_n->nxt_node;
        };
    };
    return excec_queue;
};
