#ifndef DISK_IO
#define DISK_IO

#include "data_struct.h"
#include <iostream>
#include <fstream>
#include <string>
#include "globals.h"
#include <map>
#include "logging.h"
#include "filesystem"

namespace disk_io {

	namespace fs = std::filesystem;


    void write_buffer(std::ofstream& out, uint32_t n_rows, std::vector<Values> col_datos,  std::vector<int>& buffer){

	// Creamos el buffer de los datos:
        for(size_t i = 0; i < n_rows; i++) {
            buffer[i] = std::get<int>(col_datos[i]);
	};

	out.write(reinterpret_cast<char*>(buffer.data()), n_rows * sizeof(int));
    };


    void write_buffer(std::ofstream& out, uint32_t n_rows, std::vector<Values> col_datos,  std::vector<float>& buffer){

        // Creamos el buffer de los datos:
        for(size_t i = 0; i < n_rows; i++) {
            buffer[i] = std::get<float>(col_datos[i]);                                     };
        out.write(reinterpret_cast<char*>(buffer.data()), n_rows * sizeof(float));
    };


    void write_buffer(std::ofstream& out, uint32_t n_rows, std::vector<Values> col_datos,  std::vector<uint8_t>& buffer){

        // Creamos el buffer de los datos:
        for (size_t k = 0; k < n_rows; k++){
            buffer[k] = std::get<bool>(col_datos[k]) ? 1 : 0;
	};
        out.write(reinterpret_cast<char*>(buffer.data()), n_rows * sizeof(uint8_t));

    };


    void write_buffer(std::ofstream& out, uint32_t n_rows, std::vector<Values> col_datos){

        // Creamos el buffer de los datos:
        for(size_t i = 0; i < n_rows; i++) {
            const std::string& s = std::get<std::string>(col_datos[i]);
	    uint32_t s_len = s.size();
	    out.write(reinterpret_cast<char*>(&s_len), sizeof(uint32_t));
	    out.write(s.data(), s_len);
	};
        
    };


    // FUNCIÓN DE ESCRITURA:
    void write_table(table* tabla){
	int a = 0;
	if (!tabla) return;
        Logger::log(LogLevel::DEBUG, "DENTRO DE 'write_table': ");
        std::string nombre_tabla = tabla->metadata_ptr->name;
	std::string ruta_tabla = "data/" + nombre_tabla + ".bin";

        // Abrimos la escritura:
        std::ofstream out(ruta_tabla, std::ios::binary | std::ios::out);

        // ============================================================
        // == Primero escribimos los metadatos: =======================
        // ============================================================
        // -- Escribimos el nombre -----------------------------------------
        uint32_t size_nombre = nombre_tabla.size();
        out.write(reinterpret_cast<char*>(&size_nombre), sizeof(uint32_t));
        out.write(nombre_tabla.data(), size_nombre);

        // -- Escribimos el número de columnas -----------------------------
        uint32_t num_cols = (tabla->metadata_ptr->primary_list).size();
        out.write(reinterpret_cast<char*>(&num_cols), sizeof(uint32_t));

        for(uint32_t i=0; i<num_cols; i++){
            // -- Escribimos los datos de cada columna ---------------------
            // -- Escribimos el nombre:
            std::string column_name = (tabla->metadata_ptr->column_names)[i];
            uint32_t size_column_name = column_name.size();
            out.write(reinterpret_cast<char*>(&size_column_name), sizeof(uint32_t));
            out.write(column_name.data(), size_column_name);
            // -- Escribimos el tipo de dato:
            dataType col_type = (tabla->metadata_ptr->column_types)[i];
            uint32_t col_type_disk = static_cast<uint32_t>(col_type);
            out.write(reinterpret_cast<char*>(&col_type_disk), sizeof(uint32_t));
            // -- Escribimos si es clave primaria:
            bool column_is_key = (tabla->metadata_ptr->primary_list)[i];
	    uint8_t key_val = column_is_key ? 1 : 0;
	    out.write(reinterpret_cast<char*>(&key_val), sizeof(uint8_t));
        };

        // ============================================================
        // == Escribimos los datos ====================================
        // ============================================================

        // Primero hallamos el número de instancias:
	std::map<std::string, std::vector<Values>> columnas = tabla->data_ptr->columns;
	// Antes de nada escrobiremos el numero de columnas:
	out.write(reinterpret_cast<char*>(&num_cols), sizeof(uint32_t));
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Escribimos en un bucle las columnas:
        for(uint32_t i=0; i<num_cols; i++){
	   std::string column_name = (tabla->metadata_ptr->column_names)[i];
	   // Primero, recuperamos el array de datos a escribir:
	   std::vector<Values> col_datos = columnas.at(column_name);

	   // Escribimos el tamaño del array: (n_rows es el tamaño del array a escribir en cada iteracion. nos sirve tsmbien para luego en write_buffer escribir elemento a elemento)
	   uint32_t n_rows = col_datos.size(); 
	   out.write(reinterpret_cast<char*>(&n_rows), sizeof(uint32_t));

           // Ahora operaremos en funcion del tipo de variable:
	   dataType col_type = (tabla->metadata_ptr->column_types)[i];

	   switch(col_type){
               case dataType::INT: {
		   std::vector<int> buffer(n_rows);
		   write_buffer(out, n_rows, col_datos , buffer);
		   break;
	       };
	       case dataType::FLOAT: {
		   std::vector<float> buffer(n_rows);  
		   write_buffer(out, n_rows, col_datos , buffer);
                   break;
	       };
	       case dataType::STRING: {             
		   write_buffer(out, n_rows, col_datos);
                   break;
	       };
	       case dataType::BOOL: {
	           std::vector<uint8_t> buffer(n_rows);
		   write_buffer(out, n_rows, col_datos , buffer);
                   break;
		};
		case dataType::UNKNOWN: {
		   Logger::log(LogLevel::ERROR, "ERROR: Tipo de dato desconocido");
		   break;
		};

	   };	

	};	

        // Terminamos la escritura:
	out.close();

    };

    // Funcion auxilar recursiva:
    void escanear_tablas_recursiva(const std::filesystem::path& ruta, std::vector<std::string>& arr_tablas){
       // Iteramos por cada elemento:
       for (const auto& entrada : fs::directory_iterator(ruta)){
          // Caso de quecsea una ruta:
	  if (fs::is_directory(entrada)){
             escanear_tablas_recursiva(entrada.path(), arr_tablas);
	  } else if(fs::is_regular_file(entrada) && entrada.path().extension().string() == ".bin"){
             // Caso base de la recursion:
	     arr_tablas.push_back(entrada.path().string());
	     
    	  };
       };
};

   std::vector<std::string> escanear_tablas(){

       fs::path ruta_tablas = "data";
       // Vector con el nombre de todas las tablas:
       std::vector<std::string> arr_tablas;

       escanear_tablas_recursiva(ruta_tablas, arr_tablas);
       return arr_tablas;

    };

    void mostrar_tablas_disco(std::vector<std::string>& arr_tablas){

       Logger::log(LogLevel::DEBUG, "Mostramos las tablas en disco:");
       for(int i = 0; i<arr_tablas.size(); i++){
          Logger::log(LogLevel::DEBUG, "   Tabla: ", false, true);
	  Logger::log(LogLevel::DEBUG, arr_tablas[i], true, false);
       };
       Logger::flush();
    };



    // FUNCIÓN DE SINCRONIZACIÓN:
    void write_dump_viejo(){

        /*
        Este método, una vez llamado, escribe todas las tablas, sobreescribi        endo por defecto
        */

        for (const auto& [table_name, table_ptr] : global_table_dict) {
            // Escribimos tabla por tabla:
	    if (table_ptr == nullptr) {
               Logger::log(LogLevel::ERROR, "ERROR: Puntero de tabla nulo para: " + table_name);
	    }else {
	       Logger::log(LogLevel::OUTPUT, "NOS DISPONEMOS A ESCRBIR LA TABLA EN DISCO");
               write_table(table_ptr);
	    };

        };
    };


   void write_dump(){
      Logger::log(LogLevel::DEBUG, "--- INICIO DIAGNÓSTICO DE DICCIONARIO ---");
    Logger::log(LogLevel::DEBUG, "Total de entradas en global_table_dict: " + std::to_string(global_table_dict.size()));

      // Primero vemos si el diccionario esta vacio o no:
      if(global_table_dict.empty()){
         Logger::log(LogLevel::DEBUG, "No escribiremos nada, el diccionario esta vacio");
      }else{
         //El diccionario tiene contenido:
	 for (const auto& [table_name, table_ptr] : global_table_dict){
	    std::string info_clave = "Clave encontrada: [" + table_name + "] | Longitud: " + std::to_string(table_name.length());
        Logger::log(LogLevel::DEBUG, info_clave);

            // Solo enviamos a escribir si la tabla existe:
	    if (table_ptr != nullptr){
	       // Escribimos la tabla:
	       Logger::log(LogLevel::DEBUG, "ESCRIBIMOS LA TABLA: " + table_name);
	       write_table(table_ptr);

	    }else{
               Logger::log(LogLevel::DEBUG, "ERROR: Se ha encontrado una entrada vacia para la tabla: " + table_name);
	    };
	 };
      };

   };



};


#endif
