#include "disk_wal_read.h"


//========================================================
//== FUNION LECTURA METADATOS EN EL WAL: =================
//========================================================


char* disk_wal_read::recover_meta_wal_table_buffer(std::ifstream& in, const std::string& table_name_str_input){
   /*
   Función que devuelve un buffer con los metadatosdatos de ua tabla concreta, dado su nombre.
   */

   Logger::log(LogLevel::DEBUG, "Inside the metadata buffer reading function");
  
   char* buffer = nullptr;
   // Ahora vemos si el archivo de metadatos ya existe o no:
   if (fs::exists("metadata/" + table_name_str_input + "_meta.bin")) return buffer;
   Logger::log(LogLevel::DEBUG, "Metadata file does not exist, thus we carry on");
   // En caso de no existir, leemos los datos desde el WAL de metadatos:

   Logger::log(LogLevel::DEBUG, "Proceeding to read the metadata buffer:");
   // 1º) Leemos el tamaño del buffer de memoria de los metadatos de la tabla concreta:
   Logger::log(LogLevel::DEBUG, "1º) Reding the buffer's size");
   uint32_t buffer_size = 0;
   in.read(reinterpret_cast<char*>(&buffer_size), sizeof(uint32_t));
   try{
      buffer = new char[buffer_size];
      Logger::log(LogLevel::DEBUG, "   Buffer size read successfully");
      Logger::log(LogLevel::DEBUG, "2º) Reading the buffer");
      // 2º) Leemos el buffer de datos:
      in.read(buffer, buffer_size);
      Logger::log(LogLevel::DEBUG, "   Buffer read successfully");
   }catch(...){
      delete[] buffer;
      in.close();
      throw;
   };

   return buffer;

};


std::string disk_wal_read::recover_meta_wal_table_name(std::ifstream& in){
   /*
   Función que devuelve el nombre de la tabla de la que actualmente se está leyendo los datos/metadatos
   */
   Logger::log(LogLevel::DEBUG, "Inside the table's name retrieval function");
   // Leemos el nombre de la tabls
   Logger::log(LogLevel::DEBUG, "Reading the table name's size");
   uint32_t table_name_size = 0;
   in.read(reinterpret_cast<char*>(&table_name_size), sizeof(uint32_t));
   Logger::log(LogLevel::DEBUG, "Reading the table's name");
   std::string table_name_str;
   table_name_str.resize(table_name_size);   // ← reservar memoria
   in.read(table_name_str.data(), table_name_size);
   Logger::log(LogLevel::DEBUG, "Table name retrieved successfully");
   return table_name_str;
};


