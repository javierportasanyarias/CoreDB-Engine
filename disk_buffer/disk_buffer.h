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
}; // Cierre del namespace 'disk_buffer'

#endif