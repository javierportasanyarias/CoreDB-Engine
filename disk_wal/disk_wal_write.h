#ifndef DISK_WAL_WRITE
#define DISK_WAL_WRITE

#include "data_struct.h"
#include <iostream>
#include <fstream>
#include <string>
#include "globals.h"
#include <map>
#include "logging.h"



namespace disk_wal_write {


    void write_table_wal_metadata(table* tabla);
    void write_table_data_wal(table* tabla, uint32_t n_rows_a_escribir);


// CODIGO LEGACY.
void write_table_data_wal_viejo(table* tabla, uint32_t n_rows_a_escribir);



}; // Cerrar el namespace 'disk_wal_write'

#endif