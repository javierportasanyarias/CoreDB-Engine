#include "disk_in.h"
#include "disk_buffer.h"


//====================================================
//========= tableRowIterator only disk ===============
//====================================================


// == constructor method =============================
disk_buffer::tableRowIterator_only_disk_part::tableRowIterator_only_disk_part(const std::string& table_name_str){

   counter = 0;
   table_ptr = global_table_dict.at(table_name_str);
   eof = false;
   
};


// == EOF query ======================================
bool disk_buffer::tableRowIterator_only_disk_part::is_eof() const {

   return eof;
};


// == Para obtener la próxima fila ===================
std::map<std::string, Values> disk_buffer::tableRowIterator_only_disk_part::get_next_row_only_disk_part(disk_in::read_table_iterator* table_reader_obj){

   // Realizamos la lectura:
   Logger::flush();
   Logger::log(LogLevel::DEBUG, "<<<<<<<< INSIDE DISK ITERATOR >>>>>>>>>>", true, false);
   Logger::flush();
   std::map<std::string, Values> map_row_returned;
   uint32_t n_f_disk = table_reader_obj->current_partition_n_rows;
   Logger::log(LogLevel::DEBUG, "Reading ", false, true);
   Logger::log(LogLevel::DEBUG, n_f_disk, false, false);
   Logger::log(LogLevel::DEBUG, " rows in total", true, false);
   Logger::log(LogLevel::DEBUG, "Counter value is: ", false, true);
   Logger::log(LogLevel::DEBUG, counter, true, false);

   std::vector<std::string>& col_names = table_ptr->metadata_ptr->column_names;
   // Condiciones de guarda, para ahorrar comprobaciones:
   if(counter >= n_f_disk){
      this->eof = true;
      Logger::log(LogLevel::DEBUG, "Row will return empty ('get_next_row_only_disk_part')");
      return map_row_returned;
   };
   table_data_buffer*& ptr_data = this->table_ptr->data_buffer_ptr;
   if(!ptr_data){
      this->eof = true;
      Logger::log(LogLevel::ERROR, "'data_buffer_ptr' pointer does NOT exist within the table object");
      return map_row_returned;
   }
   std::map<std::string, std::vector<Values>>& columns_map = ptr_data->columns;
   if(columns_map.empty()){
      this->eof = true;
      Logger::log(LogLevel::ERROR, "Column map is empty");
      return map_row_returned;
   };

   // Iteramos por cada columna:

   for (const std::string& col_name : col_names) {

      Logger::log(LogLevel::DEBUG, "Reding from the info retrieved from disk");
      Logger::log(LogLevel::DEBUG, "Counter value: ", false, true);
      Logger::log(LogLevel::DEBUG, counter, true, false);
      Logger::log(LogLevel::DEBUG, "Column name: ", false, true);
      Logger::log(LogLevel::DEBUG, col_name, true, false);

      Logger::log(LogLevel::DEBUG, "We write the value in <<<row to return>>>");
      Logger::log(LogLevel::DEBUG, "Column name: ", false, true);
      Logger::log(LogLevel::DEBUG, col_name, true, false);
      Logger::log(LogLevel::DEBUG, "Counter value: ", false, true);
      Logger::log(LogLevel::DEBUG, counter, true, false);
      map_row_returned[col_name] = columns_map.at(col_name)[counter];
      
      Logger::log(LogLevel::DEBUG, "Row retrieved data read from disk successfully");

   }; // Termina la iteracion de columna
   counter += 1;
   if(counter >= (n_f_disk)){
      this-> eof = true;
   };
   return map_row_returned;
}; // Termina el metodo 'get_next_row'*/



//====================================================
//== tableRowIterator only RAM =======================
//====================================================

// == constructor method =============================
disk_buffer::tableRowIterator_only_ram::tableRowIterator_only_ram(const std::string& table_name_str){
   /*
   Constructor de la clase 'tableRowIterator_only_ram'
   */
   counter = 0;
   table_ptr = global_table_dict.at(table_name_str);
   // Solo en caso de tenerlo, contamos las filas en RAM viva:



   this->n_rows_total = table_ptr->metadata_ptr->n_rows_ram;
   this->eof = (n_rows_total == 0);
};

// == EOF query ======================================
bool disk_buffer::tableRowIterator_only_ram::is_eof() const {
   /*
   Simple función para retornar el atributo booleano 'eof'
   */
   return eof;
};


// == Para obtener la próxima fila (RAM viva) ========
std::map<std::string, Values> disk_buffer::tableRowIterator_only_ram::get_next_row_ram(){
   /*
   Método parecido a 'get_next_row' de tableRowIterator. Sólo que aquí únicamente
   se retornaran datos de la misma sesión o RAM viva.
   Funciona de la siguiente manera:
      1º Itera por cada columna de la tabla.
      2º Dado el atributo 'counter' extrae dicho elemento de cada vector de columnas de cada columna,
         , siempre de los datos en RAM viva.
      3º Una vez terminado el bucle se actualiza el valor del atributo 'eof'.
      4º Se incrementa en uno el counter de fila.
   */
   std::map<std::string, Values> map_row_returned;

   std::vector<std::string>& col_names = table_ptr->metadata_ptr->column_names;
   // Condiciones de guarda, para ahorrar comprobaciones:
   if(counter >= this->n_rows_total){
      this->eof = true;
      Logger::log(LogLevel::DEBUG, "Row will return empty ('get_next_row_ram')");
      return map_row_returned;
   };
   table_data*& ptr_data = this->table_ptr->data_ptr;
   if(!ptr_data){
      this->eof = true;
      Logger::log(LogLevel::ERROR, "'data_ptr' pointer does NOT exist within the table object");
      return map_row_returned;
   };
   std::map<std::string, std::vector<Values>>& columns_map = ptr_data->columns;
   if(columns_map.empty()){
      this->eof = true;
      Logger::log(LogLevel::ERROR, "Column map is empty");
      return map_row_returned;
   };


   // Iteramos por cada columna:
   for (const std::string& col_name : col_names) {

      // Leemos desde la RAM viva:
      map_row_returned[col_name] = columns_map.at(col_name)[counter];
   };
   counter += 1;
   if(counter >= this->n_rows_total){
      this-> eof = true;
   };
   return map_row_returned;
}; // Termina el metodo 'get_next_row_ram'

//====================================================
//========= FSM iterator for disk and RAM ============
//====================================================

// == constructor method =============================
disk_buffer::tableRowIterator::tableRowIterator(const std::string& table_name_str){
   /*
   Constructir de la clase 'tableRowIterator'
   */


   Logger::flush();
   Logger::log(LogLevel::DEBUG, "////////////////////////////////////////////////");
   Logger::log(LogLevel::DEBUG, "FSM iterator (RAM and disk) initialized");

   // Obtenemos el puntero de la tabla:
   table_ptr = global_table_dict.at(table_name_str);
   first_execution = true;
   eof = false; // Este es el false general, sólo si ambosa son true este será true
   eof_ram = false;
   eof_partition = true; // Epieza siendo true porque sólo así se leerá des disco una iteración
   eof_disk = false;
   fsm_state = 0;
   table_name = this->table_ptr->metadata_ptr->name;

   // Iteradores auxiliares:
   this->iterator_ram = nullptr;
   this->table_reader_obj = nullptr;
   this->disk_iterator = nullptr;
   Logger::log(LogLevel::DEBUG, "Variables have been initialized successfully");

};

bool disk_buffer::tableRowIterator::is_eof() const {

   return eof;
};

