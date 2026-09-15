#pragma once
#include "disk_buffer.h"

namespace disk_wal_write {

    void write_table_wal_metadata(table* table_ptr_input);
    void write_table_data_wal(table* table_ptr_input, uint32_t n_rows_to_write_input);

    class walDataWriter {

    public:
        // Counters:
        uint32_t offset = 0;
        uint32_t bytes_written = 0;
        uint32_t bytes_remain = size_buffer_bytes;
        uint8_t state = 0;
        char* ptr_str_ini = nullptr;

        // Auxiliary metadata:
        std::vector<dataType> data_types;
        std::vector<std::string> column_names;

        // Buffer:
        char* buffer = new char[size_buffer_bytes];
        char* buffer_pointer = nullptr;

        // Columns:
        uint32_t num_cols = 0;
        uint32_t current_col = 0;

        // Control variables:
        uint32_t n_rows_to_write = 0;

        // Write and table objects:
        std::ofstream out;
        table* table_obj = nullptr;
        disk_buffer::tableRowIterator_only_ram_for_wal* row_iterator;
        std::map<std::string, Values> row_to_write;

        walDataWriter(table* t_obj, uint32_t rows_to_write) 
            : table_obj(t_obj), n_rows_to_write(rows_to_write), row_iterator(nullptr) {
            Logger::log(LogLevel::DEBUG, "Creacion de un objeto nuevo 'walDataWriter'");
            this->buffer_pointer = this->buffer;
            // Buffer set to all zeros:
            std::memset(this->buffer, 0, size_buffer_bytes);
        };

        ~walDataWriter() {
            delete[] this->buffer;
            delete this->row_iterator;
            if (this->out.is_open()) {
                this->out.close();
            };
        };

        bool eof_row(){
            return this->current_col + 1 > this->num_cols;
        };

        void control_unit();

        void execute_fsm();

    };

}; // Closing 'disk_wal_write' namespace
