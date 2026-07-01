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
    char* recuperar_meta_wal_tabla_buffer(std::ifstream& in, const std::string& nombre_tabla);

    std::string recuperar_meta_wal_tabla_nombre(std::ifstream& in);

    void aux_read_single_table_wal_metadata(std::ifstream& in, const std::string& nombre_tabla);


    bool is_eof_read(std::ifstream& in);

    void read_wal_viejo();
    void read_wal();


    class walDataReader {

    public:
        uint8_t state = 0;
        bool eof_rows = false;
        std::string table_name;

        char* buffer = nullptr;
        char* data_buffer_pointer = nullptr;
        
        // Auxiliary counters:
        //uint32_t num_rows_written = 0;

        // Table info:
        table* table = nullptr;
        uint32_t num_cols = 0;
        uint32_t col_counter = 0;
        uint32_t num_rows = 0;
        uint32_t row_counter = 0;

        // Buffer counters:
        uint32_t buffer_tmp_size = 0;
        uint32_t buffer_bytes_read = 0;
        //uint32_t buffer_size = size_buffer_bytes;
        uint32_t buffer_bytes_available = buffer_tmp_size;
        bool offset = false;

        // Reading:
        //uint32_t bytes_read = size_buffer_bytes;
        uint32_t bytes_read = 0;
        uint32_t tmp_var_len_bytes = 0;

        // Auxiliary variables:
        uint32_t var_len_bytes = 0;
        bool len_read = false;
        Values value;
        uint32_t var_len_bytes_read = 0;


        // Row iterator:
        //disk_buffer::tableRowIterator_only_ram_for_wal* row_iterator;

        //std::map<std::string, std::vector<Values>>& columnas; // = tabla->data_ptr->columns;
        table_metadata* metadata = nullptr;
        std::map<std::string, std::vector<Values>>* columnas = nullptr;
        std::vector<std::string> col_names; //= tabla->metadata_ptr->column_names;
        std::vector<dataType> data_types; //= tabla->metadata_ptr->column_types;

        // Reading pipeline:
        std::ifstream& in;

        walDataReader(std::ifstream& in_obj, const std::string& table_name_str) 
            : in(in_obj), eof_rows(false), buffer_tmp_size(0), buffer(nullptr), table(nullptr), state(0), table_name(table_name_str) {
            Logger::log(LogLevel::DEBUG, "Creacion de un objeto nuevo 'walDataReader'");
        };

        ~walDataReader() {
            if(this->buffer){
                delete[] this->buffer;
                this->buffer = nullptr;
            };
        };

        bool is_eof_rows(){
            return this->col_counter >= this->num_cols && this->row_counter + 1 >= this->num_rows;
            //return this->col_counter >= this->num_cols && this->row_counter >= this->num_rows;
        };

        void fill_buffer();

        void control_unit();

        void execute_fsm();

    };



    class walDataParser {

    public:
        // FSM state:
        uint8_t state = 0;
        bool eof_file = false;
        uint32_t wal_data_type;
        std::string table_name;

        // punteros de metadatos:
        //char* meta_buffer = nullptr;

        // Punteros de datos:

        // Row iterator:
        //disk_buffer::tableRowIterator_only_ram_for_wal* row_iterator;

        // Reading pipeline:
        std::ifstream in;

        walDataParser() 
            : eof_file(false), wal_data_type(0), state(0) {
            Logger::log(LogLevel::DEBUG, "Creacion de un objeto nuevo 'walDataParser'");
        }

        ~walDataParser() {
            //delete this->row_iterator;
            if (this->in.is_open()) {
                this->in.close();
            }
        }

        void control_unit();

        void execute_fsm();

    };


}; // Cerrar el namespace 'disk_wal_read'

#endif