// == Método Unidad de Control =============================
std::map<std::string, Values> disk_buffer::tableRowIterator::control_unit(){
   /*
   Este método es la Unidad de control de la Máquina de Estados finitos
   que recupera todas las filas, provengan de la misma sesión en memoria
   de sesiones anteriores proventienes del disco.
   
   Los iteradores auxiliares y la lectura en disco se crean en el HEAP,
   por lo que la propia máquina de estados finitos se encarga de eliminarlos.
   */
   Logger::log(LogLevel::DEBUG, "CU cycle");
   std::map<std::string, Values> map_row_returned = {};
   switch(this->fsm_state){
      case 0: {
         Logger::log(LogLevel::DEBUG, "State 0. FSM just started");
         Logger::log(LogLevel::DEBUG, "RAM reading");
         // Vemos si existen datos en RAM:
         if(this->table_ptr->data_ptr && !(this->table_ptr->data_ptr->columns.empty())){
            Logger::log(LogLevel::DEBUG, "There is data in the table's 'RAM'");
            Logger::log(LogLevel::DEBUG, "Creting RAM iterator:");
            this->iterator_ram = new disk_buffer::tableRowIterator_only_ram(this->table_name);
            Logger::log(LogLevel::DEBUG, "RAM iterator created successfully");
            this->fsm_state = 1;
            break;
         };
         // En caso de no haber datos en RAM, pasamos directamente a ver si hay en disco
         Logger::log(LogLevel::DEBUG, "There is NO data inside the 'RAM'. Switching to 'disk' reading");
         this->eof_ram = true;
         this->fsm_state = 2;
         break;
      };
      case 1: {
         // Iteracion de filas solo RAM
         Logger::log(LogLevel::DEBUG, "State 1. Reading a 'RAM' row:");
         map_row_returned = this->iterator_ram->get_next_row_ram();
         Logger::log(LogLevel::DEBUG, "RAM row retrieved successfully");
         
         // Vemos si está vacío lo que ha retornado:
         if(map_row_returned.empty()){
            Logger::log(LogLevel::DEBUG, "RAM row has returned empty.");
            Logger::log(LogLevel::DEBUG, "This means there is no more data left within the table's 'RAM'");
            this->eof_ram = true;
            // Cambiamos de estado:
            this->fsm_state = 2;
            // Eliminamos el iterador de la RAM:
            delete this->iterator_ram;
            this->iterator_ram = nullptr;
            Logger::log(LogLevel::DEBUG, "RAM iterator deletion completed");
            break;
         };
         this->fsm_state = 1;
         break;
      };
      case 2: {
         // Comprobamos de antemano si hay datos en DISCO:
         Logger::log(LogLevel::DEBUG, "State 2. Starting 'disk' reading");
         Logger::log(LogLevel::DEBUG, "Inspecting beforehand if there is data within the disk");
         if(std::filesystem::exists("data/" + this->table_name)){
            Logger::log(LogLevel::DEBUG, "There is data in the disk");
            this->fsm_state = 3;
            Logger::log(LogLevel::DEBUG, "Proceeding to create iterator for both reading and 'disk'");
            // En ese caso creamos el objeto de lectura desde el disco:
            this->table_reader_obj = new disk_in::read_table_iterator(this->table_ptr);
            // inicializamos también el iterados en disco:
            this->disk_iterator = new disk_buffer::tableRowIterator_only_disk_part(this->table_name); 

            break;
         };
         Logger::log(LogLevel::DEBUG, "There is NO data within the disk");
         this->eof_disk = true;
         this->eof = true; // Porque la RAM ya se ha explorado
         this->fsm_state = 255;
         break;
      };
      case 3: {
         // Lectura de una partición
         Logger::log(LogLevel::DEBUG, "State 3. Partition reading:");
         // Antes de leer la partición, 
         this->eof_disk = this->table_reader_obj->read_table();
         Logger::log(LogLevel::DEBUG, "Disk partition read successfully");
         Logger::log(LogLevel::DEBUG, "Restarting disk iterator counter");
         this->disk_iterator->counter = 0;
         Logger::log(LogLevel::DEBUG, "Disk iterator counter will be restarted each time a new partition is read");
         if(this->eof_disk){
            Logger::log(LogLevel::DEBUG, "All partitions have been read (disk EOF has been reached)");
            this->fsm_state = 255; // Esto quiere que se ha llegado al fin
            this->eof = true; // Porque la RAM tambien llego a su fin o no existe
            // Al llegar al fin, eliminamos el objeto de la lectura y de iteracion por el disco:
            Logger::log(LogLevel::DEBUG, "Deleting reading and disk iterators");
            delete this->table_reader_obj;
            this->table_reader_obj = nullptr;
            delete this->disk_iterator;
            this->disk_iterator = nullptr;
            break;
         };
         this->fsm_state = 4;
         break;
      };
      case 4: {
         // Iteracion de filas solo disco
         Logger::log(LogLevel::DEBUG, "State 4. Row reading:");
         map_row_returned =  this->disk_iterator->get_next_row_only_disk_part(table_reader_obj);
         Logger::log(LogLevel::DEBUG, "Row read from disk successfully");
         if(map_row_returned.empty()){
            Logger::log(LogLevel::DEBUG, "Row has returned empty from disk.");
            // Se ha alcanzado el fin de la partición:
            this->eof_partition = true;
            // Limpiamos la memoria de la tabla porviniente del disco:
            this->table_reader_obj->disk_data_ptr->columns.clear();
            // Cambiamos de estado:
            this->fsm_state = 3;
            break;
         };
         this->fsm_state = 4;
         break;
      };
   };
   // Actualizamos el eof general:
   this->eof = this->eof_ram && this->eof_disk;
   return map_row_returned;
};


