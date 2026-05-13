#include "globals.h"
#include "disk_in.h"
#include "logging.h"
#include "disk_buffer.h"


// ==========================================
// == FUNCIONES AUXILIARES: CONTEO FILAS ====
// ==========================================


// == Para contar filas de una tabla =================
void disk_buffer::contar_datos_ram_una_tabla(std::string& nombre_tabla) {
   /*
   Método auxiliar para actualizar el contador de filas en RAM viva
   o de datos añadidos en la misma sesión. 
   No retorna nada, actualiza el contador de filas en RAM en los metadatos de una tabla
   dado como input su nombre
   */
   table* table_ptr = global_table_dict.at(nombre_tabla);
   
   // 1. Verificamos que data_ptr exista y que el mapa de columnas no esté vacío
   if (table_ptr->data_ptr && !table_ptr->data_ptr->columns.empty()) {
      
         // 2. Tomamos la PRIMERA columna que encontremos
         auto it = table_ptr->data_ptr->columns.begin();
      
         // 3. El número de FILAS es el tamaño del vector de esa columna
         uint32_t n_rows = static_cast<uint32_t>(it->second.size());
      
      table_ptr->metadata_ptr->n_filas_ram = n_rows;
      
   } else {
         table_ptr->metadata_ptr->n_filas_ram = 0;
   };

};


// == Para contar todas filas ========================
void disk_buffer::contar_datos_en_ram_todas_tablas(){
   /*
   Función que actualiza el contador de filas en RAM viva de
   todas las tablas cargadas en memoria.
   */
   // iteramos por todas las tablas:
   for (const auto& [table_name, table_ptr] : global_table_dict){
      // Contamos el numero de filas en RAM viva:
      uint32_t n_rows = static_cast<uint32_t>(table_ptr->data_ptr->columns.size());
   table_ptr->metadata_ptr->n_filas_ram = n_rows;
   };

};

//====================================================
//========= tableRowIterator solo disco ==============
//====================================================


// == Método constructor =============================
disk_buffer::tableRowIterator_only_disk_part::tableRowIterator_only_disk_part(std::string tabla_nombre){

   contador = 0;
   tabla_ptr = global_table_dict.at(tabla_nombre);
   eof = false;


   
};


// == Para consultar eof =============================
bool disk_buffer::tableRowIterator_only_disk_part::is_eof() {

   return eof;
};


// == Para obtener la próxima fila ===================
std::map<std::string, Values> disk_buffer::tableRowIterator_only_disk_part::get_next_row_only_disk_part(disk_in::read_table_iterator* table_reader_obj){

   // Realizamos la lectura:
   Logger::flush();
   Logger::log(LogLevel::DEBUG, "<<<<<<<< DENTRO DEL ITERADOR EN DISCO >>>>>>>>>>", true, false);
   Logger::flush();
   std::map<std::string, Values> map_fila_retornar;
   uint32_t n_f_disk = table_reader_obj->numero_de_filas_current_partition;
   Logger::log(LogLevel::DEBUG, "Leeremos ", false, true);
   Logger::log(LogLevel::DEBUG, n_f_disk, false, false);
   Logger::log(LogLevel::DEBUG, " filas en total", true, false);
   Logger::log(LogLevel::DEBUG, "El valor del contador es de: ", false, true);
   Logger::log(LogLevel::DEBUG, contador, true, false);

   // Iteramos por cada columna:

   for (const std::string& nombre_col : tabla_ptr->metadata_ptr->column_names) {
      if(contador < n_f_disk){
         Logger::log(LogLevel::DEBUG, "Leemos desde disco");
         Logger::log(LogLevel::DEBUG, "Leemos el contador: ", false, true);
         Logger::log(LogLevel::DEBUG, contador, true, false);
         Logger::log(LogLevel::DEBUG, "Leemos el nombre de la columna: ", false, true);
         Logger::log(LogLevel::DEBUG, nombre_col, true, false);
         if(this->tabla_ptr->data_buffer_ptr){
            Logger::log(LogLevel::ERROR, "SI existe el puntero 'data_buffer_ptr' en la tabla");
            if(this->tabla_ptr->data_buffer_ptr->columns.empty()){
               Logger::log(LogLevel::ERROR, "El mapa de columnas está vacío");
            }else{
               Logger::log(LogLevel::ERROR, "Escribimos el valor en la <<<fila a devolver>>>");
               map_fila_retornar[nombre_col] = this->tabla_ptr->data_buffer_ptr->columns.at(nombre_col)[contador];
            };
         } else{
            Logger::log(LogLevel::ERROR, "NO existe el puntero 'data_buffer_ptr' en la tabla");
         };
         
         Logger::log(LogLevel::DEBUG, "Fila leida desde disco con exito");
      };
   }; // Termina la iteracion de columna
   contador += 1;
   if(contador >= (n_f_disk)){
      this-> eof = true;
   };
   return map_fila_retornar;
}; // Termina el metodo 'get_next_row'*/



