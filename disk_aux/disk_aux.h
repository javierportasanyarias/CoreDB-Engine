#ifndef DISK_AUX
#define DISK_AUX

#include "data_struct.h"
#include <iostream>
#include <fstream>
#include <string>
#include "globals.h"
#include <map>
#include "logging.h"
#include "filesystem"
#include "disk_buffer.h"
#include <cstring> // Para usar std::memcpy
#include "part_sort.h"


namespace disk_aux{


    ////////////////////////////////////////////////////////////////////
    // FUNCIONES AUXILIARES ////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////

    void aux_vector_buffer_write_disk(char* ptr_ini, uint32_t size_buffer, std::ofstream& out);
    void aux_vector_buffer_write_disk(const std::vector<char>& buffer, std::ofstream& out);
    void aux_vector_buffer_write_disk(char* ptr_ini, uint32_t size_buffer, std::fstream& out);
    void aux_vector_buffer_write_disk(const std::vector<char>& buffer, std::fstream& out);

    ////////////////////////////////////////////////////////////////////
    // FUNCIONES A DESCARTAR ///////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////

    Values read_aux_val(dataType tipo_dato, std::ifstream& in);

    ////////////////////////////////////////////////////////////////////
    // FUNCIONES AUXILIARES ////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////

    std::vector<std::string> obtener_archivos_en_ruta(const std::string& ruta, dataType tipo_dato);


    uint32_t obtener_tamano_archivo(const std::string& ruta);



    void write_aux_val_buffer(Values value, dataType tipo_dato, std::vector<char>& buffer);
    void write_aux_val_buffer_with_size_check(const Values& value, dataType tipo_dato, char*& buffer, uint32_t& offset, uint32_t& bytes_written, uint32_t& bytes_remain, char*& ptr_str_ini, uint32_t& current_col);



    std::streamsize return_file_size(std::ifstream& in);
    uint32_t return_file_size_bytes(std::ifstream& in);


    void fill_vector_int(uint32_t num_elementos, char* vec_in, std::vector<Values>& vec_out);
    void fill_vector_int(uint32_t num_elementos, std::vector<char>& vec_in, std::vector<Values>& vec_out);

    void fill_vector_float(uint32_t num_elementos, char* vec_in, std::vector<Values>& vec_out);
    void fill_vector_float(uint32_t num_elementos, std::vector<char>& vec_in, std::vector<Values>& vec_out);

    void fill_vector_bool(uint32_t num_elementos, char* vec_in, std::vector<Values>& vec_out);
    void fill_vector_bool(uint32_t num_elementos, std::vector<char>& vec_in, std::vector<Values>& vec_out);

    void fill_vector_string(uint32_t num_elementos, std::vector<char>& vec_str,std::vector<char>& vec_idx, std::vector<Values>& vec_out);
    ////////////////////////////////////////////////////////////////////
    // ELIMINACIÓN DE ARCHIVOS /////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////

    void eliminar_archivo_binario_metadatos(table*& tb);


    void eliminar_archivo_binario_datos(table*& tb);

    ////////////////////////////////////////////////////////////////////
    // LECTURA DE TODOS LOS METADATOS //////////////////////////////////
    ////////////////////////////////////////////////////////////////////

    // Funcion auxilar recursiva:
    void escanear_tablas_recursiva(const std::filesystem::path& ruta, std::vector<std::filesystem::path>& arr_tablas);

    // SOLO ESCANEA LOS METADATOS DE LAS TABLAS:
    std::vector<std::filesystem::path> escanear_tablas();
};

#endif