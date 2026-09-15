#include "disk_wal_utils.h"

//========================================================
//== ELIMINAR EL ARCHIVO WAL ENTERO: =====================
//========================================================

void disk_wal_utils::delete_wal_bin_file() {
  /*
  Function that deletes the recovery file or 'WAL'.
  It should run at the end of each session/execution if and only if
  the binary metadata and data files have been written without errors.
  */

  if (fs::remove(std::filesystem::path("backup_data/wal.bin"))) {
    Logger::log(LogLevel::DEBUG, "WAl file deleted SUCCESSFULLY");
  } else {
    Logger::log(LogLevel::DEBUG,
                "The WAL did not exist, thus can not be erased.");
  };
};