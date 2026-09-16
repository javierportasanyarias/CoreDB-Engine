#!/usr/bin/env bash

# 1. Definimos todos tus flags de inclusión (-I) en una sola variable
INCLUDES=(
  -Iexecution
  -Iplanning_execution
  -Inodes_for_trees
  -Itext_manipulation
  -Iglobals
  -Idata_structure
  -Ilog
  -Idisk_io
  -Idisk_in
  -Idisk_out
  -Idisk_metadata
  -Idisk_aux
  -Idisk_wal
  -Idisk_buffer
  -Itests
  -Ipartition_sorting
)

# 2. Lista de todos tus archivos .cpp a analizar
FUENTES=(
  app/main.cpp
  execution/execution.cpp
  planning_execution/execution_planning.cpp
  nodes_for_trees/node_for_trees.cpp
  text_manipulation/process_tokens.cpp
  text_manipulation/textutils.cpp
  globals/globals.cpp
  log/logging.cpp
  tests/data_structs.cpp
  tests/bateria_tests.cpp
  disk_io/disk_io.cpp
  disk_in/disk_in.cpp
  disk_out/disk_out.cpp
  disk_metadata/disk_metadata.cpp
  disk_aux/disk_aux.cpp
  disk_wal/disk_wal_write.cpp
  disk_wal/disk_wal_read.cpp
  disk_wal/disk_wal_utils.cpp
  disk_buffer/disk_buffer.cpp
  partition_sorting/part_sort.cpp
)

echo "=== INICIANDO ANÁLISIS DE ENCABEZADOS CON IWYU ==="
echo "Guardando resultados en reporte_iwyu.txt..."
echo "" > reporte_iwyu.txt

# 🌟 TRUCO MAESTRO: Forzamos a IWYU a usar las rutas de búsqueda exactas de tu compilador real.
# Esto incluye las cabeceras ocultas donde vive 'stddef.h'.
SISTEMA_FLAGS=$(clang++ -E -x c++ - -v < /dev/null 2>&1 | awk '/#include <...>/,/End of search list./' | grep '^ ' | sed 's/^ /-I/')

# 3. El bucle que analiza cada archivo uno por uno
for archivo in "${FUENTES[@]}"; do
    echo "Analizando: $archivo..."
    echo "=========================================" >> reporte_iwyu.txt
    echo "REPORTE PARA: $archivo" >> reporte_iwyu.txt
    echo "=========================================" >> reporte_iwyu.txt
    
    # Ejecutamos iwyu inyectando todas las rutas de cabeceras reales de tu sistema
    include-what-you-use -std=c++20 $SISTEMA_FLAGS "${INCLUDES[@]}" "$archivo" >> reporte_iwyu.txt 2>&1
    
    echo "" >> reporte_iwyu.txt
done

echo "=== ¡ANÁLISIS COMPLETADO! ==="
echo "Revisa el archivo 'reporte_iwyu.txt' para ver todas las optimizaciones pendientes."