#ifndef DISK_IO
#define DISK_IO

#include "data_struct.h"
#include <iostream>
#include <fstream>
#include <string>
#include "globals.h"

namespace disk_io {

    // FUNCIÓN DE SINCRONIZACIÓN:
    void write_dump(){

        /*
        Este método, una vez llamado, escribe todas las tablas, sobreescribiendo por defecto
        */

        for (const auto& [table_name, table_ptr] : global_table_dict) {
            // Escribimos tabla por tabla:
            write_table(table_ptr);

        };
    };


    // FUNCIÓN DE ESCRITURA:
    void write_table(table* tabla){

        // Path de los datos:
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
            std::string column_name = (table->table_metadata->column_names)[i];
            uint32_t size_column_name = column_name.size();
            out.write(reinterpret_cast<char*>(&size_column_name), sizeof(uint32_t));
            out.write(column_name.data(), size_column_name);
            // -- Escribimos el tipo de dato:
            dataType col_type = (tabla->metadata_ptr->column_types)[i];
            uint32_t col_type_disk = static_cast<uint32_t>(col_type);
            out.write(reinterpret_cast<char*>(&col_type_disk), sizeof(uint32_t));
            // -- Escribimos si es clave primaria:
            bool column_is_key = (table->table_metadata->primary_list)[i];
            out.write(column_is_key.data(), sizeof(uint8_t));
        };

        // ============================================================
        // == Escribimos los datos ====================================
        // ============================================================

        // Primero hallamos el número de instancias:


        // Terminamos la escritura:
        out.close();

    };


    void write_table(table* tabla){

        // leemos la tabla con sus metadatos:

        
    };



};


#endif