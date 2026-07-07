#pragma once


namespace disk_in {

    void read_fixed_len_int_columns(std::filesystem::path nombre_tabla, std::string column_name, std::vector<Values>& vec_vals);
    void read_fixed_len_float_columns(std::filesystem::path nombre_tabla, std::string column_name, std::vector<Values>& vec_vals);
    void read_fixed_len_bool_columns(std::filesystem::path nombre_tabla, std::string column_name, std::vector<Values>& vec_vals);
    //void read_fixed_len_string_columns(std::filesystem::path nombre_tabla, std::string column_name, std::vector<Values>& vec_vals);
    //void read_table_data(table* tabla);
    class read_table_iterator{
        /*
        Clase que irá insertando leyendo e insertando
        en la memoria de la tabla el contenido de las particiones, una a una.
        */
        public:
            uint32_t partition_counter;
            uint32_t total_partitions;
            std::vector<std::string> particiones_nombres;
            table* tabla;
            bool partition_eof; // Mide si ya no queda ninguna particción más

            std::string nombre_tabla;
            std::vector<dataType> tipo_columnas;
            uint32_t num_cols;
            std::vector<std::string> nombre_columnas;
            table_data_buffer* ptr_datos_disco;

            uint32_t numero_de_filas_current_partition;

            // Metodo constructor
            read_table_iterator(table* tabla);

            bool obtain_int_partition_rows(bool aux_bool, std::filesystem::path path_var, std::string partition_current);
            bool obtain_float_partition_rows(bool aux_bool, std::filesystem::path path_var, std::string partition_current);
            bool obtain_bool_partition_rows(bool aux_bool, std::filesystem::path path_var, std::string partition_current);
            bool obtain_string_partition_rows(bool aux_bool, std::filesystem::path path_var, std::string partition_current);

            // Lectura iteractiva a través de las particiones
            bool read_table();

    };

}; // Cerrar el namespace 'disk_in'