// == Método Para devolver la próxima fila =============================
std::map<std::string, Values> disk_buffer::tableRowIterator::get_next_row(){
   /*
   Este es el método que ejecuta la máquina de estados finitos.
   Hace avanzar de estado a la máquina hasta que esta retorne
   una fila que no está vacía o se haya llegado al estado final.
   */
   Logger::flush();
   Logger::log(LogLevel::DEBUG, "Another row has been requested");
   std::map<std::string, Values> map_row_returned;

   while(map_row_returned.empty() && this->fsm_state != 255) {
      Logger::log(LogLevel::DEBUG, "Making a Control Unit call");
      map_row_returned = this->control_unit();
   };

   if(this->fsm_state == 255){
      this->eof = true;
      Logger::log(LogLevel::DEBUG, "FSM end condition has been reached");
   };
   return map_row_returned;
};

//====================================================
//== tableRowIterator only WAL (inverse insertion order)
//====================================================


disk_buffer::tableRowIterator_only_ram_for_wal_inverse_order::tableRowIterator_only_ram_for_wal_inverse_order(const std::string& table_name_str, const int num_rows_to_insert){

   /*
   Constructor de la clase 'tableRowIterator_only_ram_for_wal_inverse_order'
   */
   this->n_rows_inserted = num_rows_to_insert;
   //counter = 0;
   table_ptr = global_table_dict.at(table_name_str);
   /*
   No contamos las filas en RAM viva porque solo recuperaremos
   los ultimos datos y si no fallo la inserción está asegurado
   que esas filas están en la RAM viva
   */
   this->n_rows_total = table_ptr->metadata_ptr->n_rows_ram;
   this->counter = this->n_rows_total - 1;
   this->lower_limit = this->n_rows_total - this->n_rows_inserted + 1;

   this->eof = (this->counter < 0);
}; // Termina el metodo 'tableRowIterator_only_ram_for_wal_inverse_order'

// == EOF query ======================================
bool disk_buffer::tableRowIterator_only_ram_for_wal_inverse_order::is_eof() const {
   /*
   Simple función para retornar el atributo booleano 'eof'
   */
   Logger::log(LogLevel::DEBUG, "Writing EOF has been reached for rows in WAL file");
   return eof;
};

