#pragma once


enum class dataType {
   /*
   Tipo de datos para los datos de las tablas, los cuales son:
      -> INT: correspondiente a 'int' en C++
      -> FLOAT: correspondiente a 'float' en C++
      -> STRING: correspondiente a 'std::string' en C++
      -> BOOL: correspondiente a 'bool' en C++
      -> UNKNOWN: tipo de datos desconocido o indeterminado. Será tratado como una 'std::string' de C++
   */
   INT,
   FLOAT,
   STRING,
   BOOL,
   UNKNOWN
};

struct table_metadata {
   /*
   Metadatos de las tablas.
   Posee los siguientes atributos:
      -> name: nombre de la tabla
      -> column_names: nombres en orden definición de las columnas
      -> column_types: tipo de dato (dataType) de las columnas en orden de deffinción
      -> primary_list:vector que indica ssi la columna es clave primaria (true) o no (false). Definido por orden definición de las columnas
      -> n_filas_disco: Nº de filas en "disco" (o filas que han sido leidas desde el disco y cargadas en memoria volátil)
      ->n_filas_ram: Nº de filas añadidas en la misma sesión o ejecución del programa 

   Inicializador:
      -> name como string vacía y de longitud nula
      -> n_filas_disco a cero
      -> n_filas_ram a cero

   */
   std::string name;
   std::vector<std::string> column_names;
   std::vector<dataType> column_types;
   std::vector<bool> primary_list;
   uint32_t n_cols;
   uint32_t n_filas_disco;
   uint32_t n_filas_ram;
   table_metadata():
      name(""), 
      n_cols(0),
      n_filas_disco(0), 
      n_filas_ram(0) 
      {};
};


using Values = std::variant<int, float, bool, std::string, std::vector<char>>;


struct table_data {
   /*
   Datos de la tabla que fueron añadidos en la misma sesión/ejecución del programa.
   Los datos están en un diccionario o hash map en donde:
      -> La clave es el nombre de la columna
      -> El valor es el vector con los valores de la columna
   */
	std::map<std::string, std::vector<Values>> columns;
};


struct table_data_buffer {
   /*
   Datos de la tabla recuperados del disco duro.
   Los datos están en un diccionario o hash map en donde:
      -> La clave es el nombre de la columna
      -> El valor es el vector con los valores de la columna
   */
   std::map<std::string, std::vector<Values>> columns;
};

struct table {
   /*
   Estructura de una tabla, la cual cuenta con los siguientes punteros:
      -> metadata_ptr: puntero a los metadatos de la tabla
      -> data_ptr: puntero a los datos definidos en la misma sesión
      -> data_buffer_ptr: puntero a los datos recuperados/leídos desde el disco
   
   Estos tres punteros son iniializados cuando se crea la instancia de la clase
   como punteros nulos para mayor seguridad.
   */
   table_metadata* metadata_ptr;
   table_data* data_ptr;
   table_data_buffer* data_buffer_ptr;

   table(): metadata_ptr(nullptr), data_ptr(nullptr), data_buffer_ptr(nullptr){};
};
