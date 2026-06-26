#ifndef DISK_OUT
#define DISK_OUT

#include "data_struct.h"
#include <iostream>
#include <fstream>
#include <string>
#include "globals.h"
#include <map>
#include "logging.h"
#include "filesystem"
#include <vector> // <--- FUNDAMENTAL

namespace fs = std::filesystem;

namespace disk_out {


    /*void write_fixed_len_columns(std::string nombre_tabla, std::string column_name, std::vector<int>& vec_vals);
    void write_fixed_len_columns(std::string nombre_tabla, std::string column_name, std::vector<float>& vec_vals);
    void write_fixed_len_columns(std::string nombre_tabla, std::string column_name, std::vector<uint8_t>& vec_vals);
    */
    void fix_vect_vals(std::vector<Values>::iterator inicio, std::vector<Values>::iterator fin, int* arr_vals);
    void fix_vect_vals(std::vector<Values>::iterator inicio, std::vector<Values>::iterator fin, float* arr_vals);
    void fix_vect_vals(std::vector<Values>::iterator inicio, std::vector<Values>::iterator fin, uint8_t* arr_vals);
    uint32_t fix_vect_vals(std::vector<Values>::iterator inicio, std::vector<Values>::iterator fin, std::vector<char>& vec_vals, std::vector<uint32_t>& vec_sizes);
    
    void calcular_filas_a_escribir_int(int& filas_a_escribir, int size_disponible_para_escribir, int partition_entities, int filas_totales_a_escribir);
    void calcular_filas_a_escribir_float(int& filas_a_escribir, int size_disponible_para_escribir, int partition_entities, int filas_totales_a_escribir);
    void calcular_filas_a_escribir_bool(int& filas_a_escribir, int size_disponible_para_escribir, int partition_entities, int filas_totales_a_escribir);
    void calcular_filas_a_escribir_string(int& filas_a_escribir, int size_disponible_para_escribir, int partition_entities,  int filas_totales_a_escribir);

    
    void obtener_indices_vector_valores(auto& inicio, auto& fin, std::vector<Values>*& vec_var_ptr, int filas_escritas, int& filas_a_escribir);
    int obtener_size_disponible(std::string ruta_variable, std::string ultima_particion_str, int size_particion);
    void escribir_particion_int(auto& inicio, auto& fin, std::string ruta_escritura);
    void escribir_particion_float(auto& inicio, auto& fin, std::string ruta_escritura);
    void escribir_particion_bool(auto& inicio, auto& fin, std::string ruta_escritura);
    void escribir_particion_string(std::vector<Values>::iterator inicio, std::vector<Values>::iterator fin, std::string ruta_escritura);
    void escribir_particion_unknown(std::vector<Values>::iterator inicio, std::vector<Values>::iterator fin, std::string ruta_escritura);


    void bucle_escritura_int(std::vector<Values>*& vec_var_ptr, int bytes_totales_a_escribir, int partition_entities, std::string ruta_variable, std::string ultima_particion_str, int ultima_particion_int);
    void bucle_escritura_float(std::vector<Values>*& vec_var_ptr, int bytes_totales_a_escribir, int partition_entities, std::string ruta_variable, std::string ultima_particion_str, int ultima_particion_int);
    void bucle_escritura_bool(std::vector<Values>*& vec_var_ptr, int bytes_totales_a_escribir, int partition_entities, std::string ruta_variable, std::string ultima_particion_str, int ultima_particion_int);
    void bucle_escritura_string(std::vector<Values>*& vec_var_ptr, int bytes_totales_a_escribir, int partition_entities, std::string ruta_variable, std::string ultima_particion_str, int ultima_particion_int);
    void bucle_escritura_unknown(std::vector<Values>*& vec_var_ptr, int bytes_totales_a_escribir, int partition_entities, std::string ruta_variable, std::string ultima_particion_str, int ultima_particion_int);


    void write_table_data(table* tabla);

}; // Cerrar el namespace 'disk_io'


#endif