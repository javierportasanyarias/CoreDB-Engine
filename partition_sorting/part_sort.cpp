#include "part_sort.h"


std::string part_sort::quitar_extension_archivo(std::string& input){

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


// 1. Función para extraer el número (Mejorada con punteros)
/*std::string part_sort::obtain_number(std::string input) {
    size_t pos_guion = input.find('_');
    size_t pos_punto = input.find('.');

    // Si no encuentra el guion, devolvemos "0" para evitar que stoi explote
    if (pos_guion == std::string::npos) return "0";

    // Extraemos la subcadena. 
    // Empezamos en pos_guion + 1
    // La longitud es (pos_punto - pos_guion - 1)
    std::string resultado;
    if (pos_punto != std::string::npos && pos_punto > pos_guion) {
        resultado = input.substr(pos_guion + 1, pos_punto - pos_guion - 1);
    } else {
        resultado = input.substr(pos_guion + 1);
    }

    return resultado;
}*/

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
}

/*std::string part_sort::quitar_ceros(std::string input){
    char* p = input.data();
    char* ptr_end = p + input.size();
    std::string resultado = "";
    bool aux_bool = false;
    char* aux_ptr = p;
    while (aux_ptr < ptr_end) {
        if(aux_bool){
            resultado += *aux_ptr;
        };
        if (*aux_ptr == '0' && !aux_bool){
            aux_bool = true;
        };
        aux_ptr++;
    }
    return resultado;
}*/


std::string part_sort::quitar_ceros(std::string input){
    
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
}




int part_sort::procesar_numero_particion(std::string input) {
    std::string numero_str = part_sort::obtain_number(input);
    
    if (numero_str.empty()) return 0;

    try {
        // stoi convierte "0001" en 1, "0012" en 12, etc.
        return std::stoi(numero_str);
    } catch (...) {
        // Si hay basura en el nombre del archivo, devolvemos 0 para no crashear
        return 0;
    }
}

std::string part_sort::procesar_numero_particion_como_str(std::string input){
    std::string tmp;
    tmp = part_sort::obtain_number(input);
    Logger::log(LogLevel::DEBUG, "El numero de la particion es: ", false, true);
    Logger::log(LogLevel::DEBUG, tmp, true, false);
    Logger::log(LogLevel::DEBUG, "Ahora le quitamos los ceros");
    tmp = part_sort::quitar_ceros(tmp);
    Logger::log(LogLevel::DEBUG, "Después de quitar los ceros, se queda en: ", false, true);
    Logger::log(LogLevel::DEBUG, tmp, true, false);
    return tmp;
};


bool part_sort::compare_ints(int num_1, int num_2){
    if(num_1 > num_2){
        return true;
    };
    return false;
};

bool part_sort::compare_partition_values(std::string string_1, std::string string_2) {
    int num_1 = part_sort::procesar_numero_particion(string_1);
    int num_2 = part_sort::procesar_numero_particion(string_2);
    return num_1 > num_2; // Devuelve true si el primero es mayor (para el swap del bubble)
}


// 2. Intercambio de strings
void part_sort::swap_string_vals(std::string* p1, std::string* p2){
    std::string aux = *p1;
    *p1 = *p2;
    *p2 = aux;
};

// 3. Bubble Sort con aritmética de punteros correcta
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
            //if (*p2 > *(p2 + 1)) {
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

    // For getting the number:
    //part_sort::procesar_numero_particion(string_1)


    int32_t arr_len = end - beg;
    int32_t mid_index = beg + arr_len / 2;
    int32_t pivot_var = part_sort::procesar_numero_particion(vec[mid_index]);
    std::string tmp_ptr;
    // We initialize the new pointers
    int32_t i = beg - 1;
    int32_t tmp_i_val = 0;
    int32_t j = end + 1;
    int32_t tmp_j_val = 0;


    while(i < j){
        // Advance phase:
        do{
            i++;
            tmp_i_val = part_sort::procesar_numero_particion(vec[i]);
        } while(tmp_i_val < pivot_var);
        do{
            j--;
            tmp_j_val = part_sort::procesar_numero_particion(vec[j]);
        } while(tmp_j_val > pivot_var);
        //Switch phase:
        if(i < j){
            tmp_ptr = vec[i];
            vec[i] = vec[j];
            vec[j] = tmp_ptr;
        };
    };

    // out of the loop we call te recursive
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

    // We initialize the recursion:
    part_sort::quick_sort_recursive(i, j, vec);

};