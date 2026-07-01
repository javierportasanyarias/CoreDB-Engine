#ifndef DISK_WAL_WRITE
#define DISK_WAL_WRITE

#include "data_struct.h"
#include <iostream>
#include <fstream>
#include <string>
#include "globals.h"
#include <map>
#include "logging.h"
//#include <sstream>
#include <cstring> // Para usar std::memcpy y memset
#include "disk_buffer.h"


namespace disk_wal_write {


    void write_table_wal_metadata(table* tabla);
    void write_table_data_wal(table* tabla, uint32_t n_rows_a_escribir);


    class walDataWriter {

    public:
        // Counters:
        uint32_t offset = 0;
        uint32_t bytes_written = 0;
        uint32_t bytes_remain = size_buffer_bytes;
        uint8_t state = 0;
        char* ptr_str_ini = nullptr; // 💡 Inicializado a limpio

        // Auxiliary metadata:
        std::vector<dataType> tipos_datos;
        std::vector<std::string> columnas_nombre;

        // Buffer:
        char* buffer = new char[size_buffer_bytes];
        char* buffer_pointer = nullptr;

        // Columns:
        uint32_t num_cols = 0;
        uint32_t current_col = 0;

        // Variables de control inyectadas:
        uint32_t n_rows_a_escribir = 0;

        // Write and table:
        std::ofstream out;
        table* table_obj = nullptr;
        disk_buffer::tableRowIterator_only_ram_for_wal* row_iterator;
        std::map<std::string, Values> fila_a_escribir;

        // El constructor ahora compilará perfectamente porque los punteros no tienen constructores obligatorios
        walDataWriter(table* t_obj, uint32_t rows_to_write) 
            : table_obj(t_obj), n_rows_a_escribir(rows_to_write), row_iterator(nullptr) {
            Logger::log(LogLevel::DEBUG, "Creacion de un objeto nuevo 'walDataWriter'");
            this->buffer_pointer = this->buffer;
            // Buffer set to all zeros:
            std::memset(this->buffer, 0, size_buffer_bytes);
        }

        ~walDataWriter() {
            delete[] this->buffer;
            delete this->row_iterator;
            if (this->out.is_open()) {
                this->out.close();
            }
        }

        bool eof_row(){
            return this->current_col + 1 > this->num_cols;
        };

        void control_unit();

        void execute_fsm();

    };

}; // Cerrar el namespace 'disk_wal_write'

#endif