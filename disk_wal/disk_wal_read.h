#ifndef DISK_WAL_READ
#define DISK_WAL_READ

#include "data_struct.h"
#include <iostream>
#include <fstream>
#include <string>
#include "globals.h"
#include <map>
#include "logging.h"



namespace disk_wal_read {


    // Funcions relativas a la lectura del WAl de metadatos:
    char* recuperar_meta_wal_tabla_buffer(std::ifstream& in, std::string nombre_tabla);

    std::string recuperar_meta_wal_tabla_nombre(std::ifstream& in);

    void recuperar_data_wal_tabla_buffer(std::ifstream& in, std::string nombre_tabla);

    //void aux_read_single_table_wal_metadata(std::ifstream& in);
    void aux_read_single_table_wal_metadata(std::ifstream& in, std::string nombre_tabla);

    //void aux_read_single_table_wal_data(std::ifstream& in);
    void aux_read_single_table_wal_data(std::ifstream& in, std::string nombre_tabla);

    bool is_eof_read_viejo(std::ifstream& in);
    bool is_eof_read(std::ifstream& in);

    void read_wal_viejo();
    void read_wal();


}; // Cerrar el namespace 'disk_wal_read'

#endif