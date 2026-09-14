#pragma once

#include "disk_in.h"

// Foward declarations:
namespace disk_in {
class read_table_iterator;
};


namespace disk_buffer {


   //====================================================
   //========= tableRowIterator only RAM ================
   //====================================================
   class tableRowIterator_only_ram {
      /*
      Muy similar a la clase 'tableRowIterator', pero sólo itera por los datos añadidos
      en la misma sesión (RAM viva).
      Posee los siguientes atributos:
         -> counter: para trackear la fila por la que se está iterando/recuperando
         -> table_ptr: puntero a la tabla por la que se quiere obtener las filas
         -> eof: condicional que indica si ya no qudan más filas por las que iterar:
            * Si es true: se ha llegado al final y no hay más filas que devolver
            * Si es false: todavía queda una fila o más por iterar
      Al crearse la clase automáticamente:
         -> Se cuentan los datos en RAM viva.
         -> Se calcula el valor inicial de la variable eof.
      */
      public:
         uint32_t counter;
         table* table_ptr;
         uint32_t n_rows_total;
         bool eof;

      tableRowIterator_only_ram(const std::string& table_name_str);

      // Para consultar eof:
      bool is_eof() const;

      // == PARA OBTENER LA PROXIMA FILA de la ram viva:
      std::map<std::string, Values> get_next_row_ram();
   };


   //====================================================
   //========= tableRowIterator only disk ===============
   //====================================================
   
   class tableRowIterator_only_disk_part {
      public:
         uint32_t counter;
         table* table_ptr;
         bool eof;

         // Metodo constructor:
         tableRowIterator_only_disk_part(const std::string& table_name_str);

         // Para consultar eof:
         bool is_eof() const;

         // == PARA OBTENER LA PROXIMA FILA del disco:
         std::map<std::string, Values> get_next_row_only_disk_part(disk_in::read_table_iterator* table_reader_obj);

   };


   //====================================================
   //========= FSM iterator for disk and RAM ============
   //====================================================

   class tableRowIterator {
      /*
      Clase para iterar y retornar filas, venidas del disco o de la propia sesión.
      Esta clase engloba lo que es una máquina de estados finitos, manejando así
      la lectrua de filas tanto de la RAM como de las particiones en disco.
      Posee los siguientes atributos:
         -> table_ptr: puntero a la tabla por la que se quiere obtener las filas
         -> first_execution: booleano que trackea si ha sido la primera ejecución o no
         -> eof: condicional que indica si ya no qudan más filas por las que iterar:
            * Si es true: se ha llegado al final y no hay más filas que devolver
            * Si es false: todavía queda una fila o más por iterar
            Se incluyen tanto filas definidas en la propia sesión o leídas del disco.
            Es verdadera si eof_ram y eof_disk son verdaderas las dos.
         -> eof_ram: condicional para medir si se ha llegado al final de las filas
               efinidas en la sesión.
         -> eof_partition: condicional para medir si se ha terminado de leer una partición.
         -> eof_ram: condicional para medir si se ha llegado al final de las filas
            definidas provenientes de los datos en disco.
         ->fsm_state: Es el estado de la máquina de estados finitos, sus estados son los siguientes:
            * 1:
            * 2:
            * 3:
            * 4:
            * 255:
            Este estado se guarda en un entero sin signo de 1 byte, con fin de ahorrar memoria
         -> table_name: nombre de la tabla
         -> Iteradores: Estos son objetos que nos permitirán obtener las filas de la sesión o del disco:
            * iterator_ram: iterador únicamente preparado para retornar las filas definidas en esa 
              misma sesión.
            * table_reader_obj: objeto que lee y carga temporalmente en memoria de la tabla los datos
              de cada partición. Va recuperando y cargando dicha información partición a partición.
            * disk_iterator: iterador que devuelve las filas exclusivamente recupoerdas al leer del disco.
      */
      public:
         //uint32_t counter;
         table* table_ptr;
         bool first_execution;
         bool eof;
         bool eof_ram;
         bool eof_partition; // Si se ha terminado o no de leer una particion
         bool eof_disk;
         uint8_t fsm_state;
         std::string table_name;
         
