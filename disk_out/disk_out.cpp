

#include "data_struct.h"
#include <iostream>
#include <fstream>
#include <string>
#include "globals.h"
#include <map>
#include "logging.h"
#include "filesystem"
#include "disk_wal.h"
#include "disk_buffer.h"
#include <cstring> // Para usar std::memcpy
#include "part_sort.h"
#include "disk_aux.h"
#include "disk_out.h"



////////////////////////////////////////////////////////////////////
// ESCRITURA DE DATOS //////////////////////////////////////////////
////////////////////////////////////////////////////////////////////



void disk_out::write_fixed_len_columns(std::string nombre_tabla, std::string column_name, std::vector<int>& vec_vals){

   std::ofstream out;
   out.open("data/" + nombre_tabla + "/" + column_name + ".dat", std::ios::out | std::ios::binary);

   //
   uint32_t size_valores = vec_vals.size() * sizeof(int);
   char* valores_chars = reinterpret_cast<char*>(vec_vals.data());

   out.write(valores_chars, size_valores);
   out.flush();
   out.close();
};

void disk_out::write_fixed_len_columns(std::string nombre_tabla, std::string column_name, std::vector<float>& vec_vals){

   std::ofstream out;
   out.open("data/" + nombre_tabla + "/" + column_name + ".dat", std::ios::out | std::ios::binary);

   //
   uint32_t size_valores = vec_vals.size() * sizeof(float);
   char* valores_chars = reinterpret_cast<char*>(vec_vals.data());

   out.write(valores_chars, size_valores);
   out.flush();
   out.close();
};

void disk_out::write_fixed_len_columns(std::string nombre_tabla, std::string column_name, std::vector<uint8_t>& vec_vals){

   std::ofstream out;
   out.open("data/" + nombre_tabla + "/" + column_name + ".dat", std::ios::out | std::ios::binary);

   //
   uint32_t size_valores = vec_vals.size();
   // Este es un caso especial, iteramos por cadaelemento
   char* valores_chars = reinterpret_cast<char*>(vec_vals.data());

   out.write(valores_chars, size_valores);
   out.flush();
   out.close();
};


void disk_out::fix_vect_vals(std::vector<Values>& input, std::vector<int>& vec_vals){

   uint32_t size_input = input.size();
   for(int i = 0; i<size_input; i++){
      vec_vals.push_back(std::get<int>(input[i]));
   };
};

void disk_out::fix_vect_vals(std::vector<Values>& input, std::vector<float>& vec_vals){

   uint32_t size_input = input.size();
   for(int i = 0; i<size_input; i++){
      vec_vals.push_back(std::get<float>(input[i]));
   };
};

void disk_out::fix_vect_vals(std::vector<Values>& input, std::vector<uint8_t>& vec_vals){

   uint32_t size_input = input.size();
   for(int i = 0; i<size_input; i++){
      if(std::get<bool>(input[i]) == 1){
         vec_vals.push_back(1);
      }else{
         vec_vals.push_back(0);
      };
   };
};

void disk_out::fix_vect_vals(std::vector<Values>& input, std::vector<std::string>& vec_vals, std::vector<uint32_t>& vec_sizes){
   /*
   Caso especial al registrar una string, que no solo almacenamos las
   variables en sí, si no su tamaño o dónde acaba cada una.
   */
   uint32_t size_input = input.size();
   uint32_t acc_size = 0;
   std::string tmp_string;
   for(int i = 0; i<size_input; i++){
      tmp_string = std::get<std::string>(input[i]);
      //acc_size += tmp_string.size();
      acc_size = tmp_string.size();
      vec_vals.push_back(tmp_string);
      vec_sizes.push_back(acc_size);
   };
};

// FUNCIONES AUXILIARES DE LAS FUNCIONES DE ESCRITURA:

void disk_out::calcular_filas_a_escribir_int(int& filas_a_escribir, int size_disponible_para_escribir, int partition_entities, int filas_totales_a_escribir){

   int filas_por_escribir_en_particion = size_disponible_para_escribir / 4;

   if(size_disponible_para_escribir == 0){
      // En caso de ser archivo nuevo:
      if(filas_totales_a_escribir > partition_entities){
         filas_a_escribir = partition_entities;
      }else{
         filas_a_escribir = filas_totales_a_escribir;
      };
      return;
   }else{
      if(filas_por_escribir_en_particion > filas_totales_a_escribir){
         filas_a_escribir = filas_totales_a_escribir;
      }else{
         filas_a_escribir = filas_por_escribir_en_particion;
      };
   };
};