void disk_wal_read::aux_read_single_table_wal_metadata(std::ifstream& in, const std::string& table_name_str_input){
   /*
   Función auxliar que lee los metadatos de una sola tabla desde el WAL de metadatos de una tabla
   */
   Logger::log(LogLevel::DEBUG, "Inside the WAL metadata reading function");
   // Leemos el nombre de la tabla:
   Logger::log(LogLevel::DEBUG, "Proceeding to retrieve the table's name");
   Logger::log(LogLevel::DEBUG, "Table name retrieved successfully");
   // Recuperamos el buffer de datos:
   Logger::log(LogLevel::DEBUG, "Proceeding to retrieve metadata buffer");
   char* buffer = nullptr;
   buffer = disk_wal_read::recover_meta_wal_table_buffer(in, table_name_str_input);
   Logger::log(LogLevel::DEBUG, "Metadata buffer retrieved successfully");

   /////////////////////////////////////////////
   // Antes de leer vemos is los metadatos de la tabla están en memoria RAM:
   auto it = global_table_dict.find(table_name_str_input);
   if(it != global_table_dict.end()){
      Logger::log(LogLevel::DEBUG, "Metadata has already been loades on to the table's metadata section. Therefore, we skip the next step");
      return;
   };
   // No existe en los metadatos de la RAM, por lo que creamos la entrada:
   Logger::log(LogLevel::DEBUG, "Table: ", false, true);
   Logger::log(LogLevel::DEBUG, table_name_str_input, false, false);
   Logger::log(LogLevel::DEBUG, " does not exist. Proceeding to create it", true, false);

   // Si el puntero es nulo, es que el mapa acaba de crear la entrada vacía
   table* tb_created = new table();
   global_table_dict[table_name_str_input] = tb_created;

   Logger::log(LogLevel::DEBUG, "Table created successfully");
   // Creamos además sus metadatos:
   Logger::log(LogLevel::DEBUG, "Creating a new metadata pointer for the newly spawned table");
   table_metadata* metadata_pointer = new table_metadata;
   tb_created->metadata_ptr = metadata_pointer;
   Logger::log(LogLevel::DEBUG, "Metadata pointer created successfully");
   Logger::log(LogLevel::DEBUG, "Proceeding to fill metadata information");
   Logger::flush(LogLevel::DEBUG);

   // Pasamos a insertar el nombre en la RAM de latabla:
   Logger::log(LogLevel::DEBUG, "Inserting the name");
   ((*(tb_created->metadata_ptr)).name) = table_name_str_input;
   Logger::log(LogLevel::DEBUG, "Nome inserted successfully");


   /////////////////////////////////////////////
   // Ya recuperado el buffer de datos, debemos leerlo y rellenar los metadatos de la tabla en RAM
   Logger::log(LogLevel::DEBUG, "Initializing offse");
   uint32_t offset_read = 0; // Offset de lectura

   // Leemos el número de columnas:
   Logger::log(LogLevel::DEBUG, "Copying columns number");
   uint32_t num_cols = 0;
   std::memcpy(&num_cols, buffer + offset_read, sizeof(uint32_t));
   offset_read += sizeof(uint32_t);

   tb_created->metadata_ptr->n_cols = num_cols;

   Logger::log(LogLevel::DEBUG, "Columns number copyed successfully");

   Logger::log(LogLevel::DEBUG, "Nº of columns read is: ", false, true);
   Logger::log(LogLevel::DEBUG, num_cols, true, false);
   Logger::flush(LogLevel::DEBUG);


   Logger::log(LogLevel::DEBUG, "Entering the column loop (for registering it within the metadata segment)");
   // Ahora iteramos por cada columna:
   for(int i = 0; i<num_cols; i++){

      // Leemos el nombre de la columna:
      uint32_t size_column_name = 0;
      std::memcpy(&size_column_name, buffer + offset_read, sizeof(uint32_t));
      offset_read += sizeof(uint32_t);

      std::string column_name;
      column_name.resize(size_column_name);
      std::memcpy(column_name.data(), buffer + offset_read, size_column_name);
      offset_read += size_column_name;

      tb_created->metadata_ptr->column_names.push_back(column_name);

      // Ahora leemos el tipo de dato:
      uint32_t col_type_disk_int;
      std::memcpy(&col_type_disk_int, buffer + offset_read, sizeof(uint32_t));
      dataType col_type_disk = static_cast<dataType>(col_type_disk_int);
      offset_read += sizeof(uint32_t);

      tb_created->metadata_ptr->column_types.push_back(col_type_disk);

      // Ahora leemos si la variable es clave primaria o no:
      uint8_t key_val_int;
      bool key_val = true;
      std::memcpy(&key_val_int, buffer + offset_read, sizeof(uint8_t));
      offset_read += sizeof(uint8_t);
      if(key_val_int == 0){
         key_val = false;
      };
      tb_created->metadata_ptr->primary_list.push_back(key_val);
   };

   delete[] buffer;

};


// FUNCIONES AUXILIARES:
bool disk_wal_read::is_eof_read(std::ifstream& in) {

    // 1º Guardamos la posición actual exacta
    std::streampos current_pos = in.tellg();
    
    // Si el flujo ya está roto o ha fallado antes, tellg() devuelve -1
    if (current_pos == std::streampos(-1)) {
        Logger::log(LogLevel::DEBUG, "[EOF] Pointer value is -1. End of file or reading failure.");
        return true;
    }

    // 2º Guardamos las banderas de estado actuales (good, eof, fail)
    // para que el viaje al final del archivo no corrompa el flujo
    std::ios_base::iostate original_flags = in.rdstate();

    // 3º Saltamos al final del archivo para medirlo
    in.seekg(0, std::ios::end);
    std::streampos end_pos = in.tellg();

    // 4º Volvemos de inmediato a donde estábamos
    in.seekg(current_pos);
    in.clear();
    in.setstate(original_flags);

    // 5º Traducimos las posiciones a números enteros normales (bytes) para operar con ellos
    int64_t current_bytes = static_cast<int64_t>(current_pos);
    int64_t total_bytes = static_cast<int64_t>(end_pos);
    int64_t remaining_bytes = total_bytes - current_bytes;

    // 6º LOGS DE CONTROL VISUAL: Aquí verás la verdad del archivo
    Logger::log(LogLevel::DEBUG, " -> Current reading pointer is in byte: ", false, true);
    Logger::log(LogLevel::DEBUG, current_bytes, true, false);
    Logger::log(LogLevel::DEBUG, " ->  WAL total size (bytes): ", false, true);
    Logger::log(LogLevel::DEBUG, total_bytes, true, false);
    Logger::log(LogLevel::DEBUG, " -> Reamining bytes in disk: ", false, true);
    Logger::log(LogLevel::DEBUG, remaining_bytes, true, false);

    // 7º Evaluación estricta con margen de seguridad para "bytes fantasma"
    // Si quedan 0 bytes, es el final. 
    // Si queda 1 byte (por ejemplo, un '\n' o un 0x00 residual del formateo), también es el final.
    if (remaining_bytes <= 1) {
        Logger::log(LogLevel::DEBUG, "[EOF] DETECTED: 1 or 0 bytes reamain. Stoping the parsing procedure.");
        return true;
    }

    Logger::log(LogLevel::DEBUG, "[EOF] NOT DETECTED: We carry on with the reading.");
    return false;
};




//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////



