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

    void aux_vector_buffer_write_disk(std::vector<char> buffer, std::ofstream& out);

    void eliminar_archivo_binario_metadatos(table*& tb);

    void eliminar_archivo_binario_datos(table*& tb);

    void write_buffer(std::ofstream& out, uint32_t n_rows, std::vector<Values> col_datos,  std::vector<int>& buffer);

    void write_buffer(std::ofstream& out, uint32_t n_rows, std::vector<Values> col_datos,  std::vector<float>& buffer);

    void write_buffer(std::ofstream& out, uint32_t n_rows, std::vector<Values> col_datos,  std::vector<uint8_t>& buffer);

    void write_buffer(std::ofstream& out, uint32_t n_rows, std::vector<Values> col_datos);


    // FUNCIÓN DE ESCRITURA:
    uint32_t write_table_metadata(table* tabla);

    void write_table_wal_metadata(table* tabla);

    void write_aux_val(Values value, dataType tipo_dato, std::fstream& out);

    void write_aux_val(Values value, dataType tipo_dato, std::ofstream& out);

    void write_table_data(table* tabla, uint32_t n_rows);

    void write_table_data_wal(table* tabla, uint32_t n_rows_a_escribir);

    void write_table_data_viejo(table* tabla, uint32_t n_rows);

    Values read_aux_val(dataType tipo_dato, std::ifstream& in);

    void read_table_data(table*& tabla);

    // Funciones de eliminacion:

    void delete_wal_bin_file();



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