void disk_out::calcular_filas_a_escribir_float(int& filas_a_escribir, int size_disponible_para_escribir, int partition_entities, int filas_totales_a_escribir){

   int filas_por_escribir_en_particion = size_disponible_para_escribir / 4;

   if(size_disponible_para_escribir == 0){
      // En caso de ser archivo nuevo:
      if(filas_totales_a_escribir > partition_entities){
         filas_a_escribir = partition_entities;
      }else{
         filas_a_escribir = filas_totales_a_escribir;
      };
      return;
   }else{
      if(filas_por_escribir_en_particion > filas_totales_a_escribir){
         filas_a_escribir = filas_totales_a_escribir;
      }else{
         filas_a_escribir = filas_por_escribir_en_particion;
      };
   };
};


void disk_out::calcular_filas_a_escribir_bool(int& filas_a_escribir, int size_disponible_para_escribir, int partition_entities, int filas_totales_a_escribir){

   int filas_por_escribir_en_particion = size_disponible_para_escribir;

   if(size_disponible_para_escribir == 0){
      // En caso de ser archivo nuevo:
      if(filas_totales_a_escribir > partition_entities){
         filas_a_escribir = partition_entities;
      }else{
         filas_a_escribir = filas_totales_a_escribir;
      };
      return;
   }else{
      if(filas_por_escribir_en_particion > filas_totales_a_escribir){
         filas_a_escribir = filas_totales_a_escribir;
      }else{
         filas_a_escribir = filas_por_escribir_en_particion;
      };
   };
};


void disk_out::calcular_filas_a_escribir_string(int& filas_a_escribir, int size_disponible_para_escribir, int partition_entities, int filas_totales_a_escribir){

   int filas_por_escribir_en_particion = size_disponible_para_escribir / 4;

   if(size_disponible_para_escribir == 0){
      // En caso de ser archivo nuevo:
      if(filas_totales_a_escribir > partition_entities){
         filas_a_escribir = partition_entities;
      }else{
         filas_a_escribir = filas_totales_a_escribir;
      };
      return;
   }else{
      if(filas_por_escribir_en_particion > filas_totales_a_escribir){
         filas_a_escribir = filas_totales_a_escribir;
      }else{
         filas_a_escribir = filas_por_escribir_en_particion;
      };
   };
};


void disk_out::obtener_indices_vector_valores(auto& inicio, auto& fin, std::vector<Values>*& vec_var_ptr, int filas_escritas, int& filas_a_escribir){
   inicio = vec_var_ptr->begin() + filas_escritas;
   /*filas_a_escribir = 0;
   if(partition_entities > filas_totales_a_escribir){
      filas_a_escribir = filas_totales_a_escribir;
   }else{
      filas_a_escribir = partition_entities;
   };*/
   fin = vec_var_ptr->begin() + filas_escritas + filas_a_escribir;
};

int disk_out::obtener_size_disponible(std::string ruta_variable, std::string ultima_particion_str, int size_particion){

   int size_disponible_para_escribir = 0;
   std::string ruta_escritura = ruta_variable + ultima_particion_str;
   uint32_t size_arhivo = disk_aux::obtener_tamano_archivo(ruta_escritura);
   Logger::log(LogLevel::OUTPUT, "El tamaño del archivo a escribir es: ", false, true);
   Logger::log(LogLevel::OUTPUT, size_arhivo, true, false);
   Logger::flush();
   if(size_arhivo > 0){
      size_disponible_para_escribir = size_particion - size_arhivo;
   }else{
      size_disponible_para_escribir = size_particion;
   };

   return size_disponible_para_escribir;
};


void disk_out::escribir_particion_int(auto& inicio, auto& fin, std::string ruta_escritura){
   std::ofstream out;
   std::vector<Values> sub_vector(inicio, fin);
   Logger::log(LogLevel::OUTPUT, "Empezamos la escritura");
   std::vector<int> vec_vals;
   disk_out::fix_vect_vals(sub_vector, vec_vals);
   Logger::log(LogLevel::OUTPUT, "Hemos aplicado 'fix_vect_vals' con exito");
   out.open(ruta_escritura + ".dat", std::ios::out | std::ios::binary | std::ios::app);
   Logger::log(LogLevel::OUTPUT, "La particion esta abierta");
   uint32_t size_valores = vec_vals.size() * sizeof(int);
   // Este es un caso especial, iteramos por cadaelemento
   char* valores_chars = reinterpret_cast<char*>(vec_vals.data());
   Logger::log(LogLevel::OUTPUT, "Proceedemos a escribir en disco:");
   out.write(valores_chars, size_valores);
   Logger::log(LogLevel::OUTPUT, "Escritura en disco realizada con exito");
   out.flush();
   out.close();
};

