#pragma once


namespace disk_out {

    void fix_vect_vals(std::vector<Values>::iterator start_iterator, std::vector<Values>::iterator end_iterator, int* arr_vals);
    void fix_vect_vals(std::vector<Values>::iterator start_iterator, std::vector<Values>::iterator end_iterator, float* arr_vals);
    void fix_vect_vals(std::vector<Values>::iterator start_iterator, std::vector<Values>::iterator end_iterator, uint8_t* arr_vals);

    
    void obtain_writing_indexes(auto& idx_start, auto& idx_end, std::vector<Values>*& vec_var_ptr, int written_rows, int& rows_to_write_input);
    int obtain_available_size(std::filesystem::path path_var_input, std::string last_partition_str_input, int size_partition_input);
    void write_partition_int(auto& idx_start, auto& idx_end, std::filesystem::path write_path_input);
    void write_partition_float(auto& idx_start, auto& idx_end, std::filesystem::path write_path_input);
    void write_partition_bool(auto& idx_start, auto& idx_end, std::filesystem::path write_path_input);
    void write_partition_string(std::vector<Values>::iterator start_iterator, std::vector<Values>::iterator end_iterator, std::filesystem::path write_path_input);
    void write_partition_unknown(std::vector<Values>::iterator start_iterator, std::vector<Values>::iterator end_iterator, std::filesystem::path write_path_input);


    void writing_loop_int(std::vector<Values>*& vec_var_ptr, int total_bytes_to_write_input, int partition_entities, std::filesystem::path path_var_input, std::string last_partition_str_input, int last_partition_int_input);
    void writing_loop_float(std::vector<Values>*& vec_var_ptr, int total_bytes_to_write_input, int partition_entities, std::filesystem::path path_var_input, std::string last_partition_str_input, int last_partition_int_input);
    void writing_loop_bool(std::vector<Values>*& vec_var_ptr, int total_bytes_to_write_input, int partition_entities, std::filesystem::path path_var_input, std::string last_partition_str_input, int last_partition_int_input);
    void writing_loop_tring(std::vector<Values>*& vec_var_ptr, int total_bytes_to_write_input, int partition_entities, std::filesystem::path path_var_input, std::string last_partition_str_input, int last_partition_int_input);
    void writing_loop_unknown(std::vector<Values>*& vec_var_ptr, int total_bytes_to_write_input, int partition_entities, std::filesystem::path path_var_input, std::string last_partition_str_input, int last_partition_int_input);


    void write_table_data(table* table_ptr_input);

}; // Closing 'disk_io' namespace