//====================================================
//== tableRowIterator pero solo para RAM =============
//====================================================

// == Método constructor =============================
disk_buffer::tableRowIterator_only_ram::tableRowIterator_only_ram(std::string tabla_nombre){
   /*
   Constructor de la clase 'tableRowIterator_only_ram'
   */
   contador = 0;
   tabla_ptr = global_table_dict.at(tabla_nombre);
   // Solo en caso de tenerlo, contamos las filas en RAM viva:
   if(tabla_ptr->data_ptr &&  tabla_ptr->metadata_ptr->n_filas_ram == 0){
      contar_datos_ram_una_tabla(tabla_nombre);
   };

   this->eof = (tabla_ptr->metadata_ptr->n_filas_ram == 0);
};

// == Para consultar eof =============================
bool disk_buffer::tableRowIterator_only_ram::is_eof() {
   /*
   Simple función para retornar el atributo booleano 'eof'
   */
   return eof;
};


// == Para obtener la próxima fila (RAM viva) ========
std::map<std::string, Values> disk_buffer::tableRowIterator_only_ram::get_next_row_ram_viva(){
   /*
   Método parecido a 'get_next_row' de tableRowIterator. Sólo que aquí únicamente
   se retornaran datos de la misma sesión o RAM viva.
   Funciona de la siguiente manera:
      1º Itera por cada columna de la tabla.
      2º Dado el atributo 'contador' extrae dicho elemento de cada vector de columnas de cada columna,
         , siempre de los datos en RAM viva.
      3º Una vez terminado el bucle se actualiza el valor del atributo 'eof'.
      4º Se incrementa en uno el contador de fila.
   */
   std::map<std::string, Values> map_fila_retornar;
   uint32_t n_f_total = tabla_ptr->metadata_ptr->n_filas_ram;
   // Iteramos por cada columna:
   for (const std::string& nombre_col : tabla_ptr->metadata_ptr->column_names) {

      if(contador < n_f_total){
         // Leemos desde la RAM viva:
         map_fila_retornar[nombre_col] = tabla_ptr->data_ptr->columns.at(nombre_col)[contador];
      }; 
   };
   contador += 1;
   if(contador >= (tabla_ptr->metadata_ptr->n_filas_ram)){
      this-> eof = true;
   };
   return map_fila_retornar;
}; // Termina el metodo 'get_next_row_ram_viva'


///////////////////////////////////////////////////////////////////////////
//                 FSM ITERADOR DISCO Y RAM ///////////////////////////////
///////////////////////////////////////////////////////////////////////////

// == Método constructor =============================
disk_buffer::tableRowIterator::tableRowIterator(std::string tabla_nombre){
   /*
   Constructir de la clase 'tableRowIterator'
   */


   Logger::flush();
   Logger::log(LogLevel::DEBUG, "////////////////////////////////////////////////");
   Logger::log(LogLevel::DEBUG, "INICIAMOS LA ITERACION POR RAM Y DISCO");

   // Obtenemos el puntero de la tabla:
   tabla_ptr = global_table_dict.at(tabla_nombre);
   first_execution = true;
   eof = false; // Este es el false general, sólo si ambosa son true este será true
   eof_ram = false;
   eof_partition = true; // Epieza siendo true porque sólo así se leerá des disco una iteración
   eof_disk = false;
   estado_fsm = 0;
   table_name = this->tabla_ptr->metadata_ptr->name;

   // Iteradores auxiliares:
   this->iterator_ram = nullptr;
   this->table_reader_obj = nullptr;
   this->iterator_disco = nullptr;
   Logger::log(LogLevel::DEBUG, "Variables inicializadas");

};

