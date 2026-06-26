#ifndef DISK_WAL_UTILS
#define DISK_WAL_UTILS

#include "data_struct.h"
#include <iostream>
#include <fstream>
#include <string>
#include "globals.h"
#include <map>
#include "logging.h"



namespace disk_wal_utils {


    // Funciones de eliminacion:

    void delete_wal_bin_file();


}; // Cerrar el namespace 'disk_wal_utils'

#endif