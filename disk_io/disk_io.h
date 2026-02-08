#ifndef DISK_IO
#define DISK_IO

#include "data_struct.h"
#include <iostream>
#include <fstream>
#include <string>
#include "globals.h"
#include <map>
#include "logging.h"

namespace disk_io {


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

        // Path de los datos:
	if(tabla->metadata_ptr){
           Logger::log(LogLevel::OUTPUT, "SE HA ENCONTRADO LA TABLA");
	}else{
           Logger::log(LogLevel::OUTPUT, "NO SE HA ENCONTRADO LA TABLA");
	};
        std::string nombre_tabla = tabla->metadata_ptr->name; // EL ERROR ESTA EN ESTA LINEA
        //std::string ruta_tabla = "data/" + nombre_tabla + ".bin";

        // Abrimos la escritura:
        //std::ofstream out(ruta_tabla, std::ios::binary | std::ios::out);

        // ============================================================
        // == Primero escribimos los metadatos: =======================
        // ============================================================
        // -- Escribimos el nombre -----------------------------------------
        //uint32_t size_nombre = nombre_tabla.size();
        //out.write(reinterpret_cast<char*>(&size_nombre), sizeof(uint32_t));
        //out.write(nombre_tabla.data(), size_nombre);

        // -- Escribimos el número de columnas -----------------------------
        //uint32_t num_cols = (tabla->metadata_ptr->primary_list).size();
        //out.write(reinterpret_cast<char*>(&num_cols), sizeof(uint32_t));

        //for(uint32_t i=0; i<num_cols; i++){
            // -- Escribimos los datos de cada columna ---------------------
            // -- Escribimos el nombre:
            //std::string column_name = (tabla->metadata_ptr->column_names)[i];
            //uint32_t size_column_name = column_name.size();
            //out.write(reinterpret_cast<char*>(&size_column_name), sizeof(uint32_t));
            //out.write(column_name.data(), size_column_name);
            // -- Escribimos el tipo de dato:
            //dataType col_type = (tabla->metadata_ptr->column_types)[i];
            //uint32_t col_type_disk = static_cast<uint32_t>(col_type);
            //out.write(reinterpret_cast<char*>(&col_type_disk), sizeof(uint32_t));
            // -- Escribimos si es clave primaria:
            //bool column_is_key = (tabla->metadata_ptr->primary_list)[i];
	    //uint8_t key_val = column_is_key ? 1 : 0;
	    //out.write(reinterpret_cast<char*>(&key_val), sizeof(uint8_t));
        //};

        // ============================================================
        // == Escribimos los datos ====================================
        // ============================================================

        // Primero hallamos el número de instancias:
	//std::map<std::string, std::vector<Values>> columnas = tabla->data_ptr->columns;
	// Antes de nada escrobiremos el numero de columnas:
	//out.write(reinterpret_cast<char*>(&num_cols), sizeof(uint32_t));
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Escribimos en un bucle las columnas:
        //for(uint32_t i=0; i<num_cols; i++){
	   //std::string column_name = (tabla->metadata_ptr->column_names)[i];
	   // Primero, recuperamos el array de datos a escribir:
	   //std::vector<Values> col_datos = columnas.at(column_name);

	   // Escribimos el tamaño del array: (n_rows es el tamaño del array a escribir en cada iteracion. nos sirve tsmbien para luego en write_buffer escribir elemento a elemento)
	   //uint32_t n_rows = col_datos.size(); 
	   //out.write(reinterpret_cast<char*>(&n_rows), sizeof(uint32_t));

           // Ahora operaremos en funcion del tipo de variable:
	   //dataType col_type = (tabla->metadata_ptr->column_types)[i];

	   //switch(col_type){
               //case dataType::INT: {
		   //std::vector<int> buffer(n_rows);
		   //write_buffer(out, n_rows, col_datos , buffer);
		   //break;
	       //};
	       //case dataType::FLOAT: {
		   //std::vector<float> buffer(n_rows);  
		   //write_buffer(out, n_rows, col_datos , buffer);
                   //break;
	       //};
	       //case dataType::STRING: {             
		   //write_buffer(out, n_rows, col_datos);
                   //break;
	       //};
	       //case dataType::BOOL: {
	       //std::vector<uint8_t> buffer(n_rows);
		   //write_buffer(out, n_rows, col_datos , buffer);
                   //break;
		//};
		//case dataType::UNKNOWN: {
		   //Logger::log(LogLevel::ERROR, "ERROR: Tipo de dato desconocido");
		   //break;
		//};

	   //};
	

	//};
	

        // Terminamos la escritura:
	//out.close();

    };



    // FUNCIÓN DE SINCRONIZACIÓN:
    void write_dump(){

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



};


#endif