         // Iteradores auxiliares:
         tableRowIterator_only_ram* iterator_ram;
         disk_in::read_table_iterator* table_reader_obj;
         tableRowIterator_only_disk_part* disk_iterator;

         // Metodo constructor
         tableRowIterator(const std::string& table_name_str);

         // Para consultar eof:
         bool is_eof() const;

         // Unidad de control:
         std::map<std::string, Values> control_unit();

         // Para devolver la prox fila:
         std::map<std::string, Values> get_next_row();
   };

   //====================================================
   //== tableRowIterator only WAL (inverse insertion order)
   //====================================================
   
   class tableRowIterator_only_ram_for_wal_inverse_order {
      /*
      Muy similar a la clase 'tableRowIterator', pero sólo itera por los datos añadidos
      en una sola operación de inserción de datos.
      Retornará las últimas N filas insertadas en una operación de inserción
      Posee los siguientes atributos:
         -> counter: para trackear la fila por la que se está iterando/recuperando.
            en este caso, empezará a contar desde el final hacia el principio, para
            recuperar las N últimas filas.
         -> table_ptr: puntero a la tabla por la que se quiere obtener las filas
         -> eof: condicional que indica si ya no qudan más filas por las que iterar:
            * Si es true: se han recuperado las N filas añadidas en la operación de inserción de datos en RAM viva.
            * Si es false: todavía queda una fila o más por iterar
      Al crearse la clase automáticamente:
         -> Se cuentan los datos en RAM viva.
         -> Se calcula el valor inicial de la variable eof.
      */
      public:
         int32_t counter; // Caso especial, este puede tomar el valor de negativos
         uint32_t n_rows_inserted;
         uint32_t n_rows_total;
         uint32_t lower_limit;
         table* table_ptr;
         bool eof;

      tableRowIterator_only_ram_for_wal_inverse_order(const std::string& table_name_str, const int num_rows_to_insert);

      // Para consultar eof:
      bool is_eof() const;

      // == PARA OBTENER LA PROXIMA FILA de la ram viva:
      std::map<std::string, Values> get_next_row_ram();
   };



   //====================================================
   //== tableRowIterator only WAL =======================
   //====================================================


   class tableRowIterator_only_ram_for_wal {
      /*
      Muy similar a la clase 'tableRowIterator', pero sólo itera por los datos añadidos
      en una sola operación de inserción de datos.
      Retornará las últimas N filas insertadas en una operación de inserción
      Posee los siguientes atributos:
         -> counter: para trackear la fila por la que se está iterando/recuperando.
            en este caso, empezará a contar desde el final hacia el principio, para
            recuperar las N últimas filas.
         -> table_ptr: puntero a la tabla por la que se quiere obtener las filas
         -> eof: condicional que indica si ya no qudan más filas por las que iterar:
            * Si es true: se han recuperado las N filas añadidas en la operación de inserción de datos en RAM viva.
            * Si es false: todavía queda una fila o más por iterar
      Al crearse la clase automáticamente:
         -> Se cuentan los datos en RAM viva.
         -> Se calcula el valor inicial de la variable eof.
      */
      public:
         uint32_t counter;
         uint32_t n_rows_inserted;
         uint32_t n_rows_total;
         table* table_ptr;
         bool eof;

      tableRowIterator_only_ram_for_wal(const std::string& table_name_str, const int num_rows_to_insert);

      // Para consultar eof:
      bool is_eof() const;

      // == PARA OBTENER LA PROXIMA FILA de la ram viva:
      std::map<std::string, Values> get_next_row_ram();
   };

}; // Cierre del namespace 'disk_buffer'
