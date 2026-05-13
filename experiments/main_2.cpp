#include <string>
#include <iostream>
#include <vector>

// 1. Función para extraer el número (Mejorada con punteros)
std::string obtain_number(std::string input) {
    char* p = input.data();
    char* ptr_end = p + input.size();
    std::string resultado = "";
    bool capturar = false;

    char* aux_ptr = p;
    while (aux_ptr < ptr_end) {
        if (*aux_ptr == '.') break; // Detenerse en la extensión
        
        if (capturar) {
            resultado += *aux_ptr; // Guardar el número
        }

        if (*aux_ptr == '_') {
            capturar = true; // Empezar a capturar DESPUÉS del guion
        }
        aux_ptr++;
    };
    return resultado;
};

std::string quitar_ceros(std::string input){
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
}


int procesar_numero_particion(std::string input){
    std::string tmp;
    tmp = obtain_number(input);
    return std::stoi(quitar_ceros(tmp));
};


bool compare_ints(int num_1, int num_2){
    if(num_1 > num_2){
        return true;
    };
    return false;
};

bool compare_partition_values(std::string string_1, std::string string_2){
    int num_1 = procesar_numero_particion(string_1);
    int num_2 = procesar_numero_particion(string_2);
    return compare_ints(num_1, num_2);
};


// 2. Intercambio de strings
void swap_string_vals(std::string* p1, std::string* p2){
    std::string aux = *p1;
    *p1 = *p2;
    *p2 = aux;
};

// 3. Bubble Sort con aritmética de punteros correcta
void bubble_sort(std::vector<std::string>& vec){
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
            if(compare_partition_values(*p2, *(p2 + 1))){
                swap_string_vals(p2, p2 + 1);
                swaped = true;
            };
            p2++;
        };
        if (!swaped) return;
        p1++;
    };
};

int main(){

    std::vector<std::string> particiones = {
        "part_05002.dat",
        "part_00010.dat",
        "part_01023.dat",
        "part_00001.dat",
        "part_09999.dat",
        "part_00450.dat",
        "part_00005.dat",
        "part_02100.dat"
    };
    int vector_size = particiones.size();
    bubble_sort(particiones);

    for(int i = 0; i < vector_size; i++){
        std::cout<<particiones[i]<<std::endl;
        
    };
    std::cout<<std::endl;

    return 0;
}