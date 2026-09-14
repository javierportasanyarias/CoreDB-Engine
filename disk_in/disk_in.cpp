#include "part_sort.h"
#include "disk_aux.h"
#include "disk_in.h"


////////////////////////////////////////////////////////////////////
// LECTURA DE DATOS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void disk_in::read_fixed_len_int_columns(std::filesystem::path path_var, std::string partition_current, std::vector<Values>& vec_vals){

   Logger::log(LogLevel::DEBUG, "Inside INT reading");
   std::ifstream in;
   std::filesystem::path path = path_var;
   path /= partition_current;
   path += ".dat";
   Logger::log(LogLevel::DEBUG, "Opening file data: ", false, true);
   Logger::log(LogLevel::DEBUG, path, true, false);
   in.open(path, std::ios::in | std::ios::binary);
   if (!in.is_open()) {
      Logger::log(LogLevel::ERROR, "Could not open file! Path: ", false, true);
      Logger::log(LogLevel::ERROR, path, true, false);
      return;
   }else{
      Logger::log(LogLevel::DEBUG, "File opened for reading");
   };

   // Hallamos el tamaño en bytes del archivo/partición:
   uint32_t file_size = disk_aux::return_file_size_bytes(in);
   uint32_t bytes_pointer = 0;
   uint32_t bytes_to_read = 0;
   
   // Reserbvamos el buffer:
   char* buffer = nullptr;
   try {
      buffer = new char[size_buffer_bytes];

      Logger::log(LogLevel::DEBUG, "File size retrieved successfully");
      Logger::log(LogLevel::DEBUG, "File size:", false, true);
      Logger::log(LogLevel::DEBUG, file_size, true, false);

      if(file_size > 0){
         Logger::log(LogLevel::DEBUG, "File is NOT empty");
         while(bytes_pointer < file_size){
            Logger::log(LogLevel::DEBUG, "Buffer loop iteration");
            Logger::log(LogLevel::DEBUG, "Calculating bytes to read");
            // Primero calculamos los bytes a leer:
            if(bytes_pointer + size_buffer_bytes >= file_size){
               bytes_to_read = size_buffer_bytes;
            }else{
               bytes_to_read = file_size - bytes_pointer;
            };
            Logger::log(LogLevel::DEBUG, "Reserving those bytes within the buffer");
            // Ya sabemos los bytes a leer, por lo que los reservamos:
            // Realizamos la lectura:
            Logger::log(LogLevel::DEBUG, "Loading data on to buffer");
            in.read(buffer, bytes_to_read);
            Logger::log(LogLevel::DEBUG, "Buffer loades successfully");
            Logger::log(LogLevel::DEBUG, "Proceeding to fill table:");
            // Ahora los insertamos en el vector:
            disk_aux::fill_vector_int(bytes_to_read / sizeof(int), buffer, vec_vals);
            Logger::log(LogLevel::DEBUG, "Values added to the table successfully");
            // Actualizamos el puntero con los bytes leídos:
            bytes_pointer += bytes_to_read;
         };
      }else{
         Logger::log(LogLevel::DEBUG, "File IS empty");
      };
   }catch(...){
      delete[] buffer;
      in.close();
      throw;
   };
   in.close();
   delete[] buffer;
};


void disk_in::read_fixed_len_float_columns(std::filesystem::path path_var, std::string partition_current, std::vector<Values>& vec_vals){
   
   Logger::log(LogLevel::DEBUG, "Inside FLOAT reading");
   std::ifstream in;
   std::filesystem::path path = path_var;
   path /= partition_current;
   path += ".dat";
   Logger::log(LogLevel::DEBUG, "Opening file data: ", false, true);
   Logger::log(LogLevel::DEBUG, path, true, false);
   in.open(path, std::ios::in | std::ios::binary);
   if (!in.is_open()) {
      Logger::log(LogLevel::ERROR, "Could not open file! Path: ", false, true);
      Logger::log(LogLevel::ERROR, path, true, false);
      return;
   }else{
      Logger::log(LogLevel::DEBUG, "File opened for reading");
   };

   // Hallamos el tamaño en bytes del archivo/partición:
   uint32_t file_size = disk_aux::return_file_size_bytes(in);
   uint32_t bytes_pointer = 0;
   uint32_t bytes_to_read = 0;
   
   // Reserbvamos el buffer:
   char* buffer = nullptr;
   try {
      buffer = new char[size_buffer_bytes];
      Logger::log(LogLevel::DEBUG, "File size retrieved successfully");
      Logger::log(LogLevel::DEBUG, "File size:", false, true);
      Logger::log(LogLevel::DEBUG, file_size, true, false);

      if(file_size > 0){
         Logger::log(LogLevel::DEBUG, "File is NOT empty");
         while(bytes_pointer < file_size){
            Logger::log(LogLevel::DEBUG, "Buffer loop iteration");
            Logger::log(LogLevel::DEBUG, "Calculating bytes to read");
            // Primero calculamos los bytes a leer:
            if(bytes_pointer + size_buffer_bytes >= file_size){
               bytes_to_read = size_buffer_bytes;
            }else{
               bytes_to_read = file_size - bytes_pointer;
            };
            Logger::log(LogLevel::DEBUG, "Reserving those bytes within the buffer");
            // Ya sabemos los bytes a leer, por lo que los reservamos:
            // Realizamos la lectura:
            Logger::log(LogLevel::DEBUG, "Loading data into buffer");
            in.read(buffer, bytes_to_read);
            Logger::log(LogLevel::DEBUG, "Buffer loades successfully");
            Logger::log(LogLevel::DEBUG, "Proceeding to fill table:");
            // Ahora los insertamos en el vector:
            disk_aux::fill_vector_float(bytes_to_read / sizeof(float), buffer, vec_vals);
            Logger::log(LogLevel::DEBUG, "Values added to the table successfully");
            // Actualizamos el puntero con los bytes leídos:
            bytes_pointer += bytes_to_read;
         };
      }else{
         Logger::log(LogLevel::DEBUG, "File IS empty");
      };
   }catch(...){
      delete[] buffer;
      in.close();
      throw;
   };
   in.close();
   delete[] buffer;
};


