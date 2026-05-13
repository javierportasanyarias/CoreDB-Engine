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


////////////////////////////////////////////////////////////////////
// FUNCIONES AUXILIARES ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void disk_aux::aux_vector_buffer_write_disk(std::vector<char>& buffer, std::ofstream& out){

   if (out.is_open()) {
         // Escribimos todo el contenido una sola vez
         out.write(buffer.data(), buffer.size());
   };
};

void disk_aux::aux_vector_buffer_write_disk(std::vector<char>& buffer, std::fstream& out){

   if (out.is_open()) {
         // Escribimos todo el contenido una sola vez
         out.write(buffer.data(), buffer.size());
   };
};

////////////////////////////////////////////////////////////////////
// FUNCIONES A DESCARTAR ///////////////////////////////////////////
////////////////////////////////////////////////////////////////////

Values disk_aux::read_aux_val(dataType tipo_dato, std::ifstream& in){

   Values value;
   switch(tipo_dato){

      case dataType::INT: {
         int buffer;
         in.read(reinterpret_cast<char*>(&buffer), sizeof(int));
         value = buffer;
         break;
      };
      case dataType::FLOAT: {
         float buffer;
         in.read(reinterpret_cast<char*>(&buffer), sizeof(float));
         value = buffer;
         break;
      };
      case dataType::BOOL: {
         bool buffer_bool;
         uint8_t buffer_int8;
         in.read(reinterpret_cast<char*>(&buffer_int8), sizeof(uint8_t));
         if(buffer_int8==1){
            buffer_bool = true;
         }else{
            buffer_bool = false;
         };
         value = buffer_bool;
         break;
      };
      case dataType::STRING: {
         std::string buffer;
         uint32_t string_size;
         // Primero escribimos el tamaño de la string:
         in.read(reinterpret_cast<char*>(&string_size), sizeof(uint32_t));
         // Ajustamos el tamaño de la cadena de texto donde escribiremos:
         buffer.resize(string_size);   // ← reservar memoria
         // Ahora ya si escribimos la cadena de texto:
         in.read(buffer.data(), string_size);
         /* No es necesario 'reinterpret_cast<char*>' porque data()
         de una std::string ya es un puntero a un caracter de texto
         */
         value = buffer;
         break;
      };
   };
   return value;
};

////////////////////////////////////////////////////////////////////
// FUNCIONES AUXILIARES ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////




/*std::vector<std::string> disk_aux::obtener_archivos_en_ruta(std::string ruta) {
    std::vector<std::string> archivos;

    // 1. Verificamos si la ruta existe y si es realmente un directorio
    if (!fs::exists(ruta) || !fs::is_directory(ruta)) {
        return archivos; // Devuelve vector vacío si no existe
    }

    // 2. Iteramos sobre los elementos de la carpeta
    for (const auto& entrada : fs::directory_iterator(ruta)) {
        // Verificamos que sea un archivo regular (no otra carpeta)
        if (fs::is_regular_file(entrada.path())) {
            // Guardamos solo el nombre del archivo (ej: "part_00001.dat")
            archivos.push_back(entrada.path().filename().string());
        }
    }

    return archivos;
}*/


std::vector<std::string> disk_aux::obtener_archivos_en_ruta(std::string ruta, dataType tipo_dato) {
   std::vector<std::string> archivos;

   // 1. Verificamos si la ruta existe y si es realmente un directorio
   if (!fs::exists(ruta) || !fs::is_directory(ruta)) {
      return archivos; // Devuelve vector vacío si no existe
   }

   // 2. Iteramos sobre los elementos de la carpeta
   for (const auto& entrada : fs::directory_iterator(ruta)) {
      // Verificamos que sea un archivo regular (no otra carpeta)
      if (!fs::is_regular_file(entrada.path())){
      continue;
      };
      if(tipo_dato == dataType::STRING){
         if(entrada.path().extension() == ".idx"){
            // Sólo guardamos la partición si es de una extensión concreta
            archivos.push_back(entrada.path().filename().string());
         };
      }else{
         // Guardamos solo el nombre del archivo (ej: "part_00001.dat")
         archivos.push_back(entrada.path().filename().string());
      };
   };

   return archivos;
};