bool disk_buffer::tableRowIterator::is_eof() {

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
   Logger::log(LogLevel::DEBUG, "Realizamos un ciclo de la CU");
   std::map<std::string, Values> map_fila_retornar;
   switch(this->estado_fsm){
      case 0: {
         Logger::log(LogLevel::DEBUG, "Estado 0. Acabamos de empezar");
         Logger::log(LogLevel::DEBUG, "Leeremos la RAM");
         // Vemos si existen datos en RAM:
         if(this->tabla_ptr->data_ptr && !(this->tabla_ptr->data_ptr->columns.empty())){
            Logger::log(LogLevel::DEBUG, "SI hay datos en la RAM de la tabla");
            Logger::log(LogLevel::DEBUG, "Creamos el iterador por la RAM:");
            this->iterator_ram = new disk_buffer::tableRowIterator_only_ram(this->table_name);
            Logger::log(LogLevel::DEBUG, "Iterador de la RAM creado con exito");
            this->estado_fsm = 1;
            break;
         };
         // En caso de no haber datos en RAM, pasamos directamente a ver si hay en disco
         Logger::log(LogLevel::DEBUG, "NO hay datos en la RAM. Pasamos directamente a leer de disco");
         this->eof_ram = true;
         this->estado_fsm = 2;
         break;
      };
      case 1: {
         // Iteracion de filas solo RAM
         Logger::log(LogLevel::DEBUG, "Estado 1. Leemos una fila de la RAM:");
         map_fila_retornar = this->iterator_ram->get_next_row_ram_viva();
         Logger::log(LogLevel::DEBUG, "Fila de la RAM leida con exito");
         
         // Vemos si está vacío lo que ha retornado:
         if(map_fila_retornar.empty()){
            Logger::log(LogLevel::DEBUG, "La fila ha retornado vacía de la RAM.");
            Logger::log(LogLevel::DEBUG, "Eso significa que no hay mas datos que leer en la RAM");
            this->eof_ram = true;
            // Cambiamos de estado:
            this->estado_fsm = 2;
            // Eliminamos el iterador de la RAM:
            delete this->iterator_ram;
            this->iterator_ram = nullptr;
            Logger::log(LogLevel::DEBUG, "Eliminacion del iterado de la RAM realizado");
            break;
         };
         this->estado_fsm = 1;
         break;
      };
      case 2: {
         // Comprobamos de antemano si hay datos en DISCO:
         Logger::log(LogLevel::DEBUG, "Estado 2. Inicio de la lectura en disco");
         Logger::log(LogLevel::DEBUG, "Antes vemo si hay datos en disco");
         if(fs::exists("data/" + this->table_name)){
            Logger::log(LogLevel::DEBUG, "SI que hay datos en disco");
            this->estado_fsm = 3;
            Logger::log(LogLevel::DEBUG, "Creamos el iterado de lectura y el de disco");
            // En ese caso creamos el objeto de lectura desde el disco:
            this->table_reader_obj = new disk_in::read_table_iterator(this->tabla_ptr);
            // inicializamos también el iterados en disco:
            this->iterator_disco = new disk_buffer::tableRowIterator_only_disk_part(this->table_name); 

            break;
         };
         Logger::log(LogLevel::DEBUG, "NO hay datos en disco");
         this->eof_disk = true;
         this->eof = true; // Porque la RAM ya se ha explorado
         this->estado_fsm = 255;
         break;
      };
      case 3: {
         // Lectura de una partición
         Logger::log(LogLevel::DEBUG, "Estado 3. Leemos una particion:");
         // Antes de leer la partición, 
         this->eof_disk = this->table_reader_obj->read_table();
         Logger::log(LogLevel::DEBUG, "Particion leida del disco con exito");
         Logger::log(LogLevel::DEBUG, "Reiniciamos el contador del iterador de solo disco");
         this->iterator_disco->contador = 0;
         Logger::log(LogLevel::DEBUG, "Lo reiniciaremos cada vez que leamos una partición nueva");
         if(this->eof_disk){
            Logger::log(LogLevel::DEBUG, "Ya se alcanzado el final de la lectura de TODAs las particiones");
            this->estado_fsm = 255; // Esto quiere que se ha llegado al fin
            this->eof = true; // Porque la RAM tambien llego a su fin o no existe
            // Al llegar al fin, eliminamos el objeto de la lectura y de iteracion por el disco:
            Logger::log(LogLevel::DEBUG, "Eliminamos el iterador de lectura y el de disco");
            delete this->table_reader_obj;
            this->table_reader_obj = nullptr;
            delete this->iterator_disco;
            this->iterator_disco = nullptr;
            break;
         };
         this->estado_fsm = 4;
         break;
      };
      case 4: {
         // Iteracion de filas solo disco
         Logger::log(LogLevel::DEBUG, "Estado 4. Leemos una fila:");
         Logger::log(LogLevel::DEBUG, "Fila leida del disco con exito");
         map_fila_retornar =  this->iterator_disco->get_next_row_only_disk_part(table_reader_obj);
         if(map_fila_retornar.empty()){
            Logger::log(LogLevel::DEBUG, "La fila ha retornado vacía del DISCO.");
            // Se ha alcanzado el fin de la partición:
            this->eof_partition = true;
            // Limpiamos la memoria de la tabla porviniente del disco:
            this->table_reader_obj->ptr_datos_disco->columns.clear();
            // Cambiamos de estado:
            this->estado_fsm = 3;
            break;
         };
         this->estado_fsm = 4;
         break;
      };
   };
   // Actualizamos el eof general:
   this->eof = this->eof_ram && this->eof_disk;
   return map_fila_retornar;
};


