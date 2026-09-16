#pragma once

#include "disk_buffer.h"
#include "part_sort.h"

namespace disk_aux {

//====================================================
//================= Auxiliary functions ==============
//====================================================

void aux_vector_buffer_write_disk(char* ptr_ini, uint32_t size_buffer,
                                  std::ofstream& out);

std::vector<std::string> obtain_files_in_path(const std::filesystem::path& ruta,
                                              dataType tipo_dato);

uint32_t obtain_file_size(const std::filesystem::path& ruta);

void write_aux_val_buffer_with_size_check(
    const Values& value, dataType tipo_dato, char*& buffer, uint32_t& offset,
    uint32_t& bytes_written, uint32_t& bytes_remain, char*& ptr_str_ini,
    uint32_t& current_col);

uint32_t return_file_size_bytes(std::ifstream& in);

void fill_vector_int(uint32_t elements_num, char* vec_in,
                     std::vector<Values>& vec_out);

void fill_vector_float(uint32_t elements_num, char* vec_in,
                       std::vector<Values>& vec_out);

void fill_vector_bool(uint32_t elements_num, char* vec_in,
                      std::vector<Values>& vec_out);

//====================================================
//================= File deletion ====================
//====================================================

void delete_bin_metadata_file(table*& tb);

void delete_bin_data_file(table*& tb);

//====================================================
//================= Metadata reading =================
//====================================================

void scan_tables_recursive(const std::filesystem::path& ruta,
                           std::vector<std::filesystem::path>& arr_tablas);

std::vector<std::filesystem::path> scan_tables();
};  // namespace disk_aux
