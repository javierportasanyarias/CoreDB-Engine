#ifndef DISK_WAL
#define DISK_WAL

#include "data_struct.h"
#include <iostream>
#include <fstream>
#include <string>
#include "globals.h"
#include <map>
#include "logging.h"



namespace disk_wal {


    void write_table_wal_metadata(table* tabla);

    void write_table_data_wal_viejo(table* tabla, uint32_t n_rows_a_escribir);
    void write_table_data_wal(table* tabla, uint32_t n_rows_a_escribir);

    // Funciones de eliminacion:

    void delete_wal_bin_file();

    // Funcions relativas a la lectura del WAl de metadatos:
    char* recuperar_meta_wal_tabla_buffer(std::ifstream& in, std::string nombre_tabla);

    std::string recuperar_meta_wal_tabla_nombre(std::ifstream& in);

    void recuperar_data_wal_tabla_buffer(std::ifstream& in, std::string nombre_tabla);

    void aux_read_single_table_wal_metadata(std::ifstream& in);

    void aux_read_single_table_wal_data(std::ifstream& in);

    bool is_eof_read(std::ifstream& in);

    void read_wal();


}; // Cerrar el namespace 'disk_wal'

#endif