// == Método Para devolver la próxima fila =============================
std::map<std::string, Values> disk_buffer::tableRowIterator::get_next_row(){
   /*
   Este es el método que ejecuta la máquina de estados finitos.
   Hace avanzar de estado a la máquina hasta que esta retorne
   una fila que no está vacía o se haya llegado al estado final.
   */
   Logger::flush();
   Logger::log(LogLevel::DEBUG, "Se ha solicitado otra fila");
   std::map<std::string, Values> map_fila_retornar;

   while(map_fila_retornar.empty() && this->estado_fsm != 255) {
      Logger::log(LogLevel::DEBUG, "Hacemos una llamada a la Control Unit");
      map_fila_retornar = this->control_unit();
   };

   if(this->estado_fsm == 255){
      this->eof = true;
      Logger::log(LogLevel::DEBUG, "Se ha llegado al final");
   };
   return map_fila_retornar;
};









//====================================================
//== tableRowIterator para el WAL ====================
//====================================================






disk_buffer::tableRowIterator_only_ram_for_wal_inverse_order::tableRowIterator_only_ram_for_wal_inverse_order(std::string tabla_nombre, int num_filas_a_insertar){

   /*
   Constructor de la clase 'tableRowIterator_only_ram_for_wal_inverse_order'
   */
   this->n_filas_insertadas = num_filas_a_insertar;
   //contador = 0;
   tabla_ptr = global_table_dict.at(tabla_nombre);
   /*
   No contamos las filas en RAM viva porque solo recuperaremos
   los ultimos datos y si no fallo la inserción está asegurado
   que esas filas están en la RAM viva
   */
   if(tabla_ptr->data_ptr &&  tabla_ptr->metadata_ptr->n_filas_ram == 0){
      contar_datos_ram_una_tabla(tabla_nombre);
   };
   this->contador = tabla_ptr->metadata_ptr->n_filas_ram - 1;

   this->eof = (this->contador < 0);
}; // Termina el metodo 'tableRowIterator_only_ram_for_wal_inverse_order'

// == Para consultar eof =============================
bool disk_buffer::tableRowIterator_only_ram_for_wal_inverse_order::is_eof() {
   /*
   Simple función para retornar el atributo booleano 'eof'
   */
   Logger::log(LogLevel::DEBUG, "SE HA LLEGADO A LA CONDICION DE EOF EN ESCRITURA DE FILAS EN EL WAL");
   return eof;
};

