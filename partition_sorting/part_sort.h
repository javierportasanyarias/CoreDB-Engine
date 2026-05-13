#ifndef PART_SORT
#define PART_SORT

#include <string>
#include <iostream>
#include <vector>
#include "logging.h"

namespace part_sort {

    std::string quitar_extension_archivo(std::string& input);

    std::string obtain_number(std::string input);

    std::string quitar_ceros(std::string input);


    int procesar_numero_particion(std::string input);

    std::string procesar_numero_particion_como_str(std::string input);


    bool compare_ints(int num_1, int num_2);

    bool compare_partition_values(std::string string_1, std::string string_2);

    void swap_string_vals(std::string* p1, std::string* p2);

    void bubble_sort(std::vector<std::string>& vec);

};

#endif