uint32_t disk_aux::obtener_tamano_archivo(std::string ruta) {
    // Abrimos el archivo:
    // binary: Para que no haya conversiones de caracteres extrañas
    // ate: "At the end", para situar el puntero directamente al final
    std::ifstream archivo(ruta, std::ios::binary | std::ios::ate);

    if (!archivo.is_open()) {
        return 0; // Error al abrir
    }

    // tellg() devuelve la posición actual del cursor. 
    // Como lo abrimos con 'ate', el cursor está al final, 
    // dándonos el tamaño total en bytes.
    long tamano = archivo.tellg();

    archivo.close();
    return tamano;
};



void disk_aux::write_aux_val_buffer(Values value, dataType tipo_dato, std::vector<char>& buffer){

   /*
   Función parecida a 'write_aux_val', pero esta vez escribe un buffer de carácteres.
   Dejándolo listo para así escribir el buffer en una sola llamada al disco,
   reduciendo latencia de interacción con el disco en escritura.
   Si se lee este mismo buffer se ahorrará también significativamente en tiempo de
   lectura.
   */

   char* tmp_char_ptr = nullptr;

   switch(tipo_dato){

      case dataType::INT: {
         int int_val;
         int_val = std::get<int>(value);
         tmp_char_ptr = reinterpret_cast<char*>(&int_val);
         //out.write(reinterpret_cast<char*>(&buffer), sizeof(int));
         buffer.insert(buffer.end(),
                     tmp_char_ptr,
                     tmp_char_ptr + sizeof(int)
                     );
         break;
      };
      case dataType::FLOAT: {
         float float_val;
         float_val = std::get<float>(value);
         tmp_char_ptr = reinterpret_cast<char*>(&float_val);
         //out.write(reinterpret_cast<char*>(&buffer), sizeof(float));
         buffer.insert(buffer.end(),
                     tmp_char_ptr,
                     tmp_char_ptr + sizeof(float)
                     );
         break;
      };
      case dataType::BOOL: {
         bool buffer_bool;
         uint8_t buffer_int8;
         buffer_bool = std::get<bool>(value);
         if(buffer_bool){
            buffer_int8 = 1;
         }else{
            buffer_int8 = 0;
         };
         tmp_char_ptr = reinterpret_cast<char*>(&buffer_int8);
         //out.write(reinterpret_cast<char*>(&buffer_int8), sizeof(uint8_t));
         buffer.insert(buffer.end(),
                     tmp_char_ptr,
                     tmp_char_ptr + sizeof(uint8_t)
                     );
         break;
      };
      case dataType::STRING: {
         std::string string_val;
         uint32_t string_size;
         string_val = std::get<std::string>(value);
         string_size = string_val.size();
         // Primero escribimos el tamaño de la string:
         tmp_char_ptr = reinterpret_cast<char*>(&string_size);
         buffer.insert(buffer.end(),
                     tmp_char_ptr,
                     tmp_char_ptr + sizeof(uint32_t)
                     );
         // Ahora ya si escribimos la cadena de texto:
         tmp_char_ptr = string_val.data();
         buffer.insert(buffer.end(),
                     tmp_char_ptr,
                     tmp_char_ptr + string_size
                     );
         break;
      };
   };
};


std::streamsize disk_aux::return_file_size(std::ifstream& in){

   std::streamsize size = 0;

   if(in.is_open()){
      in.seekg(0, std::ios::end);
      size = in.tellg();
      in.seekg(0, std::ios::beg);
   };
   return size;
};


void disk_aux::fill_vector_int(uint32_t num_elementos, std::vector<char>& vec_in, std::vector<Values>& vec_out){

   int* datos_enteros = reinterpret_cast<int*>(vec_in.data());

   // Vaciamos el vector de salida por si tenía basura
   vec_out.clear(); 
   
   // Reservamos el espacio necesario para los 'Values' (que son más grandes que los int)
   vec_out.reserve(num_elementos);

   for (uint32_t i = 0; i < num_elementos; i++){
      vec_out.push_back(datos_enteros[i]);
   };
};

void disk_aux::fill_vector_float(uint32_t num_elementos, std::vector<char>& vec_in, std::vector<Values>& vec_out){

   float* datos_enteros = reinterpret_cast<float*>(vec_in.data());

   // Vaciamos el vector de salida por si tenía basura
   vec_out.clear(); 
   
   // Reservamos el espacio necesario para los 'Values' (que son más grandes que los int)
   vec_out.reserve(num_elementos);
   
   for (uint32_t i = 0; i < num_elementos; i++){
      vec_out.push_back(datos_enteros[i]);
   };
};


