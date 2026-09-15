#include "part_sort.h"


std::string part_sort::erase_file_extension(std::string& input){

    uint32_t size_str = input.size();
    std::string output;
    for(int i = 0; i < size_str; i++){
        if(input[i] != '.'){
            output += input[i];
        } else{
            break;
        };
    };
    return output;
};


std::string part_sort::obtain_number(std::string input) {

    uint32_t input_size = input.size();
    std::string output = "";
    bool aux_bool = false;
    for(int i = 0; i < input_size; i++){
        if(input[i] == '.'){
            break;
        };
        if(aux_bool){
            output += input[i];
        };
        if(input[i] == '_'){
            aux_bool = true;
        };
    };
    return output;
};


std::string part_sort::erase_zeroes(std::string input){
    
    std::string output = "";
    bool aux_bool = false;
    uint32_t input_size = input.size();

    for(int i = 0; i < input_size; i++){
        if(input[i] == '.'){
            break;
        };
        if(aux_bool){
            output += input[i];
        };
        if(!aux_bool && input[i] != '0'){
            aux_bool = true;
            output += input[i];
        };
    };
    return output;
};


int part_sort::process_partition_number(std::string input) {
    std::string number_str = part_sort::obtain_number(input);
    
    if (number_str.empty()) return 0;

    try {
        return std::stoi(number_str);
    } catch (...) {
        /*
        If argument does not meet integer conversion criteria,
        the number 0 will be returned in order not to crash the program
        */
        return 0;
    }
}

std::string part_sort::process_partition_number_as_string(std::string input){
    std::string tmp;
    tmp = part_sort::obtain_number(input);
    Logger::log(LogLevel::DEBUG, "Partition number: ", false, true);
    Logger::log(LogLevel::DEBUG, tmp, true, false);
    Logger::log(LogLevel::DEBUG, "Erasing zeroes");
    tmp = part_sort::erase_zeroes(tmp);
    Logger::log(LogLevel::DEBUG, "After zeroes erasure: ", false, true);
    Logger::log(LogLevel::DEBUG, tmp, true, false);
    return tmp;
};


bool part_sort::compare_partition_values(std::string string_1, std::string string_2) {
    int num_1 = part_sort::process_partition_number(string_1);
    int num_2 = part_sort::process_partition_number(string_2);
    return num_1 > num_2; // Value comparison for bubble sort swap
};


void part_sort::swap_string_vals(std::string* p1, std::string* p2){

    // String swap/interchange

    std::string aux = *p1;
    *p1 = *p2;
    *p2 = aux;
};


void part_sort::bubble_sort(std::vector<std::string>& vec){
    if (vec.empty()) return;

    std::string* ptr_ini = vec.data();
    std::string* ptr_end = ptr_ini + vec.size() - 1;
    std::string* p1 = ptr_ini;

    while (p1 < ptr_end) {
        bool swaped = false;
        std::string* tmp_ptr = ptr_end - (p1 - ptr_ini);
        std::string* p2 = ptr_ini;

        while (p2 < tmp_ptr) {
            if(part_sort::compare_partition_values(*p2, *(p2 + 1))){
                part_sort::swap_string_vals(p2, p2 + 1);
                swaped = true;
            };
            p2++;
        };
        if (!swaped) return;
        p1++;
    };
};


void part_sort::quick_sort_recursive(int32_t beg, int32_t end, std::vector<std::string>& vec){

    // Base case:
    if(beg >= end){
        return;
    };

    int32_t arr_len = end - beg;
    int32_t mid_index = beg + arr_len / 2;
    int32_t pivot_var = part_sort::process_partition_number(vec[mid_index]);
    std::string tmp_ptr;
    // Initialazing new pointers:
    int32_t i = beg - 1;
    int32_t tmp_i_val = 0;
    int32_t j = end + 1;
    int32_t tmp_j_val = 0;


    while(i < j){
        // Advance phase:
        do{
            i++;
            tmp_i_val = part_sort::process_partition_number(vec[i]);
        } while(tmp_i_val < pivot_var);
        do{
            j--;
            tmp_j_val = part_sort::process_partition_number(vec[j]);
        } while(tmp_j_val > pivot_var);
        // Switch phase:
        if(i < j){
            tmp_ptr = vec[i];
            vec[i] = vec[j];
            vec[j] = tmp_ptr;
        };
    };

    // Out of the loop, Calling recursive helper functions:
    part_sort::quick_sort_recursive(beg, j, vec);
    part_sort::quick_sort_recursive(j+1, end, vec);
};



void part_sort::quick_sort(std::vector<std::string>& vec){
    /*
    Implementation of the Quick Sort algortihm using hoare partitions
    */

    if (vec.empty()) return;

    std::string* ptr_ini = vec.data();
    std::string* ptr_end = ptr_ini + vec.size() - 1;
    int32_t arr_len = ptr_end - ptr_ini;
    int32_t i = 0;
    int32_t j = arr_len;

    // Initializing recursive part:
    part_sort::quick_sort_recursive(i, j, vec);

};
