#ifndef DISK_METADATA
#define DISK_METADATA

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

namespace disk_metadata {


    /*void aux_vector_buffer_write_disk(std::vector<char>& buffer, std::ofstream& out);
    void aux_vector_buffer_write_disk(std::vector<char>& buffer, std::fstream& out);*/
    void read_table_metadata(std::filesystem::path ruta_tabla, std::string nombre_tabla_str);
    uint32_t calculate_metadata_byte_size(table_metadata* metadatos_ptr);
    uint32_t calculate_metadata_byte_size_wal(table_metadata* metadatos_ptr);
    uint32_t write_table_metadata(table* tabla);
    void lectura_metadatos_todas_tablas();

}; // Cerrar el namespace 'disk_metadata'


#endif