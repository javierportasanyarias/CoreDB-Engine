bool disk_io::is_eof(std::ifstream& in, std::streampos end_pos){

   // Calculamos el tamaño del archivo:
   std::streampos current_pos = in.tellg();
   return current_pos > end_pos;
};


bool disk_io::is_eof_2(uint32_t puntero_lectura, uint32_t size_archivo){

   // Calculamos el tamaño del archivo con los punteros manuales:
   return puntero_lectura > size_archivo;
};


void disk_io::write_in_memory_with_data_buffer(dataType tipo_dato, std::vector<std::string> col_names, uint32_t& size_disponible_buffer, uint32_t& contador_bytes_buffer, std::map<std::string, std::vector<Values>>& columnas, int contador_cols, std::array<char, 128>& buffer, uint32_t& offset){

   /*
   Dado el buffer de datos y el tipo de dato, se asigna el valor a la memoria RAM de la tabla.
   Siempre en caso de no superar el 
   */
   Logger::log(LogLevel::DEBUG, "Entramos en la asignacion de memoria");

   if(offset == 0){

      switch(tipo_dato){

         case dataType::INT: {
            Logger::log(LogLevel::DEBUG, "CASO ENTERO");
            if(sizeof(int) > size_disponible_buffer){
               offset = sizeof(int); 
               size_disponible_buffer -= sizeof(int);
               break;
            };
            size_disponible_buffer -= sizeof(int);
            // Podemos proceder a la lectura:
            int valor_tmp;
            std::memcpy(&valor_tmp, buffer.data() + contador_bytes_buffer, sizeof(int));
            contador_bytes_buffer += sizeof(int);
            columnas[col_names[contador_cols]].push_back(valor_tmp);
            break;
         };
         case dataType::FLOAT: {
            Logger::log(LogLevel::DEBUG, "CASO FLOAT");
            if(sizeof(float) > size_disponible_buffer){
               offset = sizeof(float);
               size_disponible_buffer -= sizeof(float);
               break;
            };
            size_disponible_buffer -= sizeof(float);
            // Podemos proceder a la lectura:
            float valor_tmp;
            std::memcpy(&valor_tmp, buffer.data() + contador_bytes_buffer, sizeof(float));
            contador_bytes_buffer += sizeof(float);
            columnas[col_names[contador_cols]].push_back(valor_tmp);
            break;
         };
         case dataType::BOOL: {
            Logger::log(LogLevel::DEBUG, "CASO BOOLEANO");
            if(sizeof(uint8_t) > size_disponible_buffer){
               offset = sizeof(uint8_t);
               size_disponible_buffer -= sizeof(uint8_t);
               break;
            };
            size_disponible_buffer -= sizeof(uint8_t);
            // Podemos proceder a la lectura:
            uint8_t valor_tmp;
            std::memcpy(&valor_tmp, buffer.data() + contador_bytes_buffer, sizeof(uint8_t));
            contador_bytes_buffer += sizeof(uint8_t);
            // En este caso convertimos el valor a uno booleano:
            bool buffer_bool = false;
            if(valor_tmp == 1){
               buffer_bool = true;
            };
            columnas[col_names[contador_cols]].push_back(buffer_bool);
            break;
         };
         case dataType::STRING: {
            Logger::log(LogLevel::DEBUG, "CASO STRING");

            uint32_t string_size;
            // Antes nos aseguramos de que quede espacio para leer el tamaño de la string:
            if(sizeof(uint32_t) > size_disponible_buffer){
               offset = sizeof(uint32_t);
               size_disponible_buffer -= sizeof(uint32_t);
               break;
            };
            size_disponible_buffer -= sizeof(uint32_t);
            // Pasados este punto, hemos podido leer el tamaño de la string entero
            // Ahora veremos si podemos leer su tamño y contenido de una:


            // Leemos del buffer el tamaño de la string
            std::memcpy(&string_size, buffer.data() + contador_bytes_buffer, sizeof(uint32_t));
            contador_bytes_buffer += sizeof(uint32_t);

            // Ahora vemos si tenemos espacio suficiente para leer la string como tal;
            if(string_size + sizeof(uint32_t) > size_disponible_buffer){
               offset = sizeof(string_size) + sizeof(uint32_t);
               size_disponible_buffer -=  string_size;
               break;
            };
            size_disponible_buffer -=  string_size;
            // Realizamos la lectura:
            std::string valor_tmp;
            valor_tmp.resize(string_size); // Resize de la string
            std::memcpy(valor_tmp.data(), buffer.data() + contador_bytes_buffer, string_size);
            contador_bytes_buffer += string_size;
            columnas[col_names[contador_cols]].push_back(valor_tmp);
         };
      };
      Logger::log(LogLevel::DEBUG, "Hemos terminado la asignacion de memoria");
   } else {
      Logger::log(LogLevel::DEBUG, "NO se ha entrado a la asignacion de memoria");
   };
};