void disk_out::escribir_particion_float(auto& inicio, auto& fin, std::string ruta_escritura){
   std::ofstream out;
   std::vector<Values> sub_vector(inicio, fin);
   Logger::log(LogLevel::OUTPUT, "Empezamos la escritura");
   std::vector<float> vec_vals;
   disk_out::fix_vect_vals(sub_vector, vec_vals);
   Logger::log(LogLevel::OUTPUT, "Hemos aplicado 'fix_vect_vals' con exito");
   out.open(ruta_escritura + ".dat", std::ios::out | std::ios::binary | std::ios::app);
   Logger::log(LogLevel::OUTPUT, "La particion esta abierta");
   uint32_t size_valores = vec_vals.size() * sizeof(float);
   // Este es un caso especial, iteramos por cadaelemento
   char* valores_chars = reinterpret_cast<char*>(vec_vals.data());
   Logger::log(LogLevel::OUTPUT, "Proceedemos a escribir en disco:");
   out.write(valores_chars, size_valores);
   Logger::log(LogLevel::OUTPUT, "Escritura en disco realizada con exito");
   out.flush();
   out.close();
};

void disk_out::escribir_particion_bool(auto& inicio, auto& fin, std::string ruta_escritura){
   std::ofstream out;
   std::vector<Values> sub_vector(inicio, fin);
   Logger::log(LogLevel::OUTPUT, "Empezamos la escritura");
   std::vector<uint8_t> vec_vals;
   disk_out::fix_vect_vals(sub_vector, vec_vals); // Ya contempla el caso de llenar un vector de uint8_t provenientes de booleanos
   Logger::log(LogLevel::OUTPUT, "Hemos aplicado 'fix_vect_vals' con exito");
   out.open(ruta_escritura + ".dat", std::ios::out | std::ios::binary | std::ios::app);
   Logger::log(LogLevel::OUTPUT, "La particion esta abierta");
   uint32_t size_valores = vec_vals.size() * sizeof(uint8_t);
   // Este es un caso especial, iteramos por cadaelemento
   char* valores_chars = reinterpret_cast<char*>(vec_vals.data());
   Logger::log(LogLevel::OUTPUT, "Proceedemos a escribir en disco:");
   out.write(valores_chars, size_valores);
   Logger::log(LogLevel::OUTPUT, "Escritura en disco realizada con exito");
   out.flush();
   out.close();
};

void disk_out::escribir_particion_string(auto& inicio, auto& fin, std::string ruta_escritura){
   std::ofstream out1;
   std::ofstream out2;
   std::vector<Values> sub_vector(inicio, fin);
   Logger::log(LogLevel::DEBUG, "Empezamos la escritura");
   std::vector<std::string> vec_vals;
   std::vector<uint32_t> vec_sizes;
   disk_out::fix_vect_vals(sub_vector, vec_vals, vec_sizes);
   Logger::log(LogLevel::DEBUG, "Hemos aplicado 'fix_vect_vals' con exito");
   out1.open(ruta_escritura + ".bin", std::ios::out | std::ios::binary | std::ios::app);
   out2.open(ruta_escritura + ".idx", std::ios::out | std::ios::binary | std::ios::app);
   Logger::log(LogLevel::DEBUG, "La particion esta abierta");
   int acc_size_aux = 0;
   int num_elementos = vec_sizes.size();
   char* valor_a_escribir_1;
   char* valor_a_escribir_2;
   int aux_counter = 1;
   Logger::log(LogLevel::DEBUG, "Comenzamos el bucle de escritura de strings una a una");
   for(int i = 0; i < num_elementos; i++){
      Logger::log(LogLevel::DEBUG, "Nº de iteracion: ", false, true);
      Logger::log(LogLevel::DEBUG, aux_counter, true, false);

      Logger::log(LogLevel::DEBUG, "Proceedemos a escribir en disco una string individual");
      Logger::log(LogLevel::DEBUG, "Definimos el valor de 'valor_a_escribir_1'");
      valor_a_escribir_1 = reinterpret_cast<char*>(vec_vals[i].data());
      Logger::log(LogLevel::DEBUG, "Valor de 'valor_a_escribir_1' definido con exito");
      Logger::log(LogLevel::DEBUG, "Definimos el valor de 'valor_a_escribir_2'");
      valor_a_escribir_2 = reinterpret_cast<char*>(&vec_sizes[i]);
      Logger::log(LogLevel::DEBUG, "Valor de 'valor_a_escribir_2' definido con exito");
      Logger::log(LogLevel::DEBUG, "Escribimos la string");
      out1.write(valor_a_escribir_1, vec_sizes[i]);
      Logger::log(LogLevel::DEBUG, "String escrita con exito");
      Logger::log(LogLevel::DEBUG, "Escribimos el tamano de la string");
      out2.write(valor_a_escribir_2, sizeof(uint32_t));
      Logger::log(LogLevel::DEBUG, "Tamano de la string escrito con exito");
      aux_counter++;
   };
   Logger::log(LogLevel::DEBUG, "Escritura en disco realizada con exito");
   out1.flush();
   out1.close();
   out2.flush();
   out2.close();
};