void disk_wal_read::walDataReader::fill_buffer(){

   uint32_t var_size = 0;

   switch(this->data_types[this->col_counter]){

      case dataType::INT: {
         Logger::log(LogLevel::DEBUG, "INT Case");
         var_size = sizeof(int);
         int int_val = 4;
         if(var_size <= this->buffer_bytes_available){
            Logger::log(LogLevel::DEBUG, "There is still data within the buffer");
            std::memcpy(&int_val, this->data_buffer_pointer, var_size);
            Logger::log(LogLevel::DEBUG, "memcpy completed");
            this->value = int_val;
            Logger::log(LogLevel::DEBUG, "Buffer data copied to the variant value successfully");

            (*this->columns)[this->col_names[this->col_counter]].push_back(this->value);


            Logger::log(LogLevel::DEBUG, "Value added to the table");

            // We update counters:
            this->bytes_read += var_size;
            this->buffer_bytes_read += var_size;
            this->buffer_bytes_available -= var_size;
            this->data_buffer_pointer += var_size;



            // We add up one row:
            this->col_counter += 1;
            break;

         };
         Logger::log(LogLevel::DEBUG, "OFFSET CONDITION");
         this->offset = true;
         break;
      };

      case dataType::FLOAT: {
         Logger::log(LogLevel::DEBUG, "FLOAT case");
         var_size = sizeof(float);
         float float_val = 5;
         if(var_size <= this->buffer_bytes_available){
            std::memcpy(&float_val, this->data_buffer_pointer, var_size);
            this->value = float_val;

            (*this->columns)[this->col_names[this->col_counter]].push_back(this->value);

            // We update counters:
            this->bytes_read += var_size;
            this->buffer_bytes_read += var_size;
            this->buffer_bytes_available -= var_size;
            this->data_buffer_pointer += var_size;



            // We add up one row:
            this->col_counter += 1;
            break;

         };
         Logger::log(LogLevel::DEBUG, "OFFSET CONDITION");
         this->offset = true;
         break;
      };

      case dataType::BOOL: {
         Logger::log(LogLevel::DEBUG, "BOOL case");
         var_size = sizeof(uint8_t);
         uint8_t tmp_val_bool_int = 0;
         bool tmp_bool_var = false;
         if(var_size <= this->buffer_bytes_available){
            std::memcpy(&tmp_val_bool_int, this->data_buffer_pointer, var_size);
            if(tmp_val_bool_int == 1){
               tmp_bool_var= true;
            };
            this->value = tmp_bool_var;

            (*this->columns)[this->col_names[this->col_counter]].push_back(this->value);

            // We update counters:
            this->bytes_read += var_size;
            this->buffer_bytes_read += var_size;
            this->buffer_bytes_available -= var_size;
            this->data_buffer_pointer += var_size;



            // We add up one row:
            this->col_counter += 1;
            break;

         };
         Logger::log(LogLevel::DEBUG, "OFFSET CONDITION");
         this->offset = true;
         break;
      };

      case dataType::STRING: {
         Logger::log(LogLevel::DEBUG, "STRING case");
         var_size = sizeof(uint32_t);

         // Fase 1: Lectura de la longitud
         if(!this->len_read){
            if(var_size <= this->buffer_bytes_available){
               this->var_len_bytes = 0;
               std::memcpy(&this->var_len_bytes, this->data_buffer_pointer, var_size);
               this->bytes_read += var_size;
               this->buffer_bytes_read += var_size;
               this->buffer_bytes_available -= var_size;
               this->data_buffer_pointer += var_size;
               this->len_read = true;
               this->var_len_bytes_read = 0;

               Logger::log(LogLevel::DEBUG, "Creating a temporary string for storing current string data from disk");
               this->value = std::string();
               Logger::log(LogLevel::DEBUG, "String created and asociated with variant value successfully");
               std::string& str_tmp = std::get<std::string>(this->value);
               Logger::log(LogLevel::DEBUG, "Access to variant value string successful");
               str_tmp.reserve(this->var_len_bytes);
               Logger::log(LogLevel::DEBUG, "String content reservation successful");
               Logger::log(LogLevel::DEBUG, "Number of bytes reserved:: ", false, true);
               Logger::log(LogLevel::DEBUG, this->var_len_bytes, true, false);
            }else{
               Logger::log(LogLevel::DEBUG, "OFFSET CONDITION");
               this->offset = true;
               break;
            };
         };
         
         // Fase 2:
         if (this->len_read && this->var_len_bytes > 0) {
            uint32_t bytes_to_read_tmp = this->buffer_bytes_available;
            if(this->buffer_bytes_available > this->var_len_bytes){
               bytes_to_read_tmp = this->var_len_bytes;
            };
            if (bytes_to_read_tmp > 0) {
               Logger::log(LogLevel::DEBUG, "Recovering variant value's string:");
               std::string& str_tmp = std::get<std::string>(this->value);
               str_tmp.append(this->data_buffer_pointer, bytes_to_read_tmp);

               this->bytes_read += bytes_to_read_tmp;
               this->buffer_bytes_read += bytes_to_read_tmp;
               this->buffer_bytes_available -= bytes_to_read_tmp;
               this->data_buffer_pointer += bytes_to_read_tmp;
               this->var_len_bytes_read += bytes_to_read_tmp;
               this->var_len_bytes -= bytes_to_read_tmp;

            };
            // If we have depleted the buffer's capacity:
            if (this->var_len_bytes > 0 && this->buffer_bytes_available == 0) {
               Logger::log(LogLevel::DEBUG, "String fragmented into WAL blocks. Forcing offset condition.");
               this->offset = true;
               break; // Salimos del case para ir al ciclo de recarga de buffer
            };
         };

         // Fase 3:
         if (this->len_read && this->var_len_bytes == 0) {
            Logger::log(LogLevel::DEBUG, "String has been read whole or is empty. Saving into the table.");
            

            (*this->columns)[this->col_names[this->col_counter]].push_back(this->value);
            
            // Reseteo de flags de control para permitir avanzar a la siguiente columna
            this->col_counter += 1;
            this->var_len_bytes = 0;
            this->var_len_bytes_read = 0;
            this->len_read = false;
         }
         break;
      
      };


      case dataType::UNKNOWN: {
         Logger::log(LogLevel::DEBUG, "UNKNOWN case");
         var_size = sizeof(uint32_t);

         // Fase 1: Lectura de la longitud
         if(!this->len_read){
            if(var_size <= this->buffer_bytes_available){
               this->var_len_bytes = 0;
               std::memcpy(&this->var_len_bytes, this->data_buffer_pointer, var_size);
               this->bytes_read += var_size;
               this->buffer_bytes_read += var_size;
               this->buffer_bytes_available -= var_size;
               this->data_buffer_pointer += var_size;
               this->len_read = true;
               this->var_len_bytes_read = 0;

               Logger::log(LogLevel::DEBUG, "Creating a temporary character array for storing current string data from disk");
               this->value = std::vector<char>();
               Logger::log(LogLevel::DEBUG, "Array created and asociated with variant value successfully");
               std::vector<char>& vec_tmp = std::get<std::vector<char>>(this->value);
               Logger::log(LogLevel::DEBUG, "Access to variant value array successful");
               vec_tmp.reserve(this->var_len_bytes);
               Logger::log(LogLevel::DEBUG, "Array content reservation successful");
               Logger::log(LogLevel::DEBUG, "Number of bytes reserved:: ", false, true);
               Logger::log(LogLevel::DEBUG, this->var_len_bytes, true, false);
            }else{
               Logger::log(LogLevel::DEBUG, "OFFSET CONDITION");
               this->offset = true;
               break;
            };
         };
         
         // Fase 2:
         if (this->len_read && this->var_len_bytes > 0) {
            uint32_t bytes_to_read_tmp = this->buffer_bytes_available;
            if(this->buffer_bytes_available > this->var_len_bytes){
               bytes_to_read_tmp = this->var_len_bytes;
            };
            if (bytes_to_read_tmp > 0) {
               Logger::log(LogLevel::DEBUG, "Recovering variant value's character array:");
               std::vector<char>& vec_tmp = std::get<std::vector<char>>(this->value);
               vec_tmp.insert(
                     vec_tmp.end(), 
                     this->data_buffer_pointer, 
                     this->data_buffer_pointer + bytes_to_read_tmp
                  );

               this->bytes_read += bytes_to_read_tmp;
               this->buffer_bytes_read += bytes_to_read_tmp;
               this->buffer_bytes_available -= bytes_to_read_tmp;
               this->data_buffer_pointer += bytes_to_read_tmp;
               this->var_len_bytes_read += bytes_to_read_tmp;
               this->var_len_bytes -= bytes_to_read_tmp;

            };
            // If we have depleted the buffer's capacity:
            if (this->var_len_bytes > 0 && this->buffer_bytes_available == 0) {
               Logger::log(LogLevel::DEBUG, "Array fragmented into WAL blocks. Forcing offset condition.");
               this->offset = true;
               break; // Salimos del case para ir al ciclo de recarga de buffer
            };
         };

         // Fase 3:
         if (this->len_read && this->var_len_bytes == 0) {
            Logger::log(LogLevel::DEBUG, "Array has been read whole or is empty. Saving into the table.");

            (*this->columns)[this->col_names[this->col_counter]].push_back(this->value);
            
            // Reseteo de flags de control para permitir avanzar a la siguiente columna
            this->col_counter += 1;
            this->var_len_bytes = 0;
            this->var_len_bytes_read = 0;
            this->len_read = false;
         }
         break;
      
      };

   };
};




