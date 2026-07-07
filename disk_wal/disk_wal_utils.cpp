#include "disk_wal_utils.h"


//========================================================
//== ELIMINAR EL ARCHIVO WAL ENTERO: =====================
//========================================================


void disk_wal_utils::delete_wal_bin_file(){
   /*
   Función que elimina el arhivo de recuperación o 'WAL'.
   Debería ejecutarse al final de cada sesión sólo y únicamente después
   de haber realizado la escritura de los archivos binarios de
   metadatos y datos binarios sin fallos
   */
   if(fs::remove(std::filesystem::path("backup_data/wal.bin"))){
      Logger::log(LogLevel::DEBUG, "Archivo WAl eliminado con EXITO");
   } else {
      Logger::log(LogLevel::DEBUG, "El archivo WAL no existía, por lo que NO ha sido eliminado");
   };
};