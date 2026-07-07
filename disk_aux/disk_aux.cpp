#include "disk_aux.h"


////////////////////////////////////////////////////////////////////
// FUNCIONES AUXILIARES ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
void disk_aux::aux_vector_buffer_write_disk(char* ptr_ini, uint32_t size_buffer, std::ofstream& out){

   if (out.is_open()) {
         // Escribimos todo el contenido una sola vez
         out.write(ptr_ini, size_buffer);
   };
};

void disk_aux::aux_vector_buffer_write_disk(const std::vector<char>& buffer, std::ofstream& out){

   if (out.is_open()) {
         // Escribimos todo el contenido una sola vez
         out.write(buffer.data(), buffer.size());
   };
};

void disk_aux::aux_vector_buffer_write_disk(char* ptr_ini, uint32_t size_buffer, std::fstream& out){

   if (out.is_open()) {
         // Escribimos todo el contenido una sola vez
         out.write(ptr_ini, size_buffer);
   };
};

void disk_aux::aux_vector_buffer_write_disk(const std::vector<char>& buffer, std::fstream& out){

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
      case dataType::UNKNOWN: {
         std::vector<char> buffer;
         uint32_t vector_size;
         // Primero escribimos el tamaño de la string:
         in.read(reinterpret_cast<char*>(&vector_size), sizeof(uint32_t));
         // Ajustamos el tamaño de la cadena de texto donde escribiremos:
         buffer.resize(vector_size);   // ← reservar memoria
         // Ahora ya si escribimos la cadena de texto:
         in.read(buffer.data(), vector_size);
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


std::vector<std::string> disk_aux::obtener_archivos_en_ruta(const std::filesystem::path& ruta, dataType tipo_dato) {
   std::vector<std::string> archivos;

   // 1. Verificamos si la ruta existe y si es realmente un directorio
   if (!std::filesystem::exists(ruta) || !std::filesystem::is_directory(ruta)) {
      return archivos; // Devuelve vector vacío si no existe
   }

   // 2. Iteramos sobre los elementos de la carpeta
   for (const auto& entrada : std::filesystem::directory_iterator(ruta)) {
      // Verificamos que sea un archivo regular (no otra carpeta)
      if (!std::filesystem::is_regular_file(entrada.path())){
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


uint32_t disk_aux::obtener_tamano_archivo(const std::filesystem::path& ruta) {
   Logger::log(LogLevel::DEBUG, "Dentro de la funcion que halla el tamano de un archivo");
   // Abrimos el archivo:
   // binary: Para que no haya conversiones de caracteres extrañas
   // ate: "At the end", para situar el puntero directamente al final
   std::ifstream archivo(ruta, std::ios::binary | std::ios::ate);

   if (!archivo.is_open()) {
      Logger::log(LogLevel::DEBUG, "ERROR al abrir el archivo para ver su tamano");
      return 0; // Error al abrir
   };

   Logger::log(LogLevel::DEBUG, "El archivo se abrio con EXITO");

   // tellg() devuelve la posición actual del cursor. 
   // Como lo abrimos con 'ate', el cursor está al final, 
   // dándonos el tamaño total en bytes.
   long tamano = archivo.tellg();

   archivo.close();
   return tamano;
};


uint32_t disk_aux::obtener_tamano_archivo_aux(const std::filesystem::path& ruta) {
    // 🚀 Forma nativa de C++: No abre el archivo, le pregunta directamente al Sistema Operativo
    std::error_code ec;
    uintmax_t tamano = std::filesystem::file_size(ruta, ec);

    if (ec) {
        // Si hay un error (ej: el archivo no existe o no hay permisos)
        // Puedes imprimir ec.message() si quieres ver el error real del sistema
        Logger::log(LogLevel::DEBUG, "Error de filesystem: " + ec.message());
        return 0; 
    }

    return static_cast<uint32_t>(tamano);
}





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
      case dataType::UNKNOWN: {
         std::vector<char> vector_val;
         uint32_t vector_size;
         vector_val = std::get<std::vector<char>>(value);
         vector_size = vector_val.size();
         // Primero escribimos el tamaño de la string:
         tmp_char_ptr = reinterpret_cast<char*>(&vector_size);
         buffer.insert(buffer.end(),
                     tmp_char_ptr,
                     tmp_char_ptr + sizeof(uint32_t)
                     );
         // Ahora ya si escribimos la cadena de texto:
         tmp_char_ptr = vector_val.data();
         buffer.insert(buffer.end(),
                     tmp_char_ptr,
                     tmp_char_ptr + vector_size
                     );
         break;
      };
   };
};




void disk_aux::write_aux_val_buffer_with_size_check(const Values& value, dataType tipo_dato, char*& buffer_pointer, uint32_t& offset, uint32_t& bytes_written, uint32_t& bytes_remain, char*& ptr_str_ini, uint32_t& current_col){

   /*
   Función parecida a 'write_aux_val', pero esta vez escribe un buffer de carácteres.
   Dejándolo listo para así escribir el buffer en una sola llamada al disco,
   reduciendo latencia de interacción con el disco en escritura.
   Si se lee este mismo buffer se ahorrará también significativamente en tiempo de
   lectura.
   */

   char* tmp_char_ptr = nullptr;
   char*& buffer_ptr = buffer_pointer;

   switch(tipo_dato){

      case dataType::INT: {

         uint8_t size_int = sizeof(int);
         //if(size_int > bytes_remain){
         if(size_int <= bytes_remain){
            int int_val;
            int_val = std::get<int>(value);
            tmp_char_ptr = reinterpret_cast<char*>(&int_val);
            std::memcpy(buffer_ptr, tmp_char_ptr, size_int);
            buffer_ptr += size_int;
            bytes_remain -= size_int;
            bytes_written += size_int;
            current_col += 1;
            return;
         };
         offset = size_int;
         return;


      };
      case dataType::FLOAT: {
         uint8_t size_float = sizeof(float);
         //if(size_float > bytes_remain){
         if(size_float <= bytes_remain){
            float float_val;
            float_val = std::get<float>(value);
            tmp_char_ptr = reinterpret_cast<char*>(&float_val);
            std::memcpy(buffer_ptr, tmp_char_ptr, size_float);
            buffer_ptr += size_float;
            bytes_remain -= size_float;
            bytes_written += size_float;
            current_col += 1;
            return;
         };
         offset = size_float;
         return;
      };
      case dataType::BOOL: {
         uint8_t size_bool_int8 = sizeof(uint8_t);
         //if(size_bool_int8 > bytes_remain){
         if(size_bool_int8 <= bytes_remain){
            bool buffer_bool;
            uint8_t buffer_int8;
            buffer_bool = std::get<bool>(value);
            if(buffer_bool){
               buffer_int8 = 1;
            }else{
               buffer_int8 = 0;
            };
            tmp_char_ptr = reinterpret_cast<char*>(&buffer_int8);
            std::memcpy(buffer_ptr, tmp_char_ptr, size_bool_int8);
            buffer_ptr += size_bool_int8;
            bytes_remain -= size_bool_int8;
            bytes_written += size_bool_int8;
            current_col += 1;
            return;
         };
         offset = size_bool_int8;
         return;
      };

      case dataType::STRING: {

         const std::string& string_val = std::get<std::string>(value);
         uint32_t string_size;
         string_size = string_val.size();
         uint8_t uint32_size = sizeof(uint32_t);

         Logger::log(LogLevel::DEBUG, "[WRITE_STRING] >>> Nueva entrada a la función auxiliar <<<");
         Logger::log(LogLevel::DEBUG, "[WRITE_STRING] Contenido texto: " + string_val);
         Logger::log(LogLevel::DEBUG, "[WRITE_STRING] string_size: ", false, true); Logger::log(LogLevel::DEBUG, string_size, true, false);
         Logger::log(LogLevel::DEBUG, "[WRITE_STRING] bytes_remain en buffer: ", false, true); Logger::log(LogLevel::DEBUG, bytes_remain, true, false);
         Logger::log(LogLevel::DEBUG, "[WRITE_STRING] Direccion base string_val.data(): ", false, true); Logger::log(LogLevel::DEBUG, reinterpret_cast<uintptr_t>(string_val.data()), true, false);
         Logger::log(LogLevel::DEBUG, "[WRITE_STRING] Estado inicial ptr_str_ini: ", false, true); Logger::log(LogLevel::DEBUG, reinterpret_cast<uintptr_t>(ptr_str_ini), true, false);

         // Fase 1: Lectura del tamaño de la string:
         if(!ptr_str_ini){
            Logger::log(LogLevel::DEBUG, "[WRITE_STRING] [Fase 1] ptr_str_ini es nullptr. Intentando escribir tamano...");
            tmp_char_ptr = reinterpret_cast<char*>(&string_size);
            if(uint32_size <= bytes_remain){
               tmp_char_ptr = reinterpret_cast<char*>(&string_size);
               std::memcpy(buffer_ptr, tmp_char_ptr, uint32_size);
               buffer_ptr += uint32_size;
               bytes_remain -= uint32_size;
               bytes_written += uint32_size;
               ptr_str_ini = const_cast<char*>(string_val.data());
               
               Logger::log(LogLevel::DEBUG, "[WRITE_STRING] [Fase 1] Tamano escrito con exito.");
               Logger::log(LogLevel::DEBUG, "[WRITE_STRING] [Fase 1] Nuevo ptr_str_ini asignado a: ", false, true); Logger::log(LogLevel::DEBUG, reinterpret_cast<uintptr_t>(ptr_str_ini), true, false);
               
               // No dejamos que caiga a la Fase 2 con bytes_remain = 0.
               if (bytes_remain == 0) {
                  Logger::log(LogLevel::DEBUG, "[WRITE_STRING] [Fase 1] Buffer lleno (bytes_remain == 0) justo tras el tamano. Saliendo.");
                  return;
               };
            }else{
               Logger::log(LogLevel::DEBUG, "[WRITE_STRING] [Fase 1] ERROR: No cabe el tamano en el buffer. Forzando offset.");
               offset = uint32_size;
               return;
            }; 
         };

         // Fase 2:
         if (ptr_str_ini && bytes_remain > 0) {
            uint32_t bytes_pendientes = (string_val.data() + string_size) - ptr_str_ini;
            
            Logger::log(LogLevel::DEBUG, "[WRITE_STRING] [Fase 2] ptr_str_ini valido. Calculando bytes pendientes.");
            Logger::log(LogLevel::DEBUG, "[WRITE_STRING] [Fase 2] string_val.data() + string_size = ", false, true); Logger::log(LogLevel::DEBUG, reinterpret_cast<uintptr_t>(string_val.data() + string_size), true, false);
            Logger::log(LogLevel::DEBUG, "[WRITE_STRING] [Fase 2] ptr_str_ini actual = ", false, true); Logger::log(LogLevel::DEBUG, reinterpret_cast<uintptr_t>(ptr_str_ini), true, false);
            Logger::log(LogLevel::DEBUG, "[WRITE_STRING] [Fase 2] bytes_pendientes de texto: ", false, true); Logger::log(LogLevel::DEBUG, bytes_pendientes, true, false);

            uint32_t bytes_to_write = 0;
            if(bytes_remain >= bytes_pendientes){ 
               bytes_to_write = bytes_pendientes;
            }else{
               bytes_to_write = bytes_remain;
            };
            
            Logger::log(LogLevel::DEBUG, "[WRITE_STRING] [Fase 2] Decidido escribir bytes_to_write: ", false, true); Logger::log(LogLevel::DEBUG, bytes_to_write, true, false);

            // Solo en caso de tener algo que escribir en el buffer:
            if (bytes_to_write > 0) {
               std::memcpy(buffer_ptr,
                           ptr_str_ini,
                           bytes_to_write
                           );
               buffer_ptr += bytes_to_write;
               ptr_str_ini += bytes_to_write;
               bytes_remain -= bytes_to_write;
               bytes_written += bytes_to_write;
               
               Logger::log(LogLevel::DEBUG, "[WRITE_STRING] [Fase 2] Copiados bytes a buffer. Avance de ptr_str_ini a: ", false, true); Logger::log(LogLevel::DEBUG, reinterpret_cast<uintptr_t>(ptr_str_ini), true, false);
            };
         };


         // Fase 3: Final
         Logger::log(LogLevel::DEBUG, "[WRITE_STRING] [Fase 3] Evaluando fin de string...");
         Logger::log(LogLevel::DEBUG, "[WRITE_STRING] [Fase 3] ptr_str_ini: ", false, true); Logger::log(LogLevel::DEBUG, reinterpret_cast<uintptr_t>(ptr_str_ini), true, false);
         Logger::log(LogLevel::DEBUG, "[WRITE_STRING] [Fase 3] Limite final (data+size): ", false, true); Logger::log(LogLevel::DEBUG, reinterpret_cast<uintptr_t>(string_val.data() + string_size), true, false);
         
         if(ptr_str_ini >= string_val.data() + string_size){
            // We have finished reading the whole string:
            Logger::log(LogLevel::DEBUG, "^^^^^^^^ ptr_str_ini se fija a nullptr ^^^^^^^^");
            current_col += 1;
            ptr_str_ini = nullptr;
         }else{
            Logger::log(LogLevel::DEBUG, "^^^^^^^^ ptr_str_ini NO se fija a nullptr ^^^^^^^^");
         };
         return;
      };





















      case dataType::UNKNOWN: {
         const std::vector<char>& char_vec_val = std::get<std::vector<char>>(value);
         uint32_t unk_size;
         unk_size = char_vec_val.size();
         uint8_t uint32_size = sizeof(uint32_t);

         Logger::log(LogLevel::DEBUG, "[WRITE_STRING] >>> Nueva entrada a la función auxiliar <<<");
         Logger::log(LogLevel::DEBUG, std::string("[WRITE_STRING] Contenido texto: ") + char_vec_val.data());
         Logger::log(LogLevel::DEBUG, "[WRITE_STRING] unk_size: ", false, true); Logger::log(LogLevel::DEBUG, unk_size, true, false);
         Logger::log(LogLevel::DEBUG, "[WRITE_STRING] bytes_remain en buffer: ", false, true); Logger::log(LogLevel::DEBUG, bytes_remain, true, false);
         Logger::log(LogLevel::DEBUG, "[WRITE_STRING] Direccion base char_vec_val.data(): ", false, true); Logger::log(LogLevel::DEBUG, reinterpret_cast<uintptr_t>(char_vec_val.data()), true, false);
         Logger::log(LogLevel::DEBUG, "[WRITE_STRING] Estado inicial ptr_str_ini: ", false, true); Logger::log(LogLevel::DEBUG, reinterpret_cast<uintptr_t>(ptr_str_ini), true, false);

         // Fase 1: Lectura del tamaño de la string:
         if(!ptr_str_ini){
            Logger::log(LogLevel::DEBUG, "[WRITE_STRING] [Fase 1] ptr_str_ini es nullptr. Intentando escribir tamano...");
            tmp_char_ptr = reinterpret_cast<char*>(&unk_size);
            if(uint32_size <= bytes_remain){
               tmp_char_ptr = reinterpret_cast<char*>(&unk_size);
               std::memcpy(buffer_ptr, tmp_char_ptr, uint32_size);
               buffer_ptr += uint32_size;
               bytes_remain -= uint32_size;
               bytes_written += uint32_size;
               ptr_str_ini = const_cast<char*>(char_vec_val.data());
               
               Logger::log(LogLevel::DEBUG, "[WRITE_STRING] [Fase 1] Tamano escrito con exito.");
               Logger::log(LogLevel::DEBUG, "[WRITE_STRING] [Fase 1] Nuevo ptr_str_ini asignado a: ", false, true); Logger::log(LogLevel::DEBUG, reinterpret_cast<uintptr_t>(ptr_str_ini), true, false);
               
               // No dejamos que caiga a la Fase 2 con bytes_remain = 0.
               if (bytes_remain == 0) {
                  Logger::log(LogLevel::DEBUG, "[WRITE_STRING] [Fase 1] Buffer lleno (bytes_remain == 0) justo tras el tamano. Saliendo.");
                  return;
               };
            }else{
               Logger::log(LogLevel::DEBUG, "[WRITE_STRING] [Fase 1] ERROR: No cabe el tamano en el buffer. Forzando offset.");
               offset = uint32_size;
               return;
            }; 
         };

         // Fase 2:
         if (ptr_str_ini && bytes_remain > 0) {
            uint32_t bytes_pendientes = (char_vec_val.data() + unk_size) - ptr_str_ini;
            
            Logger::log(LogLevel::DEBUG, "[WRITE_STRING] [Fase 2] ptr_str_ini valido. Calculando bytes pendientes.");
            Logger::log(LogLevel::DEBUG, "[WRITE_STRING] [Fase 2] char_vec_val.data() + unk_size = ", false, true); Logger::log(LogLevel::DEBUG, reinterpret_cast<uintptr_t>(char_vec_val.data() + unk_size), true, false);
            Logger::log(LogLevel::DEBUG, "[WRITE_STRING] [Fase 2] ptr_str_ini actual = ", false, true); Logger::log(LogLevel::DEBUG, reinterpret_cast<uintptr_t>(ptr_str_ini), true, false);
            Logger::log(LogLevel::DEBUG, "[WRITE_STRING] [Fase 2] bytes_pendientes de texto: ", false, true); Logger::log(LogLevel::DEBUG, bytes_pendientes, true, false);

            uint32_t bytes_to_write = 0;
            if(bytes_remain >= bytes_pendientes){ 
               bytes_to_write = bytes_pendientes;
            }else{
               bytes_to_write = bytes_remain;
            };
            
            Logger::log(LogLevel::DEBUG, "[WRITE_STRING] [Fase 2] Decidido escribir bytes_to_write: ", false, true); Logger::log(LogLevel::DEBUG, bytes_to_write, true, false);

            // Solo en caso de tener algo que escribir en el buffer:
            if (bytes_to_write > 0) {
               std::memcpy(buffer_ptr,
                           ptr_str_ini,
                           bytes_to_write
                           );
               buffer_ptr += bytes_to_write;
               ptr_str_ini += bytes_to_write;
               bytes_remain -= bytes_to_write;
               bytes_written += bytes_to_write;
               
               Logger::log(LogLevel::DEBUG, "[WRITE_STRING] [Fase 2] Copiados bytes a buffer. Avance de ptr_str_ini a: ", false, true); Logger::log(LogLevel::DEBUG, reinterpret_cast<uintptr_t>(ptr_str_ini), true, false);
            };
         };


         // Fase 3: Final
         Logger::log(LogLevel::DEBUG, "[WRITE_STRING] [Fase 3] Evaluando fin de string...");
         Logger::log(LogLevel::DEBUG, "[WRITE_STRING] [Fase 3] ptr_str_ini: ", false, true); Logger::log(LogLevel::DEBUG, reinterpret_cast<uintptr_t>(ptr_str_ini), true, false);
         Logger::log(LogLevel::DEBUG, "[WRITE_STRING] [Fase 3] Limite final (data+size): ", false, true); Logger::log(LogLevel::DEBUG, reinterpret_cast<uintptr_t>(char_vec_val.data() + unk_size), true, false);
         
         if(ptr_str_ini >= char_vec_val.data() + unk_size){
            // We have finished reading the whole string:
            Logger::log(LogLevel::DEBUG, "^^^^^^^^ ptr_str_ini se fija a nullptr ^^^^^^^^");
            current_col += 1;
            ptr_str_ini = nullptr;
         }else{
            Logger::log(LogLevel::DEBUG, "^^^^^^^^ ptr_str_ini NO se fija a nullptr ^^^^^^^^");
         };
         return;
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


uint32_t disk_aux::return_file_size_bytes(std::ifstream& in){

   uint32_t size_bytes = 0;

   if(in.is_open()){
      in.seekg(0, std::ios::end);
      size_bytes = static_cast<uint32_t>(in.tellg());
      in.seekg(0, std::ios::beg);
   };
   return size_bytes;
};


void disk_aux::fill_vector_int(uint32_t num_elementos, char* vec_in, std::vector<Values>& vec_out){

   int* datos_enteros = reinterpret_cast<int*>(vec_in);

   // Vaciamos el vector de salida por si tenía basura
   vec_out.clear(); 
   
   // Reservamos el espacio necesario para los 'Values' (que son más grandes que los int)
   vec_out.reserve(num_elementos);

   for (uint32_t i = 0; i < num_elementos; i++){
      vec_out.push_back(datos_enteros[i]);
   };
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


void disk_aux::fill_vector_float(uint32_t num_elementos, char* vec_in, std::vector<Values>& vec_out){

   float* datos_enteros = reinterpret_cast<float*>(vec_in);

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


void disk_aux::fill_vector_bool(uint32_t num_elementos, char* vec_in, std::vector<Values>& vec_out){

   uint8_t* datos_enteros = reinterpret_cast<uint8_t*>(vec_in);

   // Vaciamos el vector de salida por si tenía basura
   vec_out.clear(); 
   
   // Reservamos el espacio necesario para los 'Values' (que son más grandes que los int)
   vec_out.reserve(num_elementos);

   for (uint32_t i = 0; i < num_elementos; i++){
      if(datos_enteros[i] == 1){
         vec_out.push_back(true);
      }else{
         vec_out.push_back(false);
      };
   };
};

void disk_aux::fill_vector_bool(uint32_t num_elementos, std::vector<char>& vec_in, std::vector<Values>& vec_out){

   uint8_t* datos_enteros = reinterpret_cast<uint8_t*>(vec_in.data());

   // Vaciamos el vector de salida por si tenía basura
   vec_out.clear(); 
   
   // Reservamos el espacio necesario para los 'Values' (que son más grandes que los int)
   vec_out.reserve(num_elementos);

   for (uint32_t i = 0; i < num_elementos; i++){
      if(datos_enteros[i] == 1){
         vec_out.push_back(true);
      }else{
         vec_out.push_back(false);
      };
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
      //Logger::log(LogLevel::DEBUG, "Fila ", false, true);
      //Logger::log(LogLevel::DEBUG, i, false, false);
      //Logger::log(LogLevel::DEBUG, " - Offset en buffer: ", false, false);
      //Logger::log(LogLevel::DEBUG, (uint32_t)(cursor - vec_str.data()), false, false);
     // Logger::log(LogLevel::DEBUG, " - Tamano: ", false, false);
      //Logger::log(LogLevel::DEBUG, tam_actual, true, false);

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

   //std::filesystem::path ruta = "metadata" + tb_name + "_meta.bin";
   std::filesystem::path ruta = "metadata";
   ruta /= (tb_name + "_meta.bin");

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

   //std::filesystem::path ruta = "data/" + tb_name;
   std::filesystem::path ruta = "data";
   ruta /= (tb_name + "tb_name");

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
   for (const auto& entrada : std::filesystem::directory_iterator(ruta)){
      // Caso de quecsea una ruta:
      if (std::filesystem::is_directory(entrada)){
         disk_aux::escanear_tablas_recursiva(entrada.path(), arr_tablas);
      } else if(std::filesystem::is_regular_file(entrada) && entrada.path().extension().string() == ".bin" && entrada.path().filename().string().find("_meta") != std::string::npos){
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

      std::filesystem::path ruta_tablas = "metadata";
      // Vector con el nombre de todas las tablas:
      std::vector<std::filesystem::path> arr_tablas;

      disk_aux::escanear_tablas_recursiva(ruta_tablas, arr_tablas);
      return arr_tablas;
};



//////////////////////////////////////////////////////////////////////////////////////

// Lectura de datos, pero directamente desde el data PTR, sin tener que leer de ningún archivo: