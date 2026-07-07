#pragma once

#include "disk_in.h"

// Foward declarations:
namespace disk_in {
class read_table_iterator;
};


namespace disk_buffer {

   // ==========================================
   // == FUNCIONES AUXILIARES: CONTEO FILAS ====
   // ==========================================

   void contar_datos_ram_una_tabla(std::string& nombre_tabla);


   void contar_datos_en_ram_todas_tablas();


   //====================================================
   //== tableRowIterator pero solo para RAM =============
   //====================================================
   class tableRowIterator_only_ram {
      /*
      Muy similar a la clase 'tableRowIterator', pero sólo itera por los datos añadidos
      en la misma sesión (RAM viva).
      Posee los siguientes atributos:
         -> contador: para trackear la fila por la que se está iterando/recuperando
         -> tabla_ptr: puntero a la tabla por la que se quiere obtener las filas
         -> eof: condicional que indica si ya no qudan más filas por las que iterar:
            * Si es true: se ha llegado al final y no hay más filas que devolver
            * Si es false: todavía queda una fila o más por iterar
      Al crearse la clase automáticamente:
         -> Se cuentan los datos en RAM viva.
         -> Se calcula el valor inicial de la variable eof.
      */
      public:
         uint32_t contador;
         table* tabla_ptr;
         uint32_t n_f_total;
         bool eof;

      tableRowIterator_only_ram(const std::string& tabla_nombre);

      // Para consultar eof:
      bool is_eof() const;

      // == PARA OBTENER LA PROXIMA FILA de la ram viva:
      std::map<std::string, Values> get_next_row_ram_viva();
   };


   //====================================================
   //== tableRowIterator pero solo para DISCO ===========
   //====================================================
   
   class tableRowIterator_only_disk_part {
      public:
         uint32_t contador;
         table* tabla_ptr;
         bool eof;

         // Metodo constructor:
         tableRowIterator_only_disk_part(const std::string& tabla_nombre);

         // Para consultar eof:
         bool is_eof() const;

         // == PARA OBTENER LA PROXIMA FILA del disco:
         std::map<std::string, Values> get_next_row_only_disk_part(disk_in::read_table_iterator* table_reader_obj);

   };


   // ==========================================
   // == FUNCION PRINCIPAL: ITERADOR FILAS =====
   // ==========================================

   class tableRowIterator {
      /*
      Clase para iterar y retornar filas, venidas del disco o de la propia sesión.
      Esta clase engloba lo que es una máquina de estados finitos, manejando así
      la lectrua de filas tanto de la RAM como de las particiones en disco.
      Posee los siguientes atributos:
         -> tabla_ptr: puntero a la tabla por la que se quiere obtener las filas
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
         ->estado_fsm: Es el estado de la máquina de estados finitos, sus estados son los siguientes:
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
            * iterator_disco: iterador que devuelve las filas exclusivamente recupoerdas al leer del disco.
      */
      public:
         //uint32_t contador;
         table* tabla_ptr;
         bool first_execution;
         bool eof;
         bool eof_ram;
         bool eof_partition; // Si se ha terminado o no de leer una particion
         bool eof_disk;
         uint8_t estado_fsm;
         std::string table_name;
         
         // Iteradores auxiliares:
         tableRowIterator_only_ram* iterator_ram;
         disk_in::read_table_iterator* table_reader_obj;
         tableRowIterator_only_disk_part* iterator_disco;

         // Metodo constructor
         tableRowIterator(const std::string& tabla_nombre);

         // Para consultar eof:
         bool is_eof() const;

         // Unidad de control:
         std::map<std::string, Values> control_unit();

         // Para devolver la prox fila:
         std::map<std::string, Values> get_next_row();
   };



   //====================================================
   //== tableRowIterator pero solo para RAM en WAL ======
   //====================================================
   class tableRowIterator_only_ram_for_wal_inverse_order {
      /*
      Muy similar a la clase 'tableRowIterator', pero sólo itera por los datos añadidos
      en una sola operación de inserción de datos.
      Retornará las últimas N filas insertadas en una operación de inserción
      Posee los siguientes atributos:
         -> contador: para trackear la fila por la que se está iterando/recuperando.
            en este caso, empezará a contar desde el final hacia el principio, para
            recuperar las N últimas filas.
         -> tabla_ptr: puntero a la tabla por la que se quiere obtener las filas
         -> eof: condicional que indica si ya no qudan más filas por las que iterar:
            * Si es true: se han recuperado las N filas añadidas en la operación de inserción de datos en RAM viva.
            * Si es false: todavía queda una fila o más por iterar
      Al crearse la clase automáticamente:
         -> Se cuentan los datos en RAM viva.
         -> Se calcula el valor inicial de la variable eof.
      */
      public:
         int32_t contador; // Caso especial, este puede tomar el valor de negativos
         uint32_t n_filas_insertadas;
         uint32_t n_f_total;
         uint32_t lower_limit;
         table* tabla_ptr;
         bool eof;

      tableRowIterator_only_ram_for_wal_inverse_order(const std::string& tabla_nombre, const int num_filas_a_insertar);

      // Para consultar eof:
      bool is_eof() const;

      // == PARA OBTENER LA PROXIMA FILA de la ram viva:
      std::map<std::string, Values> get_next_row_ram_viva();
   };






   class tableRowIterator_only_ram_for_wal {
      /*
      Muy similar a la clase 'tableRowIterator', pero sólo itera por los datos añadidos
      en una sola operación de inserción de datos.
      Retornará las últimas N filas insertadas en una operación de inserción
      Posee los siguientes atributos:
         -> contador: para trackear la fila por la que se está iterando/recuperando.
            en este caso, empezará a contar desde el final hacia el principio, para
            recuperar las N últimas filas.
         -> tabla_ptr: puntero a la tabla por la que se quiere obtener las filas
         -> eof: condicional que indica si ya no qudan más filas por las que iterar:
            * Si es true: se han recuperado las N filas añadidas en la operación de inserción de datos en RAM viva.
            * Si es false: todavía queda una fila o más por iterar
      Al crearse la clase automáticamente:
         -> Se cuentan los datos en RAM viva.
         -> Se calcula el valor inicial de la variable eof.
      */
      public:
         uint32_t contador;
         uint32_t n_filas_insertadas;
         uint32_t n_f_total;
         table* tabla_ptr;
         bool eof;

      tableRowIterator_only_ram_for_wal(const std::string& tabla_nombre, const int num_filas_a_insertar);

      // Para consultar eof:
      bool is_eof() const;

      // == PARA OBTENER LA PROXIMA FILA de la ram viva:
      std::map<std::string, Values> get_next_row_ram_viva();
   };

}; // Cierre del namespace 'disk_buffer'