void disk_aux::fill_vector_string(uint32_t num_elementos, std::vector<char>& vec_str, std::vector<char>& vec_idx, std::vector<Values>& vec_out){
   vec_out.clear();
   vec_out.reserve(num_elementos);

   // 1. USA uint32_t para garantizar 4 bytes exactos por índice
   uint32_t* ptr_tamanos = reinterpret_cast<uint32_t*>(vec_idx.data());
   
   char* cursor = vec_str.data();

   for (uint32_t i = 0; i < num_elementos; i++) {
      // 2. Acceso directo por índice (ptr_tamanos[i]) es más seguro
      uint32_t tam_actual = ptr_tamanos[i]; 
      
      // LOG DE DEPURACIÓN CRÍTICO
      Logger::log(LogLevel::DEBUG, "Fila ", false, true);
      Logger::log(LogLevel::DEBUG, i, false, false);
      Logger::log(LogLevel::DEBUG, " - Offset en buffer: ", false, false);
      Logger::log(LogLevel::DEBUG, (uint32_t)(cursor - vec_str.data()), false, false);
      Logger::log(LogLevel::DEBUG, " - Tamano: ", false, false);
      Logger::log(LogLevel::DEBUG, tam_actual, true, false);

      // 3. Verificación de seguridad para no leer fuera del buffer
      if (cursor + tam_actual > vec_str.data() + vec_str.size()) {
          Logger::log(LogLevel::ERROR, "!!! INTENTO DE LECTURA FUERA DE LIMITES !!!");
          tam_actual = (vec_str.data() + vec_str.size()) - cursor;
      }

      std::string tmp_str(cursor, tam_actual);
      vec_out.push_back(tmp_str);

      cursor += tam_actual;
   };
};
////////////////////////////////////////////////////////////////////
// ELIMINACIÓN DE ARCHIVOS /////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void disk_aux::eliminar_archivo_binario_metadatos(table*& tb){
   /*
   Función que dada la referencia al puntero de la tabla,
   elimina sus metadatos en disco si estos existen.
   */

   std::string tb_name = tb->metadata_ptr->name;

   std::filesystem::path ruta = "metadata/" + tb_name + "_meta.bin";

   if(std::filesystem::exists(ruta)){
      // Sólo eliminamos si existe el archivo:
      std::filesystem::remove(ruta);
   };
};


void disk_aux::eliminar_archivo_binario_datos(table*& tb){
   /*
   Función que dada la referencia al puntero de la tabla,
   elimina sus datos en disco si estos existen.
   */

   std::string tb_name = tb->metadata_ptr->name;

   std::filesystem::path ruta = "data/" + tb_name;

   if(std::filesystem::exists(ruta)){
      // Sólo eliminamos si existe el archivo:
      std::filesystem::remove_all(ruta);
   };
};

////////////////////////////////////////////////////////////////////
// LECTURA DE TODOS LOS METADATOS //////////////////////////////////
////////////////////////////////////////////////////////////////////

  // Funcion auxilar recursiva:
void disk_aux::escanear_tablas_recursiva(const std::filesystem::path& ruta, std::vector<std::filesystem::path>& arr_tablas){
   // Iteramos por cada elemento:
   for (const auto& entrada : fs::directory_iterator(ruta)){
      // Caso de quecsea una ruta:
      if (fs::is_directory(entrada)){
         disk_aux::escanear_tablas_recursiva(entrada.path(), arr_tablas);
      } else if(fs::is_regular_file(entrada) && entrada.path().extension().string() == ".bin" && entrada.path().filename().string().find("_meta") != std::string::npos){
         /*
         Caso base de la recursion
         Solo adicionamos metadatos
         */
         arr_tablas.push_back(entrada);
      };
   };
};

// SOLO ESCANEA LOS METADATOS DE LAS TABLAS:
std::vector<std::filesystem::path> disk_aux::escanear_tablas(){

      fs::path ruta_tablas = "metadata";
      // Vector con el nombre de todas las tablas:
      std::vector<std::filesystem::path> arr_tablas;

      disk_aux::escanear_tablas_recursiva(ruta_tablas, arr_tablas);
      return arr_tablas;
};