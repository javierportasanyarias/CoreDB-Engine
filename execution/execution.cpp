#include <iostream>
#include <vector>
// #include "data_structure/data_struct.h"
#include "data_struct.h"
#include <unordered_map>
// #include "globals/globals.h"
#include "globals.h"
#include <variant>
#include "execution.h"


///////////////////////////////////////////////////////////////////////
///
///
// Tipos de datos admitidos en el esquema:
dataType transform_str_to_datatype(std::string input){

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

    std::cout << "Tabla encontrada, pasamos a insertar valores:\n";

    // Definimos la lista de columnas para iterar
    std::vector<std::string> columnas_list_bucle;
    if (nodo_ptr->columnas.empty()) {
        columnas_list_bucle = tb_recup->metadata_ptr->column_names;
    } else {
        columnas_list_bucle = nodo_ptr->columnas;
    };

    std::cout << "BUCLE DE INSERCION:\n";

    // Iteramos por las filas de datos a insertar
    for (int i = 0; i < nodo_ptr->filas.size(); i++) {
        const std::vector<std::string>& fila = nodo_ptr->filas[i];
        for (int j = 0; j < fila.size(); j++) {
            std::cout << "fila " << i + 1 << " columna Num " << j + 1 << "\n";
            // Insertamos directamente en la tabla del diccionario
            tb_recup->data_ptr->columns[columnas_list_bucle[j]].push_back(fila[j]);
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
   ((*(tb_created->metadata_ptr)).column_names).push_back(nodo_ptr->name_campo);
  ((*(tb_created->metadata_ptr)).column_types).push_back(transform_str_to_datatype(nodo_ptr->tipo));
   if(nodo_ptr->is_primary){
      ((*(tb_created->metadata_ptr)).primary_list).push_back(true);
   };
   // Estos nodos no rienen hijos
   return;
};

void recursive_metadata_fill_lv1(NodeType1* nodo_ptr){

   // Creamos un struct de la tabla:
   //extern std::unordered_map<std::string, table> global_table_dict; 
   //table tb_created = global_table_dict[nodo_ptr->nombre_tabla];
   table* tb_created = global_table_dict[nodo_ptr->nombre_tabla];
   if(!nodo_ptr){
      return;
   };

   tb_created->metadata_ptr = new table_metadata;

   // Declaramos el nombre de la tabla:
   ((*(tb_created->metadata_ptr)).name) = nodo_ptr->nombre_tabla;
   // Recorremos los hijos:N_
   for(int i = 0; i<(*nodo_ptr).hijos.size(); i++){
      //std::cout<<nodo_ptr->nombre_tabla<<std::endl;
      recursive_metadata_fill_lv2((nodo_ptr->hijos)[i], tb_created);
   };
   //global_table_dict[nodo_ptr->nombre_tabla] = tb_created;
   return;
};


///////////////////////////////////////////////////////////////
// MOSTRAR TABLA:

void mostrar_consulta_v1(QueryNode*& nodo_root){
   std::cout<<std::endl;
   // Mostramos las columnas a mostrar:
   for(int i = 0; i<(nodo_root->nodo_select->items).size(); i++){

      std::cout<<"aaa"<<std::endl;
      //std::cout<<" | "<<(nodo_root->nodo_select)->items[i].nombre;

   };
   std::cout<<" | "<<std::endl;
};


void mostrar_consulta_v2(QueryNode* nodo_root){
    std::cout << std::endl;

    //recuperamos el nombre de la tabla
    std::string nombre_tabla = nodo_root->nodo_from->nombre;

    if (nodo_root->nodo_select == nullptr) {
       // La lista no existe o está vacía:
	    std::cout<<"La lista de columnas esta vacia"<<std::endl;
	
       std::vector<std::string> col_list;
       col_list = (global_table_dict[nombre_tabla]->metadata_ptr)->column_names;
       for(int i = 0; i<col_list.size(); i++){
          std::cout<<" | "<<col_list[i];
       };
       std::cout<<" | "<<std::endl;
       // Hallamos antes el numero de filas:
       int n_filas;
       n_filas = ((global_table_dict[nombre_tabla]->data_ptr->columns)[col_list[0]]).size();
       // Ahora mostramos los valores:
       //for(int i = 0; i< col_list.size(); i++){
       for(int j = 0; j < n_filas; j++){
          for(int i = 0; i< col_list.size(); i++){
		  //std::cout<<" | "<<((global_table_dict[nombre_tabla]->data_ptr->columns)[col_list[i]])[j];
	  //std::cout<<" | "<<std::endl;
	  //
	     auto& cell = (global_table_dict[nombre_tabla]
                ->data_ptr
                ->columns[col_list[i]])[j];

             std::visit([](auto&& val){
             std::cout << " | "<<val;}, cell);
          };
          std::cout<<" | "<<std::endl;
      };
    } else {
       for(int i = 0; i<(nodo_root->nodo_select->items).size(); i++){

       //std::cout<<"aaa"<<std::endl;
       std::cout<<" | "<<(nodo_root->nodo_select)->items[i].nombre;

       };
       std::cout<<" | "<<std::endl;




       //Ahora imprimimos los valores:
       std::vector<ItemNode> col_list;
       col_list = (nodo_root->nodo_select->items);
       // Hallamos antes el numero de filas:
       int n_filas;
        n_filas = ((global_table_dict[nombre_tabla]->data_ptr->columns)[col_list[0].nombre]).size();   
        std::cout<<"N filas en el else: "<<n_filas<<std::endl;
        // Ahora mostramos los valores:                                       
        //for(int i = 0; i< col_list.size(); i++){
       for(int j = 0; j < n_filas; j++){                                              
         for(int i = 0; i< col_list.size(); i++){                                                                   
            auto& cell = (global_table_dict[nombre_tabla]->data_ptr->columns[col_list[i].nombre])[j];

             std::visit([](auto&& val){
             std::cout << " | "<<val;}, cell);
          };
          std::cout<<" | "<<std::endl;
      };
   };

};