

#include "data_struct.h"
#include <iostream>
#include <fstream>
#include <string>
#include "globals.h"
#include <map>
#include "logging.h"
#include "filesystem"
#include "disk_io.h"
#include "disk_buffer.h"

	//namespace fs = std::filesystem;

   //---------------------------------------------------------------------------------
   //========================================================
   //== FUNION ESCRITURA METADATOS: =========================
   //========================================================
   uint32_t disk_io::write_table_metadata(table* tabla){
      if (!tabla) return 0;
      Logger::log(LogLevel::DEBUG, "DENTRO DE 'write_table_metadata': ");
      std::string nombre_tabla = tabla->metadata_ptr->name;
      std::string ruta_tabla = "data/" + nombre_tabla + "_meta.bin";

      // Abrimos la escritura:
      std::ofstream out(ruta_tabla, std::ios::binary | std::ios::out);

      // == Escribimos los metadatos: ===============================

      // -- Escribimos el nombre -----------------------------------------
      uint32_t size_nombre = nombre_tabla.size();
      out.write(reinterpret_cast<char*>(&size_nombre), sizeof(uint32_t));
      Logger::log(LogLevel::DEBUG, "VALOR DE size_nombre ESCRITO: " + std::to_string(size_nombre));
      Logger::flush();
      out.write(nombre_tabla.data(), size_nombre);
      Logger::log(LogLevel::DEBUG, "VALOR DE nombre_tabla ESCRITA: " + nombre_tabla);
      Logger::flush();

      // -- Escribimos el número de columnas -----------------------------
      uint32_t num_cols = (tabla->metadata_ptr->column_names).size();
      out.write(reinterpret_cast<char*>(&num_cols), sizeof(uint32_t));
      Logger::log(LogLevel::DEBUG, "Se han escrito el numero de columnas: ", false, true);
      Logger::log(LogLevel::DEBUG, num_cols, true, false);
      Logger::flush();

      // Escribimos el numero de filas:
      std::map<std::string, std::vector<Values>> columnas = tabla->data_ptr->columns;
      if(!columnas.empty()){
         Logger::log(LogLevel::DEBUG, "LOS DATOS EN RAM VIVA NO SON NULOS");
         std::string column_name = (tabla->metadata_ptr->column_names)[0];
         Logger::log(LogLevel::DEBUG, " Hemos hallado el nombre de la primera columna");
         std::vector<Values> col_datos = columnas.at(column_name);
         Logger::log(LogLevel::DEBUG, " Hemos hallado la variable temporal 'col_datos'");
         uint32_t n_rows = col_datos.size();
         out.write(reinterpret_cast<char*>(&n_rows), sizeof(uint32_t));
         Logger::log(LogLevel::DEBUG, "*********** EMPEZAMOS LA ESCRITURA DE LOS METADATOS ****************");
         Logger::flush();
         for(uint32_t i=0; i<num_cols; i++){
            // -- Escribimos los datos de cada columna ---------------------
            Logger::log(LogLevel::DEBUG, "ITERACION: ", false, true);                                                                           Logger::log(LogLevel::DEBUG, i, false, false);                    Logger::log(LogLevel::DEBUG, " de escritura metadatos", true, false);
            Logger::flush();
            // -- Escribimos el nombre:
            std::string column_name = (tabla->metadata_ptr->column_names)[i];
            uint32_t size_column_name = column_name.size();
            out.write(reinterpret_cast<char*>(&size_column_name), sizeof(uint32_t));
            out.write(column_name.data(), size_column_name);
            // -- Escribimos el tipo de dato:
            dataType col_type = (tabla->metadata_ptr->column_types)[i];
            uint32_t col_type_disk = static_cast<uint32_t>(col_type);
            out.write(reinterpret_cast<char*>(&col_type_disk), sizeof(uint32_t));
            // -- Escribimos si es clave primaria:
            bool column_is_key = (tabla->metadata_ptr->primary_list)[i];
            uint8_t key_val = column_is_key ? 1 : 0;
            out.write(reinterpret_cast<char*>(&key_val), sizeof(uint8_t));
         };
         Logger::log(LogLevel::DEBUG, "*********** ESCRITURA DE LOS METADATOS TERMINADA ****************");
         Logger::flush();
         Logger::log(LogLevel::DEBUG, "HACEMOS FLUSH DE LOS DATOS AL ESCRIBIR");
         Logger::flush();
         out.flush();
         out.close();
         return n_rows;
      } else{
         Logger::log(LogLevel::DEBUG, "LOS DATOS EN RAM VIVA SI SON NULOS");
      };
         out.close();
         return 0;
   };



   // Funcion auxliar para escribir un valor concreto en disco:
   void disk_io::write_aux_val(Values value, dataType tipo_dato, std::ofstream& out){

      switch(tipo_dato){

         case dataType::INT: {
            Logger::log(LogLevel::DEBUG, "El tipo de valor es entero");
            int buffer;
            buffer = std::get<int>(value);
            Logger::log(LogLevel::DEBUG, "Dato recuperado en el buffer");
            out.write(reinterpret_cast<char*>(&buffer), sizeof(int));
            break;
         };
         case dataType::FLOAT: {
            Logger::log(LogLevel::DEBUG, "El tipo de valor es float");
            float buffer;
            buffer = std::get<float>(value);
            Logger::log(LogLevel::DEBUG, "Dato recuperado en el buffer");
            out.write(reinterpret_cast<char*>(&buffer), sizeof(float));
            break;
         };
         case dataType::BOOL: {
            Logger::log(LogLevel::DEBUG, "El tipo de valor es booleano");
            bool buffer_bool;
            uint8_t buffer_int8;
            buffer_bool = std::get<bool>(value);
            Logger::log(LogLevel::DEBUG, "Dato recuperado en el buffer");
            if(buffer_bool){
               buffer_int8 = 1;
            }else{
               buffer_int8 = 0;
            };
            out.write(reinterpret_cast<char*>(&buffer_int8), sizeof(uint8_t));
            break;
         };
         case dataType::STRING: {
            Logger::log(LogLevel::DEBUG, "El tipo de valor es una cadena de texto");
            std::string buffer;
            uint32_t string_size;
            buffer = std::get<std::string>(value);
            Logger::log(LogLevel::DEBUG, "Dato recuperado en el buffer");
            string_size = buffer.size();
            // Primero escribimos el tamaño de la string:
            out.write(reinterpret_cast<char*>(&string_size), sizeof(uint32_t));
            // Ahora ya si escribimos la cadena de texto:
            out.write(reinterpret_cast<char*>(buffer.data()), string_size);
            break;
         };

      };
   };


   void disk_io::write_table_data(table* tabla, uint32_t n_rows){

	   if (!tabla) return;
      Logger::log(LogLevel::DEBUG, "DENTRO DE 'write_table_data': ");
      std::string nombre_tabla = tabla->metadata_ptr->name;
	   std::string ruta_tabla = "data/" + nombre_tabla + "_data.bin";

      // Abrimos la escritura:
      std::ofstream out(ruta_tabla, std::ios::out | std::ios::binary);

      table_metadata* metadata = tabla->metadata_ptr;
      std::vector<dataType> tipos_datos = tabla->metadata_ptr->column_types;
      std::vector<std::string> columnas_nombre = tabla->metadata_ptr->column_names;
      uint32_t n_cols = tipos_datos.size();
      uint32_t n_filas = metadata->n_filas_ram;

      // Creamos el iterador por filas:
      auto it = disk_buffer::tableRowIterator_only_ram(nombre_tabla);
      std::map<std::string, Values> fila_a_escribir;
      Values valor_tmp;

      // Antes de escribir los datos, escribimos las filas de datos totales:
      uint32_t n_f_disk = tabla->metadata_ptr->n_filas_disco;
      uint32_t n_f_ram = tabla->metadata_ptr->n_filas_ram;

      uint32_t n_filas_total = n_f_disk + n_f_ram;
      out.write(reinterpret_cast<char*>(&n_filas_total), sizeof(uint32_t));

      // Ahora nos movemos al final para poder escribir sólo al finaL_
      out.seekp(0, std::ios::end);

      // Ahora iteraremos hasta que esteé vacía la fila a escribir
      bool aux_bool = true;
      while(!it.is_eof()){
         fila_a_escribir = it.get_next_row_ram_viva();
         Logger::log(LogLevel::DEBUG, "Ya hemos leido la fila desde el iterador ");
         Logger::flush();
         // Ahora iteramos por cada columna:
         for(int i = 0; i<n_cols; i++){
            Logger::log(LogLevel::DEBUG, "//////////////////////////////////////////////////////", false, true);
            Logger::log(LogLevel::DEBUG, "Insertamos el valor de la columna: ", false, true);
            Logger::log(LogLevel::DEBUG, columnas_nombre[i], true, false);
            //Logger::log(LogLevel::DEBUG, "Tipo de dato: ", false, true);
            //Logger::log(LogLevel::DEBUG, tipos_datos[i], true, false);
            Logger::flush();
            // valor_tmp = fila_a_escribir[i]; // Obtenemos el valor de una fila y columna concretos
            valor_tmp = fila_a_escribir.at(columnas_nombre[i]); // Obtenemos el valor de una fila y columna concretos
            disk_io::write_aux_val(valor_tmp, tipos_datos[i], out);
         };
      };
      out.flush();
      out.close();

   };


   //========================================================
   //== FUNION LECTURA DATOS: ===============================
   //========================================================


   // Funcion auxliar para escribir un valor concreto en disco:
   Values disk_io::read_aux_val(dataType tipo_dato, std::ifstream& in){

      Values value;
      switch(tipo_dato){

         case dataType::INT: {
            Logger::log(LogLevel::DEBUG, "El tipo de valor es entero");
            int buffer;
            //buffer = std::get<int>(value);
            in.read(reinterpret_cast<char*>(&buffer), sizeof(int));
            value = buffer;
            Logger::log(LogLevel::DEBUG, "Dato leído al buffer");
            break;
         };
         case dataType::FLOAT: {
            Logger::log(LogLevel::DEBUG, "El tipo de valor es float");
            float buffer;
            //buffer = std::get<float>(value);
            in.read(reinterpret_cast<char*>(&buffer), sizeof(float));
            value = buffer;
            Logger::log(LogLevel::DEBUG, "Dato leído al buffer");
            break;
         };
         case dataType::BOOL: {
            Logger::log(LogLevel::DEBUG, "El tipo de valor es bool");
            bool buffer_bool;
            uint8_t buffer_int8;
            //buffer_bool = std::get<bool>(value);
            in.read(reinterpret_cast<char*>(&buffer_int8), sizeof(uint8_t));
            if(buffer_int8==1){
               buffer_bool = true;
            }else{
               buffer_bool = false;
            };
            value = buffer_bool;
            Logger::log(LogLevel::DEBUG, "Dato leído al buffer");
            break;
         };
         case dataType::STRING: {
            Logger::log(LogLevel::DEBUG, "El tipo de valor es una cadena de texto");
            std::string buffer;
            uint32_t string_size;
            //buffer = std::get<std::string>(value);
            // Primero escribimos el tamaño de la string:
            in.read(reinterpret_cast<char*>(&string_size), sizeof(uint32_t));
            // Ahora ya si escribimos la cadena de texto:
            in.read(reinterpret_cast<char*>(buffer.data()), string_size);
            value = buffer;
            Logger::log(LogLevel::DEBUG, "Dato leído al buffer");
            break;
         };
      };

      return value;
   };

   
   void disk_io::read_table_data(table* tabla){
      Logger::log(LogLevel::DEBUG, "ENTRAMOS A LA FUNCION DE LECTURA DE SOLO LOS DATOS");
      std::string nombre_tabla = tabla->metadata_ptr->name;
      std::string ruta_tabla = "data/" + nombre_tabla + "_data.bin";
      Logger::log(LogLevel::DEBUG, "Vamos a leer la ruta: "+ ruta_tabla);
      std::ifstream in(ruta_tabla, std::ios::binary);
      Logger::log(LogLevel::DEBUG, "Abrimos la lectura de la tabla: "+ nombre_tabla);
      Logger::flush();
      
      // Obtenemos los datos necesarios para la lectura:
      table_metadata* metadata = tabla->metadata_ptr;
      std::vector<dataType> tipos_datos = tabla->metadata_ptr->column_types;
      uint32_t n_cols = tipos_datos.size();
      std::vector<std::string> col_names = tabla->metadata_ptr->column_names;
      uint32_t n_filas = metadata->n_filas_ram;

      // Donde guardaremos los datos:
      std::map<std::string, std::vector<Values>>& columnas = tabla->data_buffer_ptr->columns;

      // Creamos el objeto de iteració para 
      // Creamos el iterador por filas:
      std::map<std::string, Values> fila_a_escribir;
      Values valor_tmp;

      // Primero de todo, leemos las filas a leer:
      //uint32_t n_filas = 0;
      in.read(reinterpret_cast<char*>(&n_filas), sizeof(uint32_t));
      Logger::log(LogLevel::DEBUG, "Comenzamos por las filas y las columnas para leer la tabla");
      Logger::flush();
      // Ahora iteramos por otodas las filas:
      for(int i = 0; i<n_filas; i++){
         for(int j = 0; i<n_cols; j++){
            // Usamos una función auxliar para leer los datos según su tipo:
            valor_tmp = disk_io::read_aux_val(tipos_datos[j], in);
            // Hemos recuperado el valor j de la fila i
            // Aho0ra rellenamos el vector correspondiente:
            if(columnas.empty()){
               columnas[col_names[i]].push_back(valor_tmp);
            }else{
               columnas.at(col_names[i]).push_back(valor_tmp);
            };
         };
      };



      /*while(!it.is_eof()){
         // Ahora iteramos por cada columna:
         for(int i = 0; i<n_cols; i++){
            // Ahra vamos actualizando las entradas una a una del std::map de datos:
            if(columnas.empty()){
               columas[col_names][i] = 
            };
         };
      };*/

      in.close();
    };



    void disk_io::read_table_metadata(std::filesystem::path ruta_tabla, std::string nombre_tabla_str){
       std::string tabla_nombre = ruta_tabla.stem().string();
       Logger::log(LogLevel::DEBUG, "LEEMOS LOS METADATOS DE LA TABLA: ", false, true);
       Logger::log(LogLevel::DEBUG, tabla_nombre, true, false);
       Logger::flush();
       std::string ruta_tabla_str = ruta_tabla.string();
       Logger::log(LogLevel::DEBUG, "Ruta de la tabla a leer meta: " + ruta_tabla_str);
       if (global_table_dict.find(nombre_tabla_str) == global_table_dict.end()) {
         Logger::log(LogLevel::ERROR, "ERROR: La tabla " + nombre_tabla_str + " no está en el diccionario global.");
       } else {
         Logger::log(LogLevel::DEBUG, "La tabla " + nombre_tabla_str + " SI EXISTE en el diccionario");
       };
       table_metadata* metadatos_puntero = global_table_dict.at(nombre_tabla_str)->metadata_ptr;
       if(metadatos_puntero){
          Logger::log(LogLevel::DEBUG, "El puntero a metadatos NO es nulo");
       };
       Logger::flush();

       // COMIENZA LA LECTURA:
       std::ifstream in(ruta_tabla_str, std::ios::binary);
       /*
       if (!in.is_open()) {
          Logger::log(LogLevel::ERROR, "!!! NO SE PUDO ABRIR EL ARCHIVO PARA LEER: " + ruta_tabla_str);
	  return;
       };
       */
       Logger::log(LogLevel::DEBUG, "La lectura de los metadatos ha sido sbierta");
       Logger::flush();
       // Leemos el tamaño del nombre:
       uint32_t size_nombre = 0;
       in.read(reinterpret_cast<char*>(&size_nombre), sizeof(uint32_t));
       Logger::log(LogLevel::DEBUG, "Se ha leido el size del nombre");
       Logger::log(LogLevel::DEBUG, "VALOR DE size_nombre LEIDO: " + std::to_string(size_nombre));
       Logger::flush();
       // Leemos el nombre:
       std::string nombre_tabla(size_nombre, '\0');
       in.read(&nombre_tabla[0], size_nombre);
       Logger::log(LogLevel::DEBUG, "Se ha leido el nombre de la tsbla");
       metadatos_puntero->name = nombre_tabla;
       Logger::log(LogLevel::DEBUG, " Nombre de la tabla: ", false, true);
       Logger::log(LogLevel::DEBUG, nombre_tabla, true, false);
       Logger::flush();
       Logger::log(LogLevel::DEBUG, "Se ha registrado el nombre de la tabla");
       Logger::flush();

       // Leemos el numero de columnas:
       uint32_t num_cols_lectura = 0;
       in.read(reinterpret_cast<char*>(&num_cols_lectura), sizeof(uint32_t));
       // Este vslor no se escribe, lo usaremos para iterar por cada columna

       // Leemos el numero de filas en disco:
       uint32_t num_filas_disco_lectura = 0;
       in.read(reinterpret_cast<char*>(&num_filas_disco_lectura), sizeof(uint32_t));
       metadatos_puntero->n_filas_disco = num_filas_disco_lectura;
       //Logger::log(LogLevel::DEBUG, "LECTURA PRUEBA METADATOS TERMINADA");
       //Logger::flush();
       //return;

       // Ahora iteramos poe cada columna, añadiendo metadatos de cada una:
       for(uint32_t i=0; i<num_cols_lectura; i++){
          // Leemos el nombre de la columna:
	  uint32_t size_column_name;
          in.read(reinterpret_cast<char*>(&size_column_name), sizeof(uint32_t
));
	  std::string columna_nombre_leido(size_column_name, '\0');
	  in.read(&columna_nombre_leido[0], size_column_name);
	  metadatos_puntero->column_names.push_back(columna_nombre_leido);

	  // Leemos el tipo de dato:
	  uint32_t col_type_int;
	  in.read(reinterpret_cast<char*>(&col_type_int), sizeof(uint32_t));
	  dataType col_type = static_cast<dataType>(col_type_int);
	  metadatos_puntero->column_types.push_back(col_type);

	  // Recuperamos si esclave primaria:
	  uint8_t column_is_key_num;
	  bool column_is_key;
	  in.read(reinterpret_cast<char*>(&column_is_key_num), sizeof(uint8_t));
	  if(column_is_key_num == 1){
	     column_is_key = true;
	  } else {
	     column_is_key = false;
	  };
	  metadatos_puntero->primary_list.push_back(column_is_key);

	  // Nos saltsmos el num cols de size uint32_t
	  // En realidad no necesitamos leer nafa mas

       };
       Logger::log(LogLevel::ERROR, "Metadatos leidos con exito");
       Logger::flush();
       in.close();

   };









    // Funcion auxilar recursiva:
   void disk_io::escanear_tablas_recursiva(const std::filesystem::path& ruta, std::vector<std::filesystem::path>& arr_tablas){
      // Iteramos por cada elemento:
      for (const auto& entrada : fs::directory_iterator(ruta)){
         // Caso de quecsea una ruta:
	      if (fs::is_directory(entrada)){
		      disk_io::escanear_tablas_recursiva(entrada.path(), arr_tablas);
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
   std::vector<std::filesystem::path> disk_io::escanear_tablas(){

       fs::path ruta_tablas = "data";
       // Vector con el nombre de todas las tablas:
       std::vector<std::filesystem::path> arr_tablas;

       disk_io::escanear_tablas_recursiva(ruta_tablas, arr_tablas);
       return arr_tablas;

    };

    void disk_io::mostrar_tablas_disco(std::vector<std::filesystem::path>& arr_tablas){

       Logger::log(LogLevel::DEBUG, "Mostramos las tablas en disco:");
       for(int i = 0; i<arr_tablas.size(); i++){
          Logger::log(LogLevel::DEBUG, "   Tabla: ", false, true);
	  Logger::log(LogLevel::DEBUG, arr_tablas[i].stem().string(), true, false);
       };
       Logger::flush();
    };

   void disk_io::lectura_metadatos_todas_tablas(){
    
      std::vector<std::filesystem::path> arr_tablas;
      arr_tablas = disk_io::escanear_tablas();
      for(int i = 0; i<arr_tablas.size(); i++){
         // Creamos el objeto de la tabla y su entrada en el diccionario global:
         // Inicializamos el diccionsrio global y sus elementos:
         table* tabla;
         // REGISTRAMOS EL NOMBRE DE LA TABLA SIN ESE '_meta':
         std::string nombre_tabla_str = arr_tablas[i].stem().string();
         if(nombre_tabla_str.ends_with("_meta")){
            nombre_tabla_str.erase(nombre_tabla_str.size() -5);
         };
         global_table_dict[nombre_tabla_str] = new table;
         tabla = global_table_dict.at(nombre_tabla_str);
         tabla->metadata_ptr = new table_metadata();
         tabla->data_ptr = new table_data();
         // tabla->data_buffer_ptr = new table_data_buffer(); No lo inicializamos hasta que se llame al constructor de 'tableRowIterator'
         disk_io::read_table_metadata(arr_tablas[i], nombre_tabla_str);
      };
      disk_io::debug_print_metadatos_memoria();
   };



    // FUNCIÓN DE ESCRITURA DE TODAS LAS TABLAS:


   void disk_io::write_dump(){
      Logger::log(LogLevel::DEBUG, "--- INICIO DIAGNÓSTICO DE DICCIONARIO ---");
    Logger::log(LogLevel::DEBUG, "Total de entradas en global_table_dict: " + std::to_string(global_table_dict.size()));

      // Primero vemos si el diccionario esta vacio o no:
      if(global_table_dict.empty()){
         Logger::log(LogLevel::DEBUG, "No escribiremos nada, el diccionario esta vacio");
      }else{
         //El diccionario tiene contenido:
	      for (const auto& [table_name, table_ptr] : global_table_dict){
            std::string info_clave = "Clave encontrada: [" + table_name + "] | Longitud: " + std::to_string(table_name.length());
            Logger::log(LogLevel::DEBUG, info_clave);
            int32_t n_rows;
            // Solo enviamos a escribir si la tabla existe:
            if (table_ptr != nullptr){
               // Sólo permitimos la escritura si el dat_ptr (no confundir con el buffer del disco) NO está vacío:
               if(table_ptr->data_ptr){
                  if(!table_ptr->data_ptr->columns.empty()){

                     Logger::log(LogLevel::DEBUG, "ESCRIBIMOS LA TABLA: " + table_name);
                     // Primero escribimos los metadatos:
                     uint32_t n_rows = 0;

                     n_rows = disk_io::write_table_metadata(table_ptr);
                     disk_io::write_table_data(table_ptr, n_rows);
                     // Actualizamos los metadatos de la tabla para mostar el numero de filas escritas en disco:
                     table_ptr->metadata_ptr->n_filas_disco = n_rows;
                     return;
                  } else {
                     Logger::log(LogLevel::DEBUG, "EXISTE la tabla en el diccionario, pero NO tiene datos en RAM viva. NO LA ESCRIBIMOS");
                  };
               } else {
                  Logger::log(LogLevel::DEBUG, "EXISTE la tabla en el diccionario. NO EXISTE SU ENTRADA DE DATOS. NO LA ESCRIBIMO0S");
               };
            }else{
                  Logger::log(LogLevel::DEBUG, "ERROR: Se ha encontrado una entrada vacia para la tabla: " + table_name);
            };
	      };
      };
   };









void disk_io::debug_print_metadatos_memoria() {
    Logger::log(LogLevel::DEBUG, "======= DEBUG DE METADATOS EN MEMORIA =======");
    
    if (global_table_dict.empty()) {
        Logger::log(LogLevel::ERROR, "El diccionario global de tablas está VACÍO.");
        return;
    }

    for (auto const& [nombre, tabla_ptr] : global_table_dict) {
        Logger::log(LogLevel::DEBUG, "TABLA (Dicc): " + nombre);
        
        if (!tabla_ptr || !tabla_ptr->metadata_ptr) {
            Logger::log(LogLevel::ERROR, "  [!] Error: Puntero a tabla o metadatos es NULL");
            continue;
        }

        table_metadata* meta = tabla_ptr->metadata_ptr;
        Logger::log(LogLevel::DEBUG, "  Nombre en Struct: " + meta->name);
        Logger::log(LogLevel::DEBUG, "  Filas en disco:   " + std::to_string(meta->n_filas_disco));
        
        size_t n_cols = meta->column_names.size();
        size_t n_types = meta->column_types.size();
        size_t n_pks = meta->primary_list.size();

        Logger::log(LogLevel::DEBUG, "  Sincronización de Vectores:");
        Logger::log(LogLevel::DEBUG, "    - Nombres: " + std::to_string(n_cols));
        Logger::log(LogLevel::DEBUG, "    - Tipos:   " + std::to_string(n_types));
        Logger::log(LogLevel::DEBUG, "    - PKs:     " + std::to_string(n_pks));

        Logger::log(LogLevel::DEBUG, "  DETALLE DE COLUMNAS:");
        // Iteramos sobre el máximo encontrado para detectar desajustes
        size_t max_idx = std::max({n_cols, n_types, n_pks});
        
        for (size_t i = 0; i < max_idx; i++) {
            std::string col_name = (i < n_cols) ? meta->column_names[i] : "!!! MISSING NAME !!!";
            std::string tipo_str = "NOT_READ";
            
            if (i < n_types) {
                switch(meta->column_types[i]) {
                    case dataType::INT:    tipo_str = "INT"; break;
                    case dataType::FLOAT:  tipo_str = "FLOAT"; break;
                    case dataType::STRING: tipo_str = "STRING"; break;
                    case dataType::BOOL:   tipo_str = "BOOL"; break;
                    case dataType::UNKNOWN: tipo_str = "UNKNOWN"; break;
                    // El compilador ya no se quejará, cubrimos todo el enum
                }
            }
            
            std::string es_pk = (i < n_pks && meta->primary_list[i]) ? "[PK]" : "    ";
            
            Logger::log(LogLevel::DEBUG, "    [" + std::to_string(i) + "] " + es_pk + " " + 
                        col_name + " (" + tipo_str + ")");
        }
    }
    Logger::log(LogLevel::DEBUG, "==============================================");
    Logger::flush();
};


