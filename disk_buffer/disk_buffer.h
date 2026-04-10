#ifndef DISK_BUFFER
#define DISK_BUFFER

#include "globals.h"
#include "disk_io.h"
#include "logging.h"

namespace disk_buffer {

   // ==========================================
   // == FUNCIONES AUXILIARES: CONTEO FILAS ====
   // ==========================================

   void contar_datos_ram_una_tabla(std::string& nombre_tabla);


   void contar_datos_en_ram_todas_tablas();
   // ==========================================
   // == FUNCION PRINCIPAL: ITERADOR FILAS =====
   // ==========================================

   class tableRowIterator {
      /*
      Clase para iterar y retornar filas, venidas del disco o de la propia sesión.
      Posee los siguientes atributos:
         -> contador: para trackear la fila por la que se está iterando/recuperando
         -> tabla_ptr: puntero a la tabla por la que se quiere obtener las filas
         -> eof: condicional que indica si ya no qudan más filas por las que iterar:
            * Si es true: se ha llegado al final y no hay más filas que devolver
            * Si es false: todavía queda una fila o más por iterar
      Al crearse la clase automáticamente:
         -> Se cuentan los datos en RAM viva.
         -> Si existe archivo de datos, este se lee del disco.
         -> Se calcula el valor inicial de la variable eof.
      */
      public:
         uint32_t contador;
         table* tabla_ptr;
         bool eof;

         // Metodo constructor
         tableRowIterator(std::string tabla_nombre);

         // Para consultar eof:
         bool is_eof();

         // Para devolver la prox fila:
         std::map<std::string, Values> get_next_row();
   };
   
   
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
         bool eof;

      tableRowIterator_only_ram(std::string tabla_nombre);

      // Para consultar eof:
      bool is_eof();

      // == PARA OBTENER LA PROXIMA FILA de la ram viva:
      std::map<std::string, Values> get_next_row_ram_viva();
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
         uint32_t contador;
         uint32_t n_filas_insertadas;
         table* tabla_ptr;
         bool eof;

      tableRowIterator_only_ram_for_wal_inverse_order(std::string tabla_nombre, int num_filas_a_insertar);

      // Para consultar eof:
      bool is_eof();

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
         table* tabla_ptr;
         bool eof;

      tableRowIterator_only_ram_for_wal(std::string tabla_nombre, int num_filas_a_insertar);

      // Para consultar eof:
      bool is_eof();

      // == PARA OBTENER LA PROXIMA FILA de la ram viva:
      std::map<std::string, Values> get_next_row_ram_viva();
   };

}; // Cierre del namespace 'disk_buffer'

#endif