// == Para obtener la próxima ultimas filas en RAM viva (RAM viva) ========
std::map<std::string, Values> disk_buffer::tableRowIterator_only_ram_for_wal_inverse_order::get_next_row_ram(){
   /*
   Método parecido a 'get_next_row' de tableRowIterator. Sólo que aquí únicamente
   se retornaran datos de la misma sesión o RAM viva.
   Funciona de la siguiente manera:
      1º Itera por cada columna de la tabla.
      2º Dado el atributo 'counter' extrae dicho elemento de cada vector de columnas de cada columna,
         , siempre de los datos en RAM viva.
      3º Una vez terminado el bucle se actualiza el valor del atributo 'eof'.
      4º Se incrementa en uno el counter de fila.
   */
   std::map<std::string, Values> map_row_returned;

   std::vector<std::string>& col_names = table_ptr->metadata_ptr->column_names;
   // Condiciones de guarda, para ahorrar comprobaciones:
   if(this->counter < this->n_rows_total){
      this->eof = true;
      Logger::log(LogLevel::DEBUG, "Row will return empty ('tableRowIterator_only_ram_for_wal_inverse_order::get_next_row_ram')");
      return map_row_returned;
   };
   table_data*& ptr_data = this->table_ptr->data_ptr;
   if(!ptr_data){
      this->eof = true;
      Logger::log(LogLevel::ERROR, "'data_ptr' pointer does NOT exist within the table object");
      return map_row_returned;
   }
   std::map<std::string, std::vector<Values>>& columns_map = ptr_data->columns;
   if(columns_map.empty()){
      this->eof = true;
      Logger::log(LogLevel::ERROR, "Column map is empty");
      return map_row_returned;
   };

   // Iteramos por cada columna:
   for (const std::string& col_name : col_names) {

      Logger::log(LogLevel::DEBUG, "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
      Logger::log(LogLevel::DEBUG, "Counter: ", false, true);
      Logger::log(LogLevel::DEBUG, this->counter, true, false);
      Logger::log(LogLevel::DEBUG, "n_rows_total: ", false, true);
      Logger::log(LogLevel::DEBUG, this->n_rows_total, true, false);
      Logger::log(LogLevel::DEBUG, "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
      // Leemos desde la RAM viva:
      Logger::log(LogLevel::DEBUG, "Proceeding to fill the WAL row std::map");
      map_row_returned[col_name] = columns_map.at(col_name)[this->counter];
   };
   // Condiciones de contorno generales:
   if(this->counter < 0 || this->counter > this->n_rows_total || this->counter < this->lower_limit){
      this-> eof = true;
   };

   this->counter -= 1;
   return map_row_returned;
}; // Termina el metodo 'get_next_row_ram'

//====================================================
//== tableRowIterator only WAL =======================
//====================================================


disk_buffer::tableRowIterator_only_ram_for_wal::tableRowIterator_only_ram_for_wal(const std::string& table_name_str, const int num_rows_to_insert){

   /*
   Constructor de la clase 'tableRowIterator_only_ram_for_wal'
   */
   this->n_rows_inserted = num_rows_to_insert;
   //counter = 0;
   table_ptr = global_table_dict.at(table_name_str);
   /*
   No contamos las filas en RAM viva porque solo recuperaremos
   los ultimos datos y si no fallo la inserción está asegurado
   que esas filas están en la RAM viva
   */


   this->n_rows_total = table_ptr->metadata_ptr->n_rows_ram;
   this->counter = this->n_rows_total - num_rows_to_insert;

   this->eof = (this->counter >= this->n_rows_total);
}; // Termina el metodo 'tableRowIterator_only_ram_for_wal'

// == EOF query ======================================
bool disk_buffer::tableRowIterator_only_ram_for_wal::is_eof() const {
   /*
   Simple función para retornar el atributo booleano 'eof'
   */
   Logger::log(LogLevel::DEBUG, "Writing EOF has been reached for rows in WAL file");
   return eof;
};

// == Para obtener la próxima ultimas filas en RAM viva (RAM viva) ========
std::map<std::string, Values> disk_buffer::tableRowIterator_only_ram_for_wal::get_next_row_ram(){
   /*
   Método parecido a 'get_next_row' de tableRowIterator. Sólo que aquí únicamente
   se retornaran datos de la misma sesión o RAM viva.
   Funciona de la siguiente manera:
      1º Itera por cada columna de la tabla.
      2º Dado el atributo 'counter' extrae dicho elemento de cada vector de columnas de cada columna,
         , siempre de los datos en RAM viva.
      3º Una vez terminado el bucle se actualiza el valor del atributo 'eof'.
      4º Se incrementa en uno el counter de fila.
   */
   std::map<std::string, Values> map_row_returned;

   std::vector<std::string>& col_names = table_ptr->metadata_ptr->column_names;
   // Condición de guarda, para ahorrar comprobaciones:
   if(this->counter >= this->n_rows_total){
      this->eof = true;
      Logger::log(LogLevel::DEBUG, "Row will return empty ('tableRowIterator_only_ram_for_wal::get_next_row_ram')");
      return map_row_returned;
   };
   table_data*& ptr_data = this->table_ptr->data_ptr;
   if(!ptr_data){
      this->eof = true;
      Logger::log(LogLevel::ERROR, "'data_ptr' pointer does NOT exist within the table object");
      return map_row_returned;
   }
   std::map<std::string, std::vector<Values>>& columns_map = ptr_data->columns;
   if(columns_map.empty()){
      this->eof = true;
      Logger::log(LogLevel::ERROR, "Column map is empty");
      return map_row_returned;
   };

   // Iteramos por cada columna:
   for (const std::string& col_name : col_names) {

      Logger::log(LogLevel::DEBUG, "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
      Logger::log(LogLevel::DEBUG, "Counter: ", false, true);
      Logger::log(LogLevel::DEBUG, this->counter, true, false);
      Logger::log(LogLevel::DEBUG, "n_rows_total: ", false, true);
      Logger::log(LogLevel::DEBUG, this->n_rows_total, true, false);
      Logger::log(LogLevel::DEBUG, "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
      // Leemos desde la RAM viva:
      Logger::log(LogLevel::DEBUG, "Proceeding to fill the WAL row std::map");
      map_row_returned[col_name] = columns_map.at(col_name)[this->counter];
   };
   this->counter += 1;
   // Condiciones de contorno generales:
   if(this->counter < 0 || this->counter >= this->n_rows_total){
      this->eof = true;
   };
   return map_row_returned;
}; // Termina el metodo 'get_next_row_ram'