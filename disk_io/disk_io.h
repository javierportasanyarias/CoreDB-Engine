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
#include <vector> // <--- FUNDAMENTAL

namespace fs = std::filesystem;

namespace disk_io {

    void write_dump();
    void debug_print_metadatos_memoria();

}; // Cerrar el namespace 'disk_io'


#endif