void disk_in::read_fixed_len_bool_columns(std::filesystem::path path_var, std::string partition_current, std::vector<Values>& vec_vals){
   
   Logger::log(LogLevel::DEBUG, "Inside BOOL reading");
   std::ifstream in;
   std::filesystem::path path = path_var;
   path /= partition_current;
   path += ".dat";
   Logger::log(LogLevel::DEBUG, "Opening file data: ", false, true);
   Logger::log(LogLevel::DEBUG, path, true, false);
   in.open(path, std::ios::in | std::ios::binary);
   if (!in.is_open()) {
      Logger::log(LogLevel::ERROR, "Could not open file! Path: ", false, true);
      Logger::log(LogLevel::ERROR, path, true, false);
      return;
   }else{
      Logger::log(LogLevel::DEBUG, "File opened for reading");
   };

   // Hallamos el tamaño en bytes del archivo/partición:
   uint32_t file_size = disk_aux::return_file_size_bytes(in);
   uint32_t bytes_pointer = 0;
   uint32_t bytes_to_read = 0;
   
   // Reserbvamos el buffer:
   char* buffer = nullptr;
   try {
      buffer = new char[size_buffer_bytes];
      Logger::log(LogLevel::DEBUG, "File size retrieved successfully");
      Logger::log(LogLevel::DEBUG, "File size:", false, true);
      Logger::log(LogLevel::DEBUG, file_size, true, false);

      if(file_size > 0){
         Logger::log(LogLevel::DEBUG, "File is NOT empty");
         while(bytes_pointer < file_size){
            Logger::log(LogLevel::DEBUG, "Buffer loop iteration");
            Logger::log(LogLevel::DEBUG, "Calculating bytes to read");
            // Primero calculamos los bytes a leer:
            if(bytes_pointer + size_buffer_bytes >= file_size){
               bytes_to_read = size_buffer_bytes;
            }else{
               bytes_to_read = file_size - bytes_pointer;
            };
            Logger::log(LogLevel::DEBUG, "Reserving those bytes within the buffer");
            // Ya sabemos los bytes a leer, por lo que los reservamos:
            // Realizamos la lectura:
            Logger::log(LogLevel::DEBUG, "Loading data into buffer");
            in.read(buffer, bytes_to_read);
            Logger::log(LogLevel::DEBUG, "Buffer loades successfully");
            Logger::log(LogLevel::DEBUG, "Proceeding to fill table:");
            // Ahora los insertamos en el vector:
            disk_aux::fill_vector_bool(bytes_to_read / sizeof(uint8_t), buffer, vec_vals);
            Logger::log(LogLevel::DEBUG, "Values added to the table successfully");
            // Actualizamos el puntero con los bytes leídos:
            bytes_pointer += bytes_to_read;
         };
      }else{
         Logger::log(LogLevel::DEBUG, "File IS empty");
      };
   }catch(...){
      delete[] buffer;
      in.close();
      throw;
   };
   in.close();
   delete[] buffer;
};





///////////////////////////////////////////////////////////////////////////////////////////
// Para la lectura de las strings se realiza con un FSM











////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

class stringReader_v2{
   /*
   Clase que nos abstraerá por completo de la lectura de strings.
   */
   std::vector<Values>& vec_vals;
   public:
      // Variable auxiliar para ver si es una std::string o un vector de caracteres:
      bool is_unknown_type = false;
      // Variables como input y de control:
      std::filesystem::path path_var;
      std::string partition_current;
      uint8_t state = 0;

      // Valores predefindos:

      // Contadores de archivo:
      uint32_t file_idx_ptr = 0;
      uint32_t file_buffer_idx_size = 0;
      uint32_t file_idx_size = 0;

      uint32_t file_str_ptr = 0;
      uint32_t file_buffer_str_size = 0;
      uint32_t file_str_size = 0;

      // Contadores de buffer:
      char* buffer_idx_ptr_ini;
      char* buffer_idx_ptr_fin;
      uint32_t buffer_idx_bytes_to_read_each_time = 4; // Siempre se leera de 4 en 4 bytes
      uint32_t buffer_idx_read_bytes_tmp = 0;
      uint32_t buffer_idx_read_bytes_total = 0;

      char* buffer_str_ptr_ini;
      char* buffer_str_ptr_fin;
      uint32_t buffer_str_read_bytes_tmp = 0; // Esta valor dependera del tamano de la string o lo que quede por leer del buffer. Lo que sea más pequeño de las dos
      uint32_t buffer_str_read_bytes_total = 0;

      // Contadores de la lectura de strings:
      uint32_t size_string_tmp = 0;
      bool offset = false;


      // Lectura:
      std::ifstream in_str;
      std::ifstream in_idx;

      char* buffer_str = nullptr;
      char* buffer_idx = nullptr;


      uint32_t bytes_to_read = 0;

;

      stringReader_v2(std::vector<Values>& vector_variable_variantes, std::filesystem::path path_variable, std::string particion_actual, bool is_var_unknown) 
        : vec_vals(vector_variable_variantes), path_var(path_variable),  partition_current(particion_actual), is_unknown_type(is_var_unknown)// Inicialización de la referencia y strings para las rutas
      {
         this->buffer_str = new char[size_buffer_bytes];
         this->buffer_idx = new char[size_buffer_bytes];
      };

      ~stringReader_v2() {
         if(this->in_str.is_open()){
            this->in_str.close();
         };
         if(this->in_idx.is_open()){
            this->in_idx.close();
         };

         if(this->buffer_str){
               delete[] this->buffer_str;
               this->buffer_str = nullptr;
         };
            if(this->buffer_idx){
               delete[] this->buffer_idx;
               this->buffer_idx = nullptr;
         };
      };

      bool condition_eof_str_partition(){
         return this->file_str_ptr >= this->file_str_size;
      };

      bool condition_eof_idx_partition(){
         return this->file_idx_ptr >= this->file_idx_size;
      };

      bool condition_eof_buffer_str(){
         return this->buffer_str_read_bytes_tmp >= this->file_buffer_str_size;

      };

      bool condition_eof_buffer_idx(){
         return this->buffer_idx_read_bytes_tmp >= this->file_buffer_idx_size;
      };

      // Funciones auxiliares de calculo del tamño del buffer:
      void calculate_buffer_size_str(){
         this->file_buffer_str_size = size_buffer_bytes;
         if(this->file_str_ptr + size_buffer_bytes > this->file_str_size){
            this->file_buffer_str_size = this->file_str_size - this->file_str_ptr;
         };
      };

      void calculate_buffer_size_idx(){
         this->file_buffer_idx_size = size_buffer_bytes;
         if(this->file_idx_ptr + size_buffer_bytes > this->file_idx_size){
            this->file_buffer_idx_size = this->file_idx_size - this->file_idx_ptr;
         };
      };

      // Funciones axuliares de lectra de datos del disco al buffer:
      void read_str_buffer(){
         // Limpiamos la memoria:

         // Calculamos el tamano del buffer:
         this->calculate_buffer_size_str();

         // Leemos las strings:
         this->in_str.read(this->buffer_str, this->file_buffer_str_size);

         // Actualizamos el puntero de lectura:
         this->file_str_ptr += this->file_buffer_str_size;

         // Reinicamos contadores:
         this->buffer_str_read_bytes_tmp = 0;
   
         // Fijamos los punteros:
         this->buffer_str_ptr_ini = this->buffer_str;
         this->buffer_str_ptr_fin = buffer_str_ptr_ini + this->file_buffer_str_size;
      };

      void read_idx_buffer(){

         // Calculamos el tamano del buffer:
         this->calculate_buffer_size_idx();

         // Leemos las strings:
         this->in_idx.read(this->buffer_idx, this->file_buffer_idx_size);

         // Actualizamos el puntero de lectura:
         this->file_idx_ptr += this->file_buffer_idx_size;

         // Reiniciamos contadores:
         this->buffer_idx_read_bytes_tmp = 0;

         // Fijamos los punteros:
         this->buffer_idx_ptr_ini = this->buffer_idx;
         this->buffer_idx_ptr_fin = buffer_idx_ptr_ini + this->file_buffer_idx_size;
      };


      void control_unit(){
         /*
         Unidad de control para la lectura y
         cargado de los datos en la tabla
         */

         switch(this->state){
            case 0:{
               Logger::log(LogLevel::DEBUG, "State 0 stringReader_v2");
               std::filesystem::path path_str = this->path_var;
               path_str /= this->partition_current;
               path_str += ".bin";
               std::filesystem::path path_idx = this->path_var;
               path_idx /= this->partition_current;
               path_idx += ".idx";


               Logger::log(LogLevel::DEBUG, "Data path: ", false, true);
               Logger::log(LogLevel::DEBUG, path_str, true, false);
               Logger::log(LogLevel::DEBUG, "Index path: ", false, true);
               Logger::log(LogLevel::DEBUG, path_idx, true, false);

               this->in_str.open(path_str, std::ios::in | std::ios::binary);
               this->in_idx.open(path_idx, std::ios::in | std::ios::binary);

               Logger::log(LogLevel::DEBUG, "Files read successfully");

               // Recuperamos el tamano de los archivos:
               this->file_str_size = disk_aux::return_file_size_bytes(in_str);
               this->file_idx_size = disk_aux::return_file_size_bytes(in_idx);
               Logger::log(LogLevel::DEBUG, "  Strings file's size: ", false, true);
               Logger::log(LogLevel::DEBUG, this->file_str_size, true, false);
               Logger::log(LogLevel::DEBUG, "  Indices file's size: ", false, true);
               Logger::log(LogLevel::DEBUG, this->file_idx_size, true, false);

               if(this->file_str_size > 0 && this->file_idx_size > 0){
                  this->state = 1;
                  break;
               };
               this->state = 255;
               break;
            };

            case 1:{
               Logger::log(LogLevel::DEBUG, "State 1 stringReader_v2");
               // Leemos el buffer de strings:
               this->read_str_buffer();
               // Leemos el buffer de idx:
               this->read_idx_buffer();
               this->state = 4; // Saltamos a ver el tamano de la string
               break;
            };

            case 2:{
               Logger::log(LogLevel::DEBUG, "State 2 stringReader_v2");
               bool eof_buffer_str = false;
               bool eof_buffer_idx = false;
               bool eof_file_str = false;
               bool eof_file_idx = false;
               Logger::log(LogLevel::DEBUG, "<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
               Logger::log(LogLevel::DEBUG, "STOP DIAGNOSIS:");

               Logger::flush(LogLevel::DEBUG);
               Logger::log(LogLevel::DEBUG, "-> condition_eof_str_partition:");
               Logger::log(LogLevel::DEBUG, "  data pointer value: ", false, true);
               Logger::log(LogLevel::DEBUG, this->file_str_ptr, true, false);
               Logger::log(LogLevel::DEBUG, "  Data file size: ", false, true);
               Logger::log(LogLevel::DEBUG, this->file_str_size, true, false);
               Logger::flush(LogLevel::DEBUG);
               Logger::log(LogLevel::DEBUG, "-> condition_eof_idx_partition:");
               Logger::log(LogLevel::DEBUG, "  indices pointer value: ", false, true);
               Logger::log(LogLevel::DEBUG, this->file_idx_ptr, true, false);
               Logger::log(LogLevel::DEBUG, "  Indices file size: ", false, true);
               Logger::log(LogLevel::DEBUG, this->file_idx_size, true, false);
               Logger::flush(LogLevel::DEBUG);
               Logger::log(LogLevel::DEBUG, "-> condition_eof_buffer_idx:");
               Logger::log(LogLevel::DEBUG, "  Bytes read temporay: ", false, true);
               Logger::log(LogLevel::DEBUG, this->buffer_idx_read_bytes_tmp, true, false);
               Logger::log(LogLevel::DEBUG, "  Buffer size: ", false, true);
               Logger::log(LogLevel::DEBUG, this->file_buffer_idx_size, true, false);
               Logger::flush(LogLevel::DEBUG);
               Logger::log(LogLevel::DEBUG, "-> condition_eof_buffer_str:");
               Logger::log(LogLevel::DEBUG, "  Bytes read temporay: ", false, true);
               Logger::log(LogLevel::DEBUG, this->buffer_str_read_bytes_tmp, true, false);
               Logger::log(LogLevel::DEBUG, "  Buffer size: ", false, true);
               Logger::log(LogLevel::DEBUG, this->file_buffer_str_size, true, false);
               Logger::flush(LogLevel::DEBUG);
               Logger::log(LogLevel::DEBUG, "<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");

               // Ahora realizamos la lectura de los indices:
               if(this->condition_eof_idx_partition()){
                  eof_file_idx = true;
                  if(this->condition_eof_buffer_idx()){
                     eof_buffer_idx = true;
                  };
               }else{
                  // Solo si  hemos agotado el buffer leemos:
                  if(this->condition_eof_buffer_idx()){
                     eof_buffer_idx = true;
                     this->read_idx_buffer();
                  };
               };

               // Ahora realizamos la lectura de las cadenas de texto
               if(this->condition_eof_str_partition()){
                  eof_file_str = true;
                  if(this->condition_eof_buffer_str()){
                     eof_buffer_str = true;
                  };
               }else{
                  // Solo si  hemos agotado el buffer leemos:
                  if(this->condition_eof_buffer_str()){
                     eof_buffer_str = true;
                     this->read_str_buffer();
                  };
               };

               // Ahora en función de los condicionales decidimos a dónde ir:
               if(eof_file_idx && eof_buffer_idx && eof_file_str && eof_buffer_str){
                  this->state = 255;
                  break;
               };
               // Si no hemos llegado al final, podemos seguir leyendo:
               this->state = 3; // Offset checker
               break;
            };
            case 3: {
               Logger::log(LogLevel::DEBUG, "State 3 stringReader_v2");
               if(this->offset){
                  // La string la arrastramos de antes
                  this->state = 5;
                  break;
               }else{
                  // La string es nueva
                  this->state = 4;
                  break;
               };
            };
            case 4:{
               Logger::log(LogLevel::DEBUG, "Sate 4 stringReader_v2");

               // Leemos el tamano de la string:
               std::memcpy(&this->size_string_tmp,
                  this->buffer_idx_ptr_ini + this->buffer_idx_read_bytes_tmp,
                  this->buffer_idx_bytes_to_read_each_time
               );
               // Avanzamos el puntero al la siguiente fila:
               this->buffer_idx_read_bytes_total += this->buffer_idx_bytes_to_read_each_time;
               this->buffer_idx_read_bytes_tmp += this->buffer_idx_bytes_to_read_each_time;
               this->state = 5; // Saltamos a leer la string
               break;
            };

            case 5:{
               Logger::log(LogLevel::DEBUG, "State 5 stringReader_v2");

               uint32_t bytes_ram_available = this->file_buffer_str_size - this->buffer_str_read_bytes_tmp;
               this->bytes_to_read = this->size_string_tmp;
               if(this->bytes_to_read > bytes_ram_available){
                  this->bytes_to_read = bytes_ram_available;
               };

               const char* origin_bytes = this->buffer_str_ptr_ini + this->buffer_str_read_bytes_tmp;
               if(offset){
                  if(is_unknown_type){
                     std::vector<char>& tmp_vec = std::get<std::vector<char>>(vec_vals.back());
                     tmp_vec.insert(tmp_vec.end(),
                     origin_bytes,
                     origin_bytes + this->bytes_to_read);
                  }else{
                     // No es una string nueva
                     std::string& tmp_str = std::get<std::string>(vec_vals.back());
                     tmp_str.append(
                        origin_bytes,
                        this->bytes_to_read
                     );
                  };
               }else{
                  if(is_unknown_type){
                     std::vector<char> tmp_vec(origin_bytes, origin_bytes + this->bytes_to_read);
                     vec_vals.push_back(std::move(tmp_vec));
                  }else{
                     // String nueva
                     std::string tmp_str;
                     tmp_str.append(
                        origin_bytes,
                        this->bytes_to_read
                     );
                     vec_vals.push_back(std::move(tmp_str));
                  };
               };

               // Actualizamos los bytes leidos:
               this->buffer_str_read_bytes_tmp += this->bytes_to_read; 
 
               this->buffer_str_read_bytes_total += this->bytes_to_read;
  
               // Actualizamos el tamano de la string que queda por leer:
               this->size_string_tmp -= this->bytes_to_read;
 
               // Ahroa vemos si ay offset o no:
               if(this->size_string_tmp > 0){
                  // No se ha leido entera:
                  this->offset = true;
               }else{
                  // Se ha leido entera:
                  this->offset = false;
               };  
               
               // Siempre vamos a la lectura inteligente:
               this->state = 2;
               break;
            };
            case 255: {
               Logger::log(LogLevel::DEBUG, "State 255 stringReader_v2");
               break;
            };
         ///////////////////////////////////////////////////////////////
         ///////////////////////////////////////////////////////////////
         ///////////////////////////////////////////////////////////////



         }; // Switch for control unit ends
      }; // Termina la CU de la FSM

      void execute_fsm(){
         Logger::log(LogLevel::DEBUG, "Inside STRING reading");
         bool aux_bool = true;
         while(this->state != 255 && aux_bool){
            if(this->state == 255){
               aux_bool = false;
            };
            this->control_unit();
         };
      };
};






disk_in::read_table_iterator::read_table_iterator(table* table_ptr_input){
   /*
   Método constructor en el cual:
   Leemos y registramos lo necesario para realizar las sucesivas lecturas
   */
   Logger::log(LogLevel::DEBUG, "Initializing data reading");

   this->table_ptr = table_ptr_input;
   // Recuperamos el nombre:
   this->table_name = this->table_ptr->metadata_ptr->name;
   // El nombre de las columnas:
   this->column_names = this->table_ptr->metadata_ptr->column_names;
   this->num_cols = this->table_ptr->metadata_ptr->n_cols;
   // Recuperamos el tipo de las columnas:
   this->column_types = this->table_ptr->metadata_ptr->column_types;
   // Inicializamos la variable por la cual añadiremos los datos venidos del disco:
   if(this->table_ptr->data_buffer_ptr == nullptr){
      this->table_ptr->data_buffer_ptr = new table_data_buffer();
   }
   // Ahora apuntamos nuestro puntero interno al que ya tiene (o acaba de recibir) la tabla
   this->disk_data_ptr = this->table_ptr->data_buffer_ptr;
   Logger::log(LogLevel::DEBUG, "Retrieving table_ptr values and initializing it's attribute for data originated from the disk");

   // Ahora vemos que particiones hay:
   this->partition_counter = 0;
   this->total_partitions = 0;
   Logger::log(LogLevel::DEBUG, "Obtaining partitions present within the data path:");
   Logger::log(LogLevel::DEBUG, "data/" + this->table_name + "/" + this->column_names[0]);
   // Ahora vemos si la columa primera o con indice 0 es de tamaño fijo o variable:

   this->partition_names = disk_aux::obtain_files_in_path("data/" + this->table_name + "/" + this->column_names[0], this->column_types[0]);
   Logger::log(LogLevel::DEBUG, "Partition names retrieved successfully");

   if(!(this->partition_names.empty())){
      Logger::log(LogLevel::DEBUG, "There is data within the disk");
      Logger::log(LogLevel::DEBUG, "Proceeding to parttion sorting in ascending order:");
      part_sort::quick_sort(this->partition_names);
      Logger::log(LogLevel::DEBUG, "Partition sorting carried successfully");
      Logger::log(LogLevel::DEBUG, "Quering number of total partitions:");
      this->total_partitions = this->partition_names.size();
      Logger::log(LogLevel::DEBUG, "Number of partitions found: ", false, true);
      Logger::log(LogLevel::DEBUG, this->total_partitions, true, false);
   };


   ////////////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////////////
   Logger::log(LogLevel::DEBUG, "Everything ready. Proceeding to enter column loop:");

};

bool disk_in::read_table_iterator::obtain_int_partition_rows(bool aux_bool, std::filesystem::path path_var, std::string partition_current){
   
   if(aux_bool){
      Logger::log(LogLevel::DEBUG, "<<<<< PARTITION SIZE ASSERTION (INT CASE) >>>>>>>");
      uint32_t size_tmp = 0;
      Logger::log(LogLevel::DEBUG, "First execution. Quering partition size: ");
      std::filesystem::path tmp_path = path_var;
      tmp_path /= (partition_current + ".dat");
      Logger::log(LogLevel::DEBUG, tmp_path);
      size_tmp = disk_aux::obtain_file_size(tmp_path);
      Logger::log(LogLevel::DEBUG, "Partition file size: ", false, true);
      Logger::log(LogLevel::DEBUG, size_tmp, true, false);
      Logger::log(LogLevel::DEBUG, "Registering partition file size");
      this->current_partition_n_rows =  size_tmp / sizeof(int);
      Logger::log(LogLevel::DEBUG, "Current partition rows registered successfully: ", false, true);
      Logger::log(LogLevel::DEBUG, this->current_partition_n_rows, true, false);
      aux_bool = false;
   };
   return aux_bool;
};


bool disk_in::read_table_iterator::obtain_float_partition_rows(bool aux_bool, std::filesystem::path path_var, std::string partition_current){
   
   if(aux_bool){
      Logger::log(LogLevel::DEBUG, "<<<<< PARTITION SIZE ASSERTION (FLOAT CASE) >>>>>>>");
      uint32_t size_tmp = 0;
      Logger::log(LogLevel::DEBUG, "First execution. Quering partition size: ");
      std::filesystem::path tmp_path = path_var;
      tmp_path /= (partition_current + ".dat");
      Logger::log(LogLevel::DEBUG, tmp_path);
      size_tmp = disk_aux::obtain_file_size(tmp_path);
      Logger::log(LogLevel::DEBUG, "Partition file size: ", false, true);
      Logger::log(LogLevel::DEBUG, size_tmp, true, false);
      Logger::log(LogLevel::DEBUG, "Registering partition file size");
      this->current_partition_n_rows =  size_tmp / sizeof(float);
      Logger::log(LogLevel::DEBUG, "Current partition rows registered successfully: ", false, true);
      Logger::log(LogLevel::DEBUG, this->current_partition_n_rows, true, false);
      aux_bool = false;
   };
   return aux_bool;
};


bool disk_in::read_table_iterator::obtain_bool_partition_rows(bool aux_bool, std::filesystem::path path_var, std::string partition_current){
   
   if(aux_bool){
      uint32_t size_tmp = 0;
      Logger::log(LogLevel::DEBUG, "<<<<< PARTITION SIZE ASSERTION (BOOL CASE) >>>>>>>");
      Logger::log(LogLevel::DEBUG, "First execution. Quering partition size: ");
      std::filesystem::path tmp_path = path_var;
      tmp_path /= (partition_current + ".dat");
      Logger::log(LogLevel::DEBUG, tmp_path);
      size_tmp = disk_aux::obtain_file_size(tmp_path);
      Logger::log(LogLevel::DEBUG, "Partition file size: ", false, true);
      Logger::log(LogLevel::DEBUG, size_tmp, true, false);
      Logger::log(LogLevel::DEBUG, "Registering partition file size");
      this->current_partition_n_rows =  size_tmp;
      Logger::log(LogLevel::DEBUG, "Current partition rows registered successfully: ", false, true);
      Logger::log(LogLevel::DEBUG, this->current_partition_n_rows, true, false);
      aux_bool = false;
   };
   return aux_bool;
};


bool disk_in::read_table_iterator::obtain_string_partition_rows(bool aux_bool, std::filesystem::path path_var, std::string partition_current){
   
   if(aux_bool){
      uint32_t size_tmp = 0;
      Logger::log(LogLevel::DEBUG, "<<<<< PARTITION SIZE ASSERTION (STRING CASE) >>>>>>>");
      Logger::log(LogLevel::DEBUG, "First execution. Quering partition size: ");
      std::filesystem::path tmp_path = path_var;
      tmp_path /= (partition_current + ".idx");
      Logger::log(LogLevel::DEBUG, tmp_path);
      size_tmp = disk_aux::obtain_file_size(tmp_path);
      Logger::log(LogLevel::DEBUG, "Partition file size: ", false, true);
      Logger::log(LogLevel::DEBUG, size_tmp, true, false);
      Logger::log(LogLevel::DEBUG, "Registering partition file size");
      this->current_partition_n_rows =  size_tmp / sizeof(uint32_t);
      Logger::log(LogLevel::DEBUG, "Current partition rows registered successfully: ", false, true);
      Logger::log(LogLevel::DEBUG, this->current_partition_n_rows, true, false);
      aux_bool = false;
   };
   return aux_bool;
};

bool disk_in::read_table_iterator::read_table(){

   Logger::flush(LogLevel::DEBUG);
   Logger::log(LogLevel::DEBUG, "inside 'read_table'");

   if(this->partition_counter < this->total_partitions){
      Logger::log(LogLevel::DEBUG, "Read partitions: ", false, true);
      Logger::log(LogLevel::DEBUG, this->partition_counter, true, false);
      Logger::log(LogLevel::DEBUG, "Total partitions: ", false, true);
      Logger::log(LogLevel::DEBUG, this->total_partitions, true, false);
      // En este caso preparamos dónde se almacenarán los datos:
      Logger::log(LogLevel::DEBUG, "Accessing disk's std::map:");
      std::map<std::string, std::vector<Values>>& buffer_ram_disk = this->disk_data_ptr->columns;
      Logger::log(LogLevel::DEBUG, "disk's std::map retrieved successfully");
      Logger::log(LogLevel::DEBUG, "Proceeding to clean disk's std::map:");
      buffer_ram_disk.clear();
      Logger::log(LogLevel::DEBUG, "disk's std::map cleaned successfully");
      bool aux_bool = true;


      // Se puede hacer fuera del bucle:
      std::string partition_current = this->partition_names[this->partition_counter];
      // Le quitamos la extension al nombre:
      partition_current = part_sort::erase_file_extension(partition_current);
      Logger::log(LogLevel::DEBUG, partition_current, true, false);
   
      for(int i = 0; i< this->num_cols; i++){

         dataType data_type = column_types[i];
         std::string column_name = this->column_names[i];
         Logger::log(LogLevel::DEBUG, "Processing the column: ", false, true);
         Logger::log(LogLevel::DEBUG, column_name, true, false);
         Logger::log(LogLevel::DEBUG, "Obtaining path: ", false, true);
         std::filesystem::path path_var = std::filesystem::path("data") / this->table_name / column_name;
         Logger::log(LogLevel::DEBUG, path_var, true, false);
         Logger::log(LogLevel::DEBUG, "Accessing current partition: ", false, true);
         std::vector<Values> vec_vals;

         switch(data_type){
            case dataType::INT: {
               Logger::log(LogLevel::DEBUG, "INT Case");
               Logger::log(LogLevel::DEBUG, "++++++ SWITCH INT CASE ++++++");
               aux_bool = this->obtain_int_partition_rows(aux_bool, path_var, partition_current);
               Logger::log(LogLevel::DEBUG, "Proceeding current INT partition:");
               disk_in::read_fixed_len_int_columns(path_var, partition_current, vec_vals);
               // Adicionamos estos datos a la tabla, en la region de datos del disco:
               Logger::log(LogLevel::DEBUG, "Proceeding to add INT vector to the table");
               buffer_ram_disk[column_name] = std::move(vec_vals);
               Logger::log(LogLevel::DEBUG, "Vector added to the table successfully");
               break;
            };
            case dataType::FLOAT: {
               Logger::log(LogLevel::DEBUG, "++++++ SWITCH FLOAT CASE ++++++");
               aux_bool = this->obtain_float_partition_rows(aux_bool, path_var, partition_current);
               Logger::log(LogLevel::DEBUG, "FLOAT case");
               Logger::log(LogLevel::DEBUG, "Proceeding to add FLOAT vector to the table");
               disk_in::read_fixed_len_float_columns(path_var, partition_current, vec_vals);
               // Adicionamos estos datos a la tabla, en la region de datos del disco:
               buffer_ram_disk[column_name] = std::move(vec_vals);
               Logger::log(LogLevel::DEBUG, "Vector added to the table successfully");
               break;
            };
            case dataType::BOOL: {
               Logger::log(LogLevel::DEBUG, "++++++ SWITCH BOOL CASE ++++++");
               aux_bool = this->obtain_bool_partition_rows(aux_bool, path_var, partition_current);

               Logger::log(LogLevel::DEBUG, "Proceeding to add BOOL vector to the table");
               disk_in::read_fixed_len_bool_columns(path_var, partition_current, vec_vals);
               buffer_ram_disk[column_name] = std::move(vec_vals);
               Logger::log(LogLevel::DEBUG, "Vector added to the table successfully");
               break;
            };
            case dataType::STRING: {
               Logger::log(LogLevel::DEBUG, "++++++ SWITCH STRING CASE ++++++");
               aux_bool = this->obtain_string_partition_rows(aux_bool, path_var, partition_current);
               Logger::log(LogLevel::DEBUG, "Proceeding to add STRING vector to the table");
               // Creamos el objeto:
               Logger::flush(LogLevel::DEBUG);
               Logger::flush(LogLevel::DEBUG);
               Logger::flush(LogLevel::DEBUG);
               Logger::log(LogLevel::DEBUG, "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||");
               stringReader_v2 string_reader = stringReader_v2(vec_vals, path_var, partition_current, false);
               // Realizamos la lectura de la partición:
               string_reader.execute_fsm();

               Logger::flush(LogLevel::DEBUG);
               Logger::flush(LogLevel::DEBUG);
               Logger::flush(LogLevel::DEBUG);
               Logger::log(LogLevel::DEBUG, "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||");

               buffer_ram_disk[column_name] = std::move(vec_vals);
               Logger::log(LogLevel::DEBUG, "Vector added to the table successfully");
               break;
            };
            case dataType::UNKNOWN: {
               Logger::log(LogLevel::DEBUG, "++++++ SWITCH UNKNOWN CASE ++++++");
               aux_bool = this->obtain_string_partition_rows(aux_bool, path_var, partition_current);
               Logger::log(LogLevel::DEBUG, "Proceeding to add UNKNOWN vector to the table");

               // Creamos el objeto:
               Logger::flush(LogLevel::DEBUG);
               Logger::flush(LogLevel::DEBUG);
               Logger::flush(LogLevel::DEBUG);
               Logger::log(LogLevel::DEBUG, "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||");
               stringReader_v2 string_reader = stringReader_v2(vec_vals, path_var, partition_current, true);
               // Realizamos la lectura de la partición:
               string_reader.execute_fsm();

               Logger::flush(LogLevel::DEBUG);
               Logger::flush(LogLevel::DEBUG);
               Logger::flush(LogLevel::DEBUG);
               Logger::log(LogLevel::DEBUG, "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||");

               buffer_ram_disk[column_name] = std::move(vec_vals);
               Logger::log(LogLevel::DEBUG, "Vector added to the table successfully");
               break;
            };
         };
      };
      Logger::log(LogLevel::DEBUG, "Adding one parttition !!!!!!!");
      this->partition_counter += 1;
      return false;
   };
   Logger::log(LogLevel::DEBUG, "There are no partitions left to read");
   // En el caso de ya haberse quedado sin particiones que leer, limpiamos la region de la tabla para datos leidos del disco:
   std::map<std::string, std::vector<Values>>& buffer_ram_disk = this->disk_data_ptr->columns;
   buffer_ram_disk.clear();
   return true; // En caso de no haber leído naada, porque ya no hay datos
   // Ya habríamos aacabado la lectura, avanzamos en uno la partición:
};
