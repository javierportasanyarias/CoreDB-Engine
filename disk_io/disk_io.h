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

namespace fs = std::filesystem;

namespace disk_io {


    void write_buffer(std::ofstream& out, uint32_t n_rows, std::vector<Values> col_datos,  std::vector<int>& buffer);


    void write_buffer(std::ofstream& out, uint32_t n_rows, std::vector<Values> col_datos,  std::vector<float>& buffer);


    void write_buffer(std::ofstream& out, uint32_t n_rows, std::vector<Values> col_datos,  std::vector<uint8_t>& buffer);


    void write_buffer(std::ofstream& out, uint32_t n_rows, std::vector<Values> col_datos);


    // FUNCIÓN DE ESCRITURA:
    uint32_t write_table_metadata(table* tabla);

    void write_aux_val(Values value, dataType tipo_dato, std::fstream& out);

    void write_table_data(table* tabla, uint32_t n_rows);

    void write_table_data_viejo(table* tabla, uint32_t n_rows);

    Values read_aux_val(dataType tipo_dato, std::ifstream& in);

    void read_table_data(table*& tabla);



    void read_table_metadata(std::filesystem::path ruta_tabla, std::string nombre_tabla_str);



    // Funcion auxilar recursiva:
    void escanear_tablas_recursiva(const std::filesystem::path& ruta, std::vector<std::filesystem::path>& arr_tablas);

   std::vector<std::filesystem::path> escanear_tablas();

    void mostrar_tablas_disco(std::vector<std::filesystem::path>& arr_tablas);

    void lectura_metadatos_todas_tablas();



    // FUNCIÓN DE ESCRITURA DE TODAS LAS TABLAS:

   void write_dump();

   void debug_print_metadatos_memoria();


}; // Cerrar el namespace 'disk_io'


#endif