// == Para obtener la próxima ultimas filas en RAM viva (RAM viva) ========
std::map<std::string, Values> disk_buffer::tableRowIterator_only_ram_for_wal_inverse_order::get_next_row_ram_viva(){
   /*
   Método parecido a 'get_next_row' de tableRowIterator. Sólo que aquí únicamente
   se retornaran datos de la misma sesión o RAM viva.
   Funciona de la siguiente manera:
      1º Itera por cada columna de la tabla.
      2º Dado el atributo 'contador' extrae dicho elemento de cada vector de columnas de cada columna,
         , siempre de los datos en RAM viva.
      3º Una vez terminado el bucle se actualiza el valor del atributo 'eof'.
      4º Se incrementa en uno el contador de fila.
   */
   std::map<std::string, Values> map_fila_retornar;
   uint32_t n_f_total = tabla_ptr->metadata_ptr->n_filas_ram;
   // Iteramos por cada columna:
   for (const std::string& nombre_col : tabla_ptr->metadata_ptr->column_names) {

      Logger::log(LogLevel::DEBUG, "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
      Logger::log(LogLevel::DEBUG, "Contador: ", false, true);
      Logger::log(LogLevel::DEBUG, this->contador, true, false);
      Logger::log(LogLevel::DEBUG, "n_f_total: ", false, true);
      Logger::log(LogLevel::DEBUG, n_f_total, true, false);
      Logger::log(LogLevel::DEBUG, "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
      if(this->contador < n_f_total){
         // Leemos desde la RAM viva:
         Logger::log(LogLevel::DEBUG, "Pasamois a rellenar el std::map de la fila a escribir en el WAL");
         map_fila_retornar[nombre_col] = tabla_ptr->data_ptr->columns.at(nombre_col)[this->contador];
      } else  {
         Logger::log(LogLevel::DEBUG, "La fila RETORNARÁ VACÍA");
      };
   };
   // Condiciones de contorno generales:
   if(this->contador < 0 || this->contador > tabla_ptr->metadata_ptr->n_filas_ram){
      this-> eof = true;
   };

   //if((tabla_ptr->metadata_ptr->n_filas_ram) - this->contador + 1 >= this->n_filas_insertadas){
   // Condición específica de EOF:
   if(this->contador < tabla_ptr->metadata_ptr->n_filas_ram - this->n_filas_insertadas + 1){
      this-> eof = true;
   };
   //} else if(contador <= 0) {
      //this-> eof = true;
   //};
   this->contador -= 1;
   return map_fila_retornar;
}; // Termina el metodo 'get_next_row_ram_viva'







disk_buffer::tableRowIterator_only_ram_for_wal::tableRowIterator_only_ram_for_wal(std::string tabla_nombre, int num_filas_a_insertar){

   /*
   Constructor de la clase 'tableRowIterator_only_ram_for_wal'
   */
   this->n_filas_insertadas = num_filas_a_insertar;
   //contador = 0;
   tabla_ptr = global_table_dict.at(tabla_nombre);
   /*
   No contamos las filas en RAM viva porque solo recuperaremos
   los ultimos datos y si no fallo la inserción está asegurado
   que esas filas están en la RAM viva
   */
   if(tabla_ptr->data_ptr &&  tabla_ptr->metadata_ptr->n_filas_ram == 0){
      contar_datos_ram_una_tabla(tabla_nombre);
   };
   this->contador = tabla_ptr->metadata_ptr->n_filas_ram - num_filas_a_insertar;

   this->eof = (this->contador >= tabla_ptr->metadata_ptr->n_filas_ram);
}; // Termina el metodo 'tableRowIterator_only_ram_for_wal'

// == Para consultar eof =============================
bool disk_buffer::tableRowIterator_only_ram_for_wal::is_eof() {
   /*
   Simple función para retornar el atributo booleano 'eof'
   */
   Logger::log(LogLevel::DEBUG, "SE HA LLEGADO A LA CONDICION DE EOF EN ESCRITURA DE FILAS EN EL WAL");
   return eof;
};

// == Para obtener la próxima ultimas filas en RAM viva (RAM viva) ========
std::map<std::string, Values> disk_buffer::tableRowIterator_only_ram_for_wal::get_next_row_ram_viva(){
   /*
   Método parecido a 'get_next_row' de tableRowIterator. Sólo que aquí únicamente
   se retornaran datos de la misma sesión o RAM viva.
   Funciona de la siguiente manera:
      1º Itera por cada columna de la tabla.
      2º Dado el atributo 'contador' extrae dicho elemento de cada vector de columnas de cada columna,
         , siempre de los datos en RAM viva.
      3º Una vez terminado el bucle se actualiza el valor del atributo 'eof'.
      4º Se incrementa en uno el contador de fila.
   */
   std::map<std::string, Values> map_fila_retornar;
   uint32_t n_f_total = tabla_ptr->metadata_ptr->n_filas_ram;
   // Iteramos por cada columna:
   for (const std::string& nombre_col : tabla_ptr->metadata_ptr->column_names) {

      Logger::log(LogLevel::DEBUG, "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
      Logger::log(LogLevel::DEBUG, "Contador: ", false, true);
      Logger::log(LogLevel::DEBUG, this->contador, true, false);
      Logger::log(LogLevel::DEBUG, "n_f_total: ", false, true);
      Logger::log(LogLevel::DEBUG, n_f_total, true, false);
      Logger::log(LogLevel::DEBUG, "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
      if(this->contador < n_f_total){
         // Leemos desde la RAM viva:
         Logger::log(LogLevel::DEBUG, "Pasamois a rellenar el std::map de la fila a escribir en el WAL");
         map_fila_retornar[nombre_col] = tabla_ptr->data_ptr->columns.at(nombre_col)[this->contador];
      } else  {
         Logger::log(LogLevel::DEBUG, "La fila RETORNARÁ VACÍA");
      };
   };
   this->contador += 1;
   // Condiciones de contorno generales:
   if(this->contador < 0 || this->contador >= tabla_ptr->metadata_ptr->n_filas_ram){
      this->eof = true;
   };
   return map_fila_retornar;
}; // Termina el metodo 'get_next_row_ram_viva'