void disk_out::bucle_escritura_int(std::vector<Values>*& vec_var_ptr, int bytes_totales_a_escribir, int partition_entities, std::string ruta_variable, std::string ultima_particion_str, int ultima_particion_int){

   // Estas son las variables que usaremos para trackear lo que llevamos avanzado
   int bytes_escritos = 0;
   int filas_escritas = 0;
   int filas_totales_a_escribir = bytes_totales_a_escribir / 4;
   int size_particion = partition_entities * sizeof(int);
   Logger::flush();

   Logger::log(LogLevel::DEBUG, "Entramos en la funcion de escritura de INTs");
   Logger::log(LogLevel::DEBUG, "Tamano de la particion: ", false, true);
   Logger::log(LogLevel::DEBUG, size_particion, false, true);

   // Ahora consultamos la última partición para saber realmente en que númerio estamos:
   std::vector<Values>::iterator inicio;
   std::vector<Values>::iterator fin;
   int filas_a_escribir;
   int bytes_a_escribir;
   int size_disponible_para_escribir = 0;

   int filas_que_quedan_por_escribir = 0;
   int bytes_que_quedan_por_escribir = 0;
   int filas_que_caben_en_particion_actual = 0;
   
   while(bytes_totales_a_escribir > bytes_escritos){
      /*
      1) Consultamos el tamaño que le queda a la partición por escribir
         Si no existiera, se asigna el tmaño de una partición entera
         (del tipo concreto)
      */
      size_disponible_para_escribir = disk_out::obtener_size_disponible(ruta_variable,
                                                                       ultima_particion_str + ".dat",
                                                                       size_particion
                                                                      );
      Logger::log(LogLevel::DEBUG, "El tamaño disponible en la particion es: ", false, true);
      Logger::log(LogLevel::DEBUG, size_disponible_para_escribir, true, false);
      // 1) Calculamos las filas a escribir:
      filas_que_quedan_por_escribir = filas_totales_a_escribir - filas_escritas;
      bytes_que_quedan_por_escribir = bytes_totales_a_escribir - bytes_escritos;
      filas_que_caben_en_particion_actual = size_disponible_para_escribir / sizeof(int);
      if(filas_que_quedan_por_escribir > filas_que_caben_en_particion_actual){
         filas_a_escribir = filas_que_caben_en_particion_actual;
         bytes_a_escribir = size_disponible_para_escribir;
      }else{
         filas_a_escribir = filas_que_quedan_por_escribir;
         bytes_a_escribir = bytes_que_quedan_por_escribir;
      };
      Logger::log(LogLevel::DEBUG, "Filas que quedan pro escribir: ", false, true);
      Logger::log(LogLevel::DEBUG, filas_a_escribir, true, false);
      if(filas_a_escribir > 0){
         // Sacamos los índices inicial y final;
         disk_out::obtener_indices_vector_valores(inicio,
                                                fin,
                                                vec_var_ptr,
                                                filas_escritas,
                                                filas_a_escribir
                                                );

         // 3) Realizamos la escritura
         std::string ruta_escritura = ruta_variable + ultima_particion_str;
         disk_out::escribir_particion_int(inicio, fin, ruta_escritura);
            // Una vez hecha la escritura, actualizamos los contadores
            // 4) Actualización de los contadores:
            bytes_escritos += bytes_a_escribir;
            filas_escritas += filas_a_escribir;
         };
         // Actualizamos el nombre de las particiones:
         Logger::log(LogLevel::DEBUG, "Valor de 'ultima_particion_str' antes: ", false, true);
         Logger::log(LogLevel::DEBUG, ultima_particion_str, true, false);
         ultima_particion_int += 1;
         ultima_particion_str = std::to_string(ultima_particion_int);
         Logger::log(LogLevel::DEBUG, "Valor de 'ultima_particion_str' despues: ", false, true);
         Logger::log(LogLevel::DEBUG, ultima_particion_str, true, false);
   };
};

void disk_out::bucle_escritura_float(std::vector<Values>*& vec_var_ptr, int bytes_totales_a_escribir, int partition_entities, std::string ruta_variable, std::string ultima_particion_str, int ultima_particion_int){

   // Estas son las variables que usaremos para trackear lo que llevamos avanzado
   int bytes_escritos = 0;
   int filas_escritas = 0;
   int filas_totales_a_escribir = bytes_totales_a_escribir / 4;
   int size_particion = partition_entities * sizeof(float);
   Logger::flush();

   Logger::log(LogLevel::DEBUG, "Entramos en la funcion de escritura de FLOATs");
   Logger::log(LogLevel::DEBUG, "Tamano de la particion: ", false, true);
   Logger::log(LogLevel::DEBUG, size_particion, false, true);

   // Ahora consultamos la última partición para saber realmente en que númerio estamos:
   std::vector<Values>::iterator inicio;
   std::vector<Values>::iterator fin;
   int filas_a_escribir;
   int bytes_a_escribir;
   int size_disponible_para_escribir = 0;

   int filas_que_quedan_por_escribir = 0;
   int bytes_que_quedan_por_escribir = 0;
   int filas_que_caben_en_particion_actual = 0;
   
   while(bytes_totales_a_escribir > bytes_escritos){
      /*
      1) Consultamos el tamaño que le queda a la partición por escribir
         Si no existiera, se asigna el tmaño de una partición entera
         (del tipo concreto)
      */
      size_disponible_para_escribir = disk_out::obtener_size_disponible(ruta_variable,
                                                                       ultima_particion_str + ".dat",
                                                                       size_particion
                                                                      );
      Logger::log(LogLevel::DEBUG, "El tamaño disponible en la particion es: ", false, true);
      Logger::log(LogLevel::DEBUG, size_disponible_para_escribir, true, false);
      // 1) Calculamos las filas a escribir:
      filas_que_quedan_por_escribir = filas_totales_a_escribir - filas_escritas;
      bytes_que_quedan_por_escribir = bytes_totales_a_escribir - bytes_escritos;
      filas_que_caben_en_particion_actual = size_disponible_para_escribir / sizeof(float);
      if(filas_que_quedan_por_escribir > filas_que_caben_en_particion_actual){
         filas_a_escribir = filas_que_caben_en_particion_actual;
         bytes_a_escribir = size_disponible_para_escribir;
      }else{
         filas_a_escribir = filas_que_quedan_por_escribir;
         bytes_a_escribir = bytes_que_quedan_por_escribir;
      };
      Logger::log(LogLevel::DEBUG, "Filas que quedan pro escribir: ", false, true);
      Logger::log(LogLevel::DEBUG, filas_a_escribir, true, false);
      if(filas_a_escribir > 0){
         // Sacamos los índices inicial y final;
         disk_out::obtener_indices_vector_valores(inicio,
                                                fin,
                                                vec_var_ptr,
                                                filas_escritas,
                                                filas_a_escribir
                                                );

         // 3) Realizamos la escritura
         std::string ruta_escritura = ruta_variable + ultima_particion_str;
         disk_out::escribir_particion_float(inicio, fin, ruta_escritura);
         // Una vez hecha la escritura, actualizamos los contadores
         // 4) Actualización de los contadores:
         bytes_escritos += bytes_a_escribir;
         filas_escritas += filas_a_escribir;
      };
      // Actualizamos el nombre de las particiones:
      Logger::log(LogLevel::DEBUG, "Valor de 'ultima_particion_str' antes: ", false, true);
      Logger::log(LogLevel::DEBUG, ultima_particion_str, true, false);
      ultima_particion_int += 1;
      ultima_particion_str = std::to_string(ultima_particion_int);
      Logger::log(LogLevel::DEBUG, "Valor de 'ultima_particion_str' despues: ", false, true);
      Logger::log(LogLevel::DEBUG, ultima_particion_str, true, false);
   };
};


