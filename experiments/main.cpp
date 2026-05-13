#include <string>
#include <iostream>
#include <vector>


void recursive_merge_sort(int* ptr_ini, int* ptr_end, int aux_counter){


    std::cout<<std::endl;
    std::cout<<"Llamada recursiva: ";
    std::cout<<aux_counter<<std::endl;
    aux_counter++;
    std::cout<<"Valor del puntero inicial: ";
    std::cout<<*ptr_ini<<std::endl;
    std::cout<<"Valor del puntero final: ";
    std::cout<<*ptr_end<<std::endl;

    int vec_len = ptr_end - ptr_ini;
    std::cout<<"Longitud del vector: ";
    std::cout<<vec_len<<std::endl;
    int vec_mitad = vec_len / 2;
    int* ptr_mitad = ptr_ini + (vec_len / 2);
   std::cout<<"Valor del puntero mitad: ";
    std::cout<<*ptr_mitad<<std::endl;
    if(vec_len > 1){
        recursive_merge_sort(ptr_ini, ptr_mitad, aux_counter);
        recursive_merge_sort(ptr_mitad, ptr_end, aux_counter);
    }else{
        return;
    };
    // Acción que hacer una vez ya llegó al final de la recursión:
    /*int* ptr_aux = ptr_ini;
    int* ptr_aux_2 = ptr_ini;
    while(ptr_aux < ptr_mitad){
        if(*ptr_aux > *(ptr_aux + vec_mitad)){
            int aux_val = *ptr_aux;
            *ptr_aux = *(ptr_aux + vec_mitad);
            *(ptr_aux + vec_mitad) = aux_val;
        };
        ptr_aux++;
    };
    std::cout<<"En la reconstruccion, queda el vector:";
    std::cout<<*ptr_end<<std::endl;
    while(ptr_aux_2 < ptr_mitad){
        std::cout<<*ptr_aux_2;
        std::cout<<" , ";
        ptr_aux_2++;
    };
    std::cout<<std::endl;*/
    int* ptr_aux = ptr_ini;
    int offset;
    while(ptr_aux < ptr_mitad){
        offset = ptr_aux - ptr_ini;
        if(*ptr_aux > *(ptr_mitad + offset)){
            int aux_val = *ptr_aux;
            *ptr_aux = *(ptr_mitad + offset);
            *(ptr_mitad + offset) = aux_val;
        };
        ptr_aux++;
    };
    return;
};


void merge_sort(std::vector<int>& vec){

    int aux_counter = 1;

    int* ptr_ini = vec.data();
    int* ptr_end = ptr_ini + vec.size();
    recursive_merge_sort(ptr_ini, ptr_end, aux_counter);
};


// 1. Intercambio corregido (usando un valor temporal, no un puntero vacío)
void swap_poniter_vals(int* p1, int* p2){
    int aux = *p1;
    *p1 = *p2;
    *p2 = aux;
}

void bubble_sort(std::vector<int>& vec){
    if(vec.empty()) return;

    int* ptr_ini = vec.data();
    int* ptr_end = ptr_ini + vec.size() - 1;
    int* ptr_aux_1 = ptr_ini;

    while(ptr_aux_1 < ptr_end){
        bool swaped = false;
        // Calculamos el límite dinámico
        int* tmp_ptr = ptr_end - (ptr_aux_1 - ptr_ini);
        
        int* ptr_aux_2 = ptr_ini;
        while(ptr_aux_2 < tmp_ptr){
            // 2. Comparamos el actual con el SIGUIENTE
            if(*ptr_aux_2 > *(ptr_aux_2 + 1)){
                swap_poniter_vals(ptr_aux_2, ptr_aux_2 + 1);
                swaped = true;
            }
            ptr_aux_2++;
        }

        // 3. El check de optimización va AQUÍ (fuera del bucle interno)
        if(!swaped) return; 

        ptr_aux_1++;
    };
};



int main(){

    std::vector<int> numeros = {3, 7, 1, 10, 4, 8, 2, 5};

    std::cout<<"Vector antes de la ordenacion:"<<std::endl;
    uint32_t vec_size = numeros.size();
    for(int i = 0; i<vec_size; i++){
        std::cout<<numeros[i];
        std::cout<<" , ";
    };
    std::cout<<std::endl;
    std::cout<<std::endl;
    std::cout<<"Ejecutamos el Merge Sort:"<<std::endl;
    /*merge_sort(numeros);*/
    bubble_sort(numeros);
    std::cout<<std::endl;
    std::cout<<"Merge Sort ejecutado con exito"<<std::endl;
    std::cout<<"El vector resultante es este:"<<std::endl;
    for(int i = 0; i<vec_size; i++){
        std::cout<<numeros[i];
        std::cout<<" , ";
    };
    std::cout<<std::endl;

    return 0;
};