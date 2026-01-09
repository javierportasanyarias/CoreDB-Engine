#include <iostream>
#include <vector>
// #include "data_structure/data_struct.h"
#include "data_struct.h"
#include <unordered_map>
// #include "globals/globals.h"
#include "globals.h"
#include <variant>
#include "execution.h"
#include "logging.h"


///////////////////////////////////////////////////////////////////////
///
///
// Tipos de datos admitidos en el esquema:
dataType transform_str_to_datatype(const std::string& input){

      if (input == "INT"){
         return dataType::INT;
      } else if(input == "FLOAT"){
         return dataType::FLOAT;
      } else if(input == "BOOL"){
         return dataType::BOOL;
      } else if(input == "STRING"){
         return dataType::STRING;
      } else{
         return dataType::UNKNOWN;
      };
};

////////////////////////////////////////////////////////////////////////////////////////
/// PARA RELLENAR LAS TABLAS:


void fill_table_with_values_v4(NodeType3* nodo_ptr) {
    // Comprobamos que la tabla existe en el diccionario global
    auto it = global_table_dict.find(nodo_ptr->nombre_tabla);
    if (it == global_table_dict.end()) {
        throw std::runtime_error("No se ha encontrado la tabla en el diccionario global de tablas");
    };

    // Recuperamos el puntero a la tabla
    table* tb_recup = it->second;
    if (!tb_recup) {
        throw std::runtime_error("El puntero de la tabla es nulo");
    };

    // Comprobamos que los metadatos existen
    if (!tb_recup->metadata_ptr) {
        throw std::runtime_error("La tabla existe pero no tiene metadatos inicializados");
    };

    // Inicializamos la parte de datos si es necesario
    if (!tb_recup->data_ptr) {
        tb_recup->data_ptr = new table_data;
    };

    //std::cout << "Tabla encontrada, pasamos a insertar valores:\n";
    Logger::log(LogLevel::DEBUG, "Tabla encontrada, pasamos a insertar valores:\n", false);

    // Definimos la lista de columnas para iterar

   const std::vector<std::string>& columnas_list_bucle = nodo_ptr->columnas.empty() ? tb_recup->metadata_ptr->column_names : nodo_ptr->columnas;


    //std::cout << "BUCLE DE INSERCION:\n";
    Logger::log(LogLevel::DEBUG, "BUCLE DE INSERCION:\n", false);

    // Iteramos por las filas de datos a insertar
    for (int i = 0; i < nodo_ptr->filas.size(); i++) {
        const std::vector<std::string>& fila = nodo_ptr->filas[i];
        for (int j = 0; j < fila.size(); j++) {
            Logger::log(LogLevel::DEBUG, "fila ", false);
            Logger::log(LogLevel::DEBUG, i + 1, false, false);
            Logger::log(LogLevel::DEBUG, " columna Num ", false, false);
            Logger::log(LogLevel::DEBUG, j + 1, true, false);
            // Insertamos directamente en la tabla del diccionario
            tb_recup->data_ptr->columns[columnas_list_bucle[j]].push_back(std::move(fila[j]));
        };
    };
};

////////////////////////////////////////////////////////////////////////////////////////
/// PARA RELLENAR METADATOS:

void recursive_metadata_fill_lv2(NodeType2* nodo_ptr, table* tb_created){
   if(!nodo_ptr){
      return;
   };
   // Rellenamos los datos del campo a tratar:
   auto& metadata = *tb_created->metadata_ptr;
   (metadata.column_names).push_back(std::move(nodo_ptr->name_campo));
  (metadata.column_types).push_back(transform_str_to_datatype(nodo_ptr->tipo));
   if(nodo_ptr->is_primary){
      (metadata.primary_list).push_back(true);
   };
   // Estos nodos no rienen hijos
   return;
};

void recursive_metadata_fill_lv1(NodeType1* nodo_ptr){

   // Creamos un struct de la tabla:
   table* tb_created = global_table_dict[nodo_ptr->nombre_tabla];
   if(!nodo_ptr){
      return;
   };

   tb_created->metadata_ptr = new table_metadata;

   // Declaramos el nombre de la tabla:
   ((*(tb_created->metadata_ptr)).name) = nodo_ptr->nombre_tabla;
   // Recorremos los hijos:N_
   for(int i = 0; i<(*nodo_ptr).hijos.size(); i++){
      recursive_metadata_fill_lv2((nodo_ptr->hijos)[i], tb_created);
   };
   //global_table_dict[nodo_ptr->nombre_tabla] = tb_created;
   return;
};


///////////////////////////////////////////////////////////////
// MOSTRAR TABLA:

// Función auxiliar que imprime los valores de la tabla (TODOS LOS VALORES):
void aux_table_values_print(const auto& columns_buffer, const std::vector<std::string>& col_list, int n_filas) {
    for (int j = 0; j < n_filas; j++) {
        std::string fila; // acumulamos toda la fila en un string
        for (int i = 0; i < col_list.size(); i++) {
            auto& col = columns_buffer.at(col_list[i]);
            auto& cell = col[j];

            // Convertimos el valor a string usando std::visit
            std::visit([&fila](auto&& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    fila += " | " + val;
                } else if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
                    fila += " | " + std::to_string(val);
                } else {
                    // Ignoramos otros tipos como bool
                    fila += " | "; // opcional: dejar columna vacía
                }
            }, cell);
        }

        fila += " |"; // final de fila
        Logger::log(LogLevel::OUTPUT, fila, false); // imprimimos la fila sin flush automático
        Logger::flush();
    }
};



// Función auxiliar que imprime los valores de la tabla (SE ESPECIFICÓ LISTA DE COLUMNAS EN EL SELECT):
void aux_table_values_print(const auto& columns_buffer, const std::vector<ItemNode>& col_list, int n_filas) {
    for (int j = 0; j < n_filas; j++) {
        std::string fila; // acumulamos toda la fila en un string
        for (int i = 0; i < col_list.size(); i++) {
            auto& col = columns_buffer.at(col_list[i].nombre);
            auto& cell = col[j];

            // Convertimos el valor a string usando std::visit
            std::visit([&fila](auto&& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    fila += " | " + val;
                } else if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
                    fila += " | " + std::to_string(val);
                } else {
                    // Ignoramos bool u otros tipos no deseados
                    fila += " | ";
                }
            }, cell);
        }

        fila += " |"; // final de fila
        Logger::log(LogLevel::OUTPUT, fila, false); // imprimimos la fila sin flush automático
        Logger::log(LogLevel::OUTPUT, "", false);
        Logger::flush();
    }
};



// Función auxiliar para imprimir la tabla: PARA TODAS LAS COLUMNAS:
void imprimir_tabla(const auto& columns_buffer, const std::vector<std::string>& col_list, int n_filas){
   // Imprimios los nombres de las columnas:
   for(int  i = 0; i<col_list.size(); i++){
      //std::cout<<" | "<<col_list[i];
      Logger::log(LogLevel::OUTPUT, " | ", false); // sin flush automático
      Logger::log(LogLevel::OUTPUT, col_list[i], false);
   };
   //std::cout<<" | "<<std::endl;
   Logger::log(LogLevel::OUTPUT, " | ", true);
   // Imprimimos los valores:
   aux_table_values_print(columns_buffer, col_list, n_filas);
};

// impresión de toda la tabla, pero habiendo seleccionado clumnas
void imprimir_tabla(QueryNode*& nodo_root, const auto& columns_buffer, const std::vector<ItemNode>& col_list, int n_filas){
   // Imprimios los nombres de las columnas:
       for(int i = 0; i<(nodo_root->nodo_select->items).size(); i++){
         //std::cout<<" | "<<(nodo_root->nodo_select)->items[i].nombre;
         Logger::log(LogLevel::OUTPUT, " | ", false); // sin flush automático
         Logger::log(LogLevel::OUTPUT, (nodo_root->nodo_select)->items[i].nombre, false);
       };
       //std::cout<<" | "<<std::endl;
       Logger::log(LogLevel::OUTPUT, " | ", true);
   // Imprimimos los valores:
   aux_table_values_print(columns_buffer, col_list, n_filas);
};

void mostrar_tabla_query(QueryNode* nodo_root){
    // std::cout << std::endl;
    Logger::flush();

    //recuperamos el nombre de la tabla
    std::string nombre_tabla = nodo_root->nodo_from->nombre;

    if (nodo_root->nodo_select == nullptr) {
       // La lista no existe o está vacía:
        Logger::log(LogLevel::DEBUG, "La lista de columnas esta vacia (Se ha hecho Select *)");
	
       // Guardamos en memoria valores del diccionario accedidos con regularidad:
       const std::vector<std::string>& col_list = (global_table_dict[nombre_tabla]->metadata_ptr)->column_names;
       const auto& columns_buffer = global_table_dict[nombre_tabla]->data_ptr->columns;

       int n_filas = (columns_buffer.at(col_list[0])).size();
       imprimir_tabla(columns_buffer, col_list, n_filas);
    } else {

       //Ahora imprimimos los valores:
       const std::vector<ItemNode>& col_list = (nodo_root->nodo_select->items);
       const auto& columns_buffer = global_table_dict[nombre_tabla]->data_ptr->columns;
       // Hallamos antes el numero de filas:
       int n_filas = (columns_buffer.at(col_list[0].nombre)).size();   
        Logger::log(LogLevel::DEBUG, "N filas en el else: ", false);
        Logger::log(LogLevel::DEBUG, n_filas, true, false);

       imprimir_tabla(nodo_root, columns_buffer, col_list, n_filas);
   };
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Eliminación de tablas:
void liberar_tabla(table* tb){
    if (!tb) return;
    delete tb->metadata_ptr;
    tb->metadata_ptr = nullptr;
    delete tb->data_ptr;
    tb->data_ptr = nullptr;
    delete tb;
};