void disk_out::bucle_escritura_bool(std::vector<Values>*& vec_var_ptr, int bytes_totales_a_escribir, int partition_entities, std::string ruta_variable, std::string ultima_particion_str, int ultima_particion_int){

   // Estas son las variables que usaremos para trackear lo que llevamos avanzado
   int bytes_escritos = 0;
   int filas_escritas = 0;
   int filas_totales_a_escribir = bytes_totales_a_escribir;
   int size_particion = partition_entities;
   Logger::flush();

   Logger::log(LogLevel::DEBUG, "Entramos en la funcion de escritura de BOOLs");
   Logger::log(LogLevel::DEBUG, "Tamano de la particion: ", false, true);
   Logger::log(LogLevel::DEBUG, size_particion, false, true);

   // Ahora consultamos la última partición para saber realmente en que númerio estamos:
   std::vector<Values>::iterator inicio;
   std::vector<Values>::iterator fin;
   int filas_a_escribir;
   int bytes_a_escribir;
   int size_disponible_para_escribir = 0;

   int filas_que_quedan_por_escribir = 0;
   int bytes_que_quedan_por_escribir = 0;
   int filas_que_caben_en_particion_actual = 0;
   
   while(bytes_totales_a_escribir > bytes_escritos){
      /*
      1) Consultamos el tamaño que le queda a la partición por escribir
         Si no existiera, se asigna el tmaño de una partición entera
         (del tipo concreto)
      */
      size_disponible_para_escribir = disk_out::obtener_size_disponible(ruta_variable,
                                                                       ultima_particion_str + ".dat",
                                                                       size_particion
                                                                      );
      Logger::log(LogLevel::DEBUG, "El tamaño disponible en la particion es: ", false, true);
      Logger::log(LogLevel::DEBUG, size_disponible_para_escribir, true, false);
      // 1) Calculamos las filas a escribir:
      filas_que_quedan_por_escribir = filas_totales_a_escribir - filas_escritas;
      bytes_que_quedan_por_escribir = bytes_totales_a_escribir - bytes_escritos;
      filas_que_caben_en_particion_actual = size_disponible_para_escribir / sizeof(uint8_t);
      if(filas_que_quedan_por_escribir > filas_que_caben_en_particion_actual){
         filas_a_escribir = filas_que_caben_en_particion_actual;
         bytes_a_escribir = size_disponible_para_escribir;
      }else{
         filas_a_escribir = filas_que_quedan_por_escribir;
         bytes_a_escribir = bytes_que_quedan_por_escribir;
      };
      Logger::log(LogLevel::DEBUG, "Filas que quedan pro escribir: ", false, true);
      Logger::log(LogLevel::DEBUG, filas_a_escribir, true, false);
      if(filas_a_escribir > 0){
         // Sacamos los índices inicial y final;
         disk_out::obtener_indices_vector_valores(inicio,
                                                fin,
                                                vec_var_ptr,
                                                filas_escritas,
                                                filas_a_escribir
                                                );

         // 3) Realizamos la escritura
         std::string ruta_escritura = ruta_variable + ultima_particion_str;
         disk_out::escribir_particion_bool(inicio, fin, ruta_escritura);
         // Una vez hecha la escritura, actualizamos los contadores
         // 4) Actualización de los contadores:
         bytes_escritos += bytes_a_escribir;
         filas_escritas += filas_a_escribir;
      };
      // Actualizamos el nombre de las particiones:
      Logger::log(LogLevel::DEBUG, "Valor de 'ultima_particion_str' antes: ", false, true);
      Logger::log(LogLevel::DEBUG, ultima_particion_str, true, false);
      ultima_particion_int += 1;
      ultima_particion_str = std::to_string(ultima_particion_int);
      Logger::log(LogLevel::DEBUG, "Valor de 'ultima_particion_str' despues: ", false, true);
      Logger::log(LogLevel::DEBUG, ultima_particion_str, true, false);
   };
};


void disk_out::bucle_escritura_string(std::vector<Values>*& vec_var_ptr, int bytes_totales_a_escribir, int partition_entities, std::string ruta_variable, std::string ultima_particion_str, int ultima_particion_int){

   // Estas son las variables que usaremos para trackear lo que llevamos avanzado
   int bytes_escritos = 0;
   int filas_escritas = 0;
   int filas_totales_a_escribir = bytes_totales_a_escribir / 4;
   int size_particion = partition_entities * sizeof(uint32_t);
   Logger::flush();

   Logger::log(LogLevel::DEBUG, "Entramos en la funcion de escritura de INTs");
   Logger::log(LogLevel::DEBUG, "Tamano de la particion: ", false, true);
   Logger::log(LogLevel::DEBUG, size_particion, false, true);

   // Ahora consultamos la última partición para saber realmente en que númerio estamos:
   std::vector<Values>::iterator inicio;
   std::vector<Values>::iterator fin;
   int filas_a_escribir;
   int bytes_a_escribir;
   int size_disponible_para_escribir = 0;

   int filas_que_quedan_por_escribir = 0;
   int bytes_que_quedan_por_escribir = 0;
   int filas_que_caben_en_particion_actual = 0;
   
   while(bytes_totales_a_escribir > bytes_escritos){
      /*
      1) Consultamos el tamaño que le queda a la partición por escribir
         Si no existiera, se asigna el tmaño de una partición entera
         (del tipo concreto)
      */
      size_disponible_para_escribir = disk_out::obtener_size_disponible(ruta_variable,
                                                                       ultima_particion_str + ".idx",
                                                                       size_particion
                                                                      );
      Logger::log(LogLevel::DEBUG, "El tamaño disponible en la particion es: ", false, true);
      Logger::log(LogLevel::DEBUG, size_disponible_para_escribir, true, false);
      // 1) Calculamos las filas a escribir:
      filas_que_quedan_por_escribir = filas_totales_a_escribir - filas_escritas;
      bytes_que_quedan_por_escribir = bytes_totales_a_escribir - bytes_escritos;
      filas_que_caben_en_particion_actual = size_disponible_para_escribir / sizeof(uint32_t);
      if(filas_que_quedan_por_escribir > filas_que_caben_en_particion_actual){
         filas_a_escribir = filas_que_caben_en_particion_actual;
         bytes_a_escribir = size_disponible_para_escribir;
      }else{
         filas_a_escribir = filas_que_quedan_por_escribir;
         bytes_a_escribir = bytes_que_quedan_por_escribir;
      };
      Logger::log(LogLevel::DEBUG, "Filas que quedan pro escribir: ", false, true);
      Logger::log(LogLevel::DEBUG, filas_a_escribir, true, false);
      if(filas_a_escribir > 0){
         // Sacamos los índices inicial y final;
         disk_out::obtener_indices_vector_valores(inicio,
                                                fin,
                                                vec_var_ptr,
                                                filas_escritas,
                                                filas_a_escribir
                                                );

         // 3) Realizamos la escritura
         std::string ruta_escritura = ruta_variable + ultima_particion_str;
         disk_out::escribir_particion_string(inicio, fin, ruta_escritura);
         // Una vez hecha la escritura, actualizamos los contadores
         // 4) Actualización de los contadores:
         bytes_escritos += bytes_a_escribir;
         filas_escritas += filas_a_escribir;
      };
      // Actualizamos el nombre de las particiones:
      Logger::log(LogLevel::DEBUG, "Valor de 'ultima_particion_str' antes: ", false, true);
      Logger::log(LogLevel::DEBUG, ultima_particion_str, true, false);
      ultima_particion_int += 1;
      ultima_particion_str = std::to_string(ultima_particion_int);
      Logger::log(LogLevel::DEBUG, "Valor de 'ultima_particion_str' despues: ", false, true);
      Logger::log(LogLevel::DEBUG, ultima_particion_str, true, false);
   };
};