void disk_io::lectura_datos_monolitica(table* tabla){

   // Abrimos el archivo:
   std::string nombre_tabla = tabla->metadata_ptr->name;
   std::string ruta_tabla = "data/" + nombre_tabla + "_data.bin";
   std::ifstream in(ruta_tabla, std::ios::binary);

   // Calculamos el tamaño del archivo:
   in.seekg(0, std::ios::end);
   std::streampos end_pos = in.tellg();
   in.seekg(0, std::ios::beg);
   std::streampos ini_pos = in.tellg();
   std::streampos file_size = end_pos - ini_pos;

   // PUNTEROS AUXILIARES:
   uint32_t posicion_inicial = 0;
   //uint32_t posicion_final = 0;
   uint32_t buffer_size = 128;
   uint32_t offset = 0;
   uint32_t final_archivo = static_cast<uint32_t>(file_size);
   Logger::log(LogLevel::DEBUG, "EL TAMANO DEL ARCHIVO ES: ", false, true);
   Logger::log(LogLevel::DEBUG, final_archivo, true, false);
   Logger::flush();
   uint32_t tamano_archivo_disponible = final_archivo;
   uint32_t size_dinamico_lectura = 0;
   uint32_t puntero_lectura = 0;

   // incializamos el buffer de lectura
   std::array<char, 128> buffer;

   Logger::flush();
   Logger::flush();
   Logger::log(LogLevel::DEBUG, "7777777777777777777777777777777777777777777");
   Logger::flush();
   Logger::log(LogLevel::DEBUG, "SE HA INICIADO LA LECTURA CON BUFFER DE LOS DATOS:");



   // Accedemos al tipo de datos desde el esquema:
   table_metadata* metadata = tabla->metadata_ptr;
   std::vector<dataType> tipos_datos = tabla->metadata_ptr->column_types;
   uint32_t n_cols = tipos_datos.size();
   std::vector<std::string> col_names = tabla->metadata_ptr->column_names;
   uint32_t n_filas;

   // Leemos el número de filas:
   in.read(reinterpret_cast<char*>(&n_filas), sizeof(uint32_t));

   int aux_counter = 1;

   // uint32_t contador_columna = 0;


   uint32_t size_disponible = 0;

   std::streampos current_pos = in.tellg();
   uint32_t posicion_actual = static_cast<uint32_t>(current_pos);

   Logger::log(LogLevel::DEBUG, "Se han leido el numero de filas");
   while(!disk_io::is_eof_2(puntero_lectura, size_dinamico_lectura)){
   //while(!disk_io::is_eof(in, end_pos)){
      Logger::log(LogLevel::DEBUG, "Iteracion del bucle while principal: ", false, true);
      Logger::log(LogLevel::DEBUG, aux_counter, true, false);
      Logger::log(LogLevel::DEBUG, "Posicion alctual del puntero de lectura: ", false, true);
      Logger::log(LogLevel::DEBUG, posicion_actual, true, false);


      Logger::flush();
      // Dentro del bucle de lectura principal:
      //posicion_final += buffer_size;
      // leemos el buffer.
      /*if(disk_io::is_eof_2(puntero_lectura, size_dinamico_lectura)){
      //if(disk_io::is_eof(in, end_pos)){
         Logger::log(LogLevel::DEBUG, "ESTAMOS EN LA ULTIMA LECTURA");
         // Esta será la última lectura:
         uint32_t final_calculado = final_archivo - posicion_inicial;
         size_dinamico_lectura = buffer_size - offset - final_calculado;
         //in.read(buffer.data() + offset , buffer_size - offset - final_calculado);
      } else {
         // No se ha llegado al final del archivo:
         size_dinamico_lectura = buffer_size - offset;
         //in.read(buffer.data() + offset , buffer_size - offset);
      };*/


      uint32_t final_calculado = final_archivo - posicion_inicial;
      size_dinamico_lectura = buffer_size - offset;
      if(size_dinamico_lectura > tamano_archivo_disponible){
         size_dinamico_lectura = tamano_archivo_disponible;
      };

      // Actualizamos la posicion_final:
      posicion_inicial += size_dinamico_lectura;

      // Justo antes de la lectura, movemos los bytes del offset, del final al principio:
      /*std::copy(
            buffer.end() - offset, 
            buffer.end(), 
            buffer.begin()
         );*/

      std::memmove(buffer.data(), buffer.data() + size_dinamico_lectura, offset);

      // Ahora si realizamos la lectura:
      in.read(buffer.data() + offset , size_dinamico_lectura);
      Logger::log(LogLevel::DEBUG, "Hemos leido estos bytes: ", false, true);
      Logger::log(LogLevel::DEBUG, size_dinamico_lectura, true, false);
      Logger::flush();
      puntero_lectura += size_dinamico_lectura;

      if(aux_counter == 300){
         break;
      };

      // Ahora ya podemos volver a filar el offset a cero:
      offset = 0;
      ///////////////////////////////////////////////////////////////////////////////////
      //Ya tenemos el buffer, ahora, procesamos su contenido

      // Leer por filas y columnas y con un contador de bytes para ver donde vamos y si dejamos una variable a medias:
      uint32_t contador_bytes_buffer = 0;

      size_disponible = size_dinamico_lectura;

      // inicializamos donde guardaremos los datos en disco:
      std::map<std::string, std::vector<Values>>& columnas = tabla->data_buffer_ptr->columns;
      while(size_disponible >= 0 && offset == 0){
         for(int j = 0; j<n_cols; j++){
            // Actualizamos el tamaño disponible del buffer
            //size_disponible = size_disponible - contador_bytes_buffer;

            Logger::log(LogLevel::DEBUG, "size_disponible: ", false, true);
            Logger::log(LogLevel::DEBUG, size_disponible, true, false);
            Logger::log(LogLevel::DEBUG, "offset: ", false, true);
            Logger::log(LogLevel::DEBUG, offset, true, false);
            Logger::flush();

            disk_io::write_in_memory_with_data_buffer(
               tipos_datos[j],
               col_names,
               size_disponible, // Size disponible del buffer
               contador_bytes_buffer,
               columnas,
               j,
               buffer,
               offset
            );


            // Si tenemos aprovisionamiento llenamos la sección de datos venidos de disco de la tabla en RAM
         }; // Fin de la iteración de una fila

      };

      aux_counter += 1;

      // Recalculamos el tamño disponbile del archivo:
      tamano_archivo_disponible -= size_dinamico_lectura;
      
   };
   in.close();
};