void disk_wal_read::walDataReader::control_unit(){
   switch(this->state){
      case 0: {
         Logger::log(LogLevel::DEBUG, "State 0");

         // We recover the table's object:
         auto it = global_table_dict.find(this->table_name);


         if (it != global_table_dict.end()) {
            this->table = it->second;
         } else {
            this->state = 255;
            break;
         };
         this->table = it->second;
         Logger::log(LogLevel::DEBUG, "Table has been found");
         this->metadata = this->table->metadata_ptr;
         Logger::log(LogLevel::DEBUG, "Metadata pointer retrieval successful");
         this->num_cols = this->metadata->n_cols;
         Logger::log(LogLevel::DEBUG, "Table's number of columns: ", false, true);
         Logger::log(LogLevel::DEBUG, this->num_cols, true, false);
         this->col_names = this->metadata->column_names;
         this->data_types = this->metadata->column_types;

         // In case the data pointer has not yet been created:
         table_data*& tb_d_ptr = this->table->data_ptr;
         if(this->table->data_ptr == nullptr){
            this->table->data_ptr = new table_data;
         };

         this->columns = &(tb_d_ptr->columns);

         // We read the numbers of rows to process:
         Logger::log(LogLevel::DEBUG, "Reading number of rows:");
         this->in.read(reinterpret_cast<char*>(&this->num_rows), sizeof(uint32_t));

         // We read the buffer size:
         this->in.read(reinterpret_cast<char*>(&this->buffer_tmp_size), sizeof(uint32_t));
         Logger::log(LogLevel::DEBUG, "Buffer size: ", false, true);
         Logger::log(LogLevel::DEBUG, this->buffer_tmp_size, true, false);
         this->buffer_bytes_available = this->buffer_tmp_size;
         // First we read the buffer size:
         this->buffer = new char[this->buffer_tmp_size];
         this->data_buffer_pointer = this->buffer;

         Logger::log(LogLevel::DEBUG, "Table's number of rows: ", false, true);
         Logger::log(LogLevel::DEBUG, this->num_rows, true, false);
         this->state = 1;
         break;
      };

      case 1: {
         Logger::log(LogLevel::DEBUG, "State 1");
         this->eof_rows = is_eof_rows();
         if(this->eof_rows){
            // End of reading, proceeding to empty char buffer
            this->state = 255;
            break;
         } else {
            this->state = 2;
            break;
         };
      };

      case 2: {
         Logger::log(LogLevel::DEBUG, "State 2");
         this->in.read(this->buffer, this->buffer_tmp_size);
         Logger::log(LogLevel::DEBUG, "--- CRAW BUFFER DATA HASBEEN READ ---");
         std::string debug_str(this->buffer, this->buffer_tmp_size);
         Logger::log(LogLevel::DEBUG, debug_str);
         this->state = 3;
         break;
      };

      case 3: {
         Logger::log(LogLevel::DEBUG, "State 3");

         this->state = 4;

         Logger::log(LogLevel::DEBUG, "++++++++++++++++++++++++++++++++");
         Logger::log(LogLevel::DEBUG, "Read bytes: ", false, true);
         Logger::log(LogLevel::DEBUG, this->bytes_read, true, false);
         Logger::log(LogLevel::DEBUG, "'buffer_bytes_read': ", false, true);
         Logger::log(LogLevel::DEBUG, this->buffer_bytes_read, true, false);
         Logger::log(LogLevel::DEBUG, "Buffer's available bytes: ", false, true);
         Logger::log(LogLevel::DEBUG, this->buffer_bytes_available, true, false);

         Logger::log(LogLevel::DEBUG, "++++++++++++++++++++++++++++++++");
         Logger::log(LogLevel::DEBUG, "Column counter: ", false, true);
         Logger::log(LogLevel::DEBUG, this->col_counter, true, false);
         Logger::log(LogLevel::DEBUG, "Row counter: ", false, true);
         Logger::log(LogLevel::DEBUG, this->row_counter, true, false);

         // EXECUTE VARIANT VALUE BUFFER FILLING:
         disk_wal_read::walDataReader::fill_buffer();

         break;
      };

      case 4: {
         Logger::log(LogLevel::DEBUG, "State 4");
         Logger::log(LogLevel::DEBUG, "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
         Logger::log(LogLevel::DEBUG, this->col_counter, false, true);
         Logger::log(LogLevel::DEBUG, " >= ", false, false);
         Logger::log(LogLevel::DEBUG, this->num_cols, false, false);
         Logger::log(LogLevel::DEBUG, " && ", false, false);
         Logger::log(LogLevel::DEBUG, this->row_counter + 1, false, false);
         Logger::log(LogLevel::DEBUG, " >=", false, false);
         Logger::log(LogLevel::DEBUG, this->num_rows, true, false);
         Logger::log(LogLevel::DEBUG, "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
         this->eof_rows = is_eof_rows();
         if(this->eof_rows){
            // End of reading, proceeding to empty char buffer
            this->state = 255;
            break;
         } else {
            this->state = 5;
            break;
         };
      };

      case 5: {
         Logger::log(LogLevel::DEBUG, "State 5");
         if(this->col_counter >= this->num_cols){
            this->col_counter = 0;
            this->row_counter += 1;
         };

         if(this->offset){
            // Insufficient capacity in buffer
            this->bytes_read = this->buffer_tmp_size;
            this->buffer_bytes_read = 0;
            this->data_buffer_pointer = this->buffer;
            this->offset = false;
            this->state = 1;
            this->buffer_bytes_available = this->buffer_tmp_size;
            break;
         };
         this->state = 3;
         break;
      };

      case 255: {
         Logger::log(LogLevel::DEBUG, "State 255");

         Logger::flush(LogLevel::DEBUG);
         this->metadata->n_rows_ram += (this->row_counter + 1);
         Logger::log(LogLevel::DEBUG, "Metadata's 'n_rows_ram' has been filled with the number of rows successfully: ", false, true);
         Logger::log(LogLevel::DEBUG, this->metadata->n_rows_ram, true, false);
         break;
      };
   };
};


void disk_wal_read::walDataReader::execute_fsm() {
   bool aux_bool = true;
   while(aux_bool) {
      if(this->state == 255){
         aux_bool = false;
      };
      this->control_unit();
   }
}





// NUEVO CÓDIGO:


void disk_wal_read::walDataParser::control_unit(){
   switch(this->state){

      case 0: {
         std::filesystem::path tmp_path = "backup_data/wal.bin";
         Logger::log(LogLevel::DEBUG, "walDataParser State 0");
         if(!fs::exists(tmp_path)){
            this->state = 255;
            break;
         };
         this->in.open(tmp_path, std::ios::binary);
      };

      case 1: {
         Logger::log(LogLevel::DEBUG, "walDataParser State 1");
         if(disk_wal_read::is_eof_read(this->in)){
            Logger::log(LogLevel::DEBUG, "<<<<<<<<<<<< WAL END >>>>>>>>>>>>>>>>>>>>>>>");
            this->state = 255;
            break;
         };
         // We continuiue to type reading:
         this->state = 2;
         break;
      };

      case 2: {
         Logger::log(LogLevel::DEBUG, "walDataParser State 2");
         this->in.read(reinterpret_cast<char*>(&this->wal_data_type), sizeof(uint8_t));
         this->state = 3;
         break;
      };

      case 3: {
         Logger::log(LogLevel::DEBUG, "walDataParser State 3");
         this->table_name = disk_wal_read::recover_meta_wal_table_name(this->in);
         Logger::log(LogLevel::DEBUG, "Retrieved table name is: ", false, true);
         Logger::log(LogLevel::DEBUG, this->table_name, true, false);

         // We redirect depending on thew WAL data type:
         // Si tipo_dato es 0 es metadato y si es 1 es dato
         if(this->wal_data_type == 0){
            // Metadato:
            Logger::log(LogLevel::DEBUG, "Metadata", true, true);
            this->state = 4;
            break;
         } else {
            // Dato:
            Logger::log(LogLevel::DEBUG, "Data", true, true);
            this->state = 5;
            break;
         };
      }

      case 4: {
         Logger::log(LogLevel::DEBUG, "walDataParser State 4");
         // Metadata read:
         disk_wal_read::aux_read_single_table_wal_metadata(this->in, this->table_name);
         Logger::log(LogLevel::DEBUG, "Nº of table's columns: ", false, true);
         auto it = global_table_dict.find(this->table_name);
         table* table_ptr = it->second;
         uint32_t num_cols_tmp = table_ptr->metadata_ptr->n_cols;
         Logger::log(LogLevel::DEBUG, num_cols_tmp, true, false);
         this->state = 1;
         break;
      };

      case 5: {
         Logger::log(LogLevel::DEBUG, "walDataParser State 5");
         // Data read:

         // We create the data reader object:
         disk_wal_read::walDataReader data_reader_obj(this->in, this->table_name); 
         // We execute the data reader FSM within:
         data_reader_obj.execute_fsm();

         this->state = 1;
         break;
      };

      case 255: {
         Logger::log(LogLevel::DEBUG, "walDataParser State 255");
         break;
      };
   }
}

void disk_wal_read::walDataParser::execute_fsm() {
   bool aux_bool = true;
   while(aux_bool) {
      if(this->state == 255){
         aux_bool = false;
      };
      this->control_unit();
   }
}


// Nueva función de lectura del WAl:
void disk_wal_read::read_wal(){
   // Whole WAl reading process:
   // We create the WAL data parser object:
   disk_wal_read::walDataParser data_parser_obj; 
   // We execute the data reader FSM within:
   data_parser_obj.execute_fsm();
};