void disk_out::write_table_data(table* tabla){

   Logger::log(LogLevel::DEBUG, "<<<ENTRAMOS EN LA FUNCION DE ESCRITURA POR PARTICIONES>>>>");

   // Recuperamos los tipos de datos:
   std::vector<dataType> tipo_columnas = tabla->metadata_ptr->column_types;
   // Recuperamos el nombre de la columna:
   std::vector<std::string> nombre_columnas = tabla->metadata_ptr->column_names;
   // Recuperamos el vector de vectores de los datos:
   std::map<std::string, std::vector<Values>> vec_de_vec_vals = tabla->data_ptr->columns;
   // Recuperamos el nombre de la tabla:
   std::string nombre_tabla = tabla->metadata_ptr->name;
   Logger::log(LogLevel::DEBUG, "Valores preliminares recuperados");

   // Antes de nada, creamos el directorio de la tabla:
   std::filesystem::path ruta_tabla = "data/" + nombre_tabla;
   std::filesystem::create_directories(ruta_tabla);
   Logger::log(LogLevel::DEBUG, "Directorio de la tabla creado");

   // Ahora, examinamos cuantas particiones hay:
   std::vector<std::string> particiones_nombres;
   particiones_nombres = disk_aux::obtener_archivos_en_ruta("data/" + nombre_tabla + "/" + nombre_columnas[0], tipo_columnas[0]);
   std::string last_partition = "";
   //////////////////////////////////////////////////////////////////////
   // Obtenemos la última partición:
   std::string ultima_particion_str = "0";
   int ultima_particion_int = 0;


   if(!particiones_nombres.empty()){
      Logger::log(LogLevel::DEBUG, "Hacemos bubble sort");
      part_sort::bubble_sort(particiones_nombres);
      Logger::log(LogLevel::DEBUG, "Bubble sort realizado con exito");
      int parts = particiones_nombres.size();
      last_partition = particiones_nombres[parts-1];
      if(last_partition != ""){
         Logger::log(LogLevel::DEBUG, "La ultima particion no es '' ");
         Logger::log(LogLevel::DEBUG, "El valor de 'last_partition' es: ", false, true);
         Logger::log(LogLevel::DEBUG, last_partition, true, false);
         ultima_particion_str = part_sort::procesar_numero_particion_como_str(last_partition);
         Logger::log(LogLevel::DEBUG, "Valor de 'ultima_particion_str': ", false, true);
         Logger::log(LogLevel::DEBUG, ultima_particion_str, false, true);
         ultima_particion_int = std::stoi(ultima_particion_str);
      };
   };

   Logger::log(LogLevel::OUTPUT, "Encontrada la ultima particion: ", false, true);
   Logger::log(LogLevel::OUTPUT, ultima_particion_str, true, false);
   // Ahora obtenemos el ultimo numero en forma de string y entero, para hacerlo sólo una vez:


   // VARIABLES NECESARIAS ANTES DEL BUCLE:
   uint32_t num_cols = tipo_columnas.size();
   int bytes_totales_a_escribir = 0; // Cantidad que nos servirá poara ver cuánto tenemos que escribir:
   int partition_entities = 3;

   // Iteramos por cada elemento
   for(int i = 0; i<num_cols; i++){
      // Iteramos por cada columna:
      dataType& tipo_dato = tipo_columnas[i];
      std::string& column_name = nombre_columnas[i];
      std::vector<Values>* vec_var_ptr = &(vec_de_vec_vals.at(column_name));
      // Creamos el directorio de cada varuiable de la tabla:
      std::string ruta_columna = "data/" + nombre_tabla + "/" + column_name;
      std::filesystem::create_directories(ruta_columna);
      std::string ruta_variable = "data/" + nombre_tabla + "/" + column_name + "/" + "part_";

      switch(tipo_dato){
         case dataType::INT: {
            bytes_totales_a_escribir = vec_var_ptr->size() * 4; //  Tamaño total de lo que hay que escribir

            disk_out::bucle_escritura_int(vec_var_ptr, bytes_totales_a_escribir, partition_entities, ruta_variable, ultima_particion_str, ultima_particion_int);
            break;
         };
         case dataType::FLOAT: {
            bytes_totales_a_escribir = vec_var_ptr->size() * 4; //  Tamaño total de lo que hay que escribir

            disk_out::bucle_escritura_float(vec_var_ptr, bytes_totales_a_escribir, partition_entities, ruta_variable, ultima_particion_str, ultima_particion_int);
            break;
         };
         case dataType::BOOL: {
            bytes_totales_a_escribir = vec_var_ptr->size();
            disk_out::bucle_escritura_bool(vec_var_ptr, bytes_totales_a_escribir, partition_entities, ruta_variable, ultima_particion_str, ultima_particion_int);
            break;
         };
         case dataType::STRING: {
            bytes_totales_a_escribir = vec_var_ptr->size() * 4;
            disk_out::bucle_escritura_string(vec_var_ptr, bytes_totales_a_escribir, partition_entities, ruta_variable, ultima_particion_str, ultima_particion_int);
            break;
         };
      };
   };
};