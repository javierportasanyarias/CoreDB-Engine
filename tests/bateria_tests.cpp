#include "bateria_tests.h"

// -- FUNCION AUXILIAR PARA DEFINIR TEST:
FIFO* define_test_1(){
                                                                               FIFO* fifo_obj = new FIFO;                                                  std::string input = "    CREATE    TABLE  t1    ( ID STRING PRIMARY KEY , Edad INT, Producto STRING) ; INSERT INTO t1 VALUES( 'Carlos', 49, 'Secadora'); INSERT INTO t1 VALUES( 'Andrea', 34, 'Lavadora'); INSERT INTO t1 ( ID, Edad, Producto) VALUES( 'Edu', 29, 'Plancha'); INSERT INTO t1 VALUES ('Cassandra', 47, 'Batidora'), ( 'Lucas', 19, 'Correa'); SELECT * FROM t1; SELECT ID, Producto FROM t1;  DROP TABLE t1  ;\n";

   FifoNode* nodo_1 = new FifoNode;
   nodo_1->comando = input;
   fifo_obj->head = nodo_1;                                                    FifoNode* nodo_2 = new FifoNode;
   nodo_1->nxt_node = nodo_2;
   nodo_2->comando = input;
   FifoNode* nodo_3 = new FifoNode;                                            nodo_2->nxt_node = nodo_3;                                                  std::string input_2 = "exit\n";                                             nodo_3->comando = input_2;                                                                                                                              return fifo_obj;                                                                                                                                     };                                                                                                                                                                                                                                  FIFO* define_test_2(){                                                                                                                   FIFO* fifo_obj = new FIFO;                                                  std::string input = "    CREATE    TABLE  t1    ( ID STRING PRIMARY KEY , Edad INT, Producto STRING) ; INSERT INTO t1 VALUES( 'Carlos', 49, 'Secadora'); INSERT INTO t1 VALUES( 'Andrea', 34, 'Lavadora'); INSERT INTO t1 ( ID, Edad, Producto) VALUES( 'Edu', 29, 'Plancha'); INSERT INTO t1 VALUES ('Cassandra', 47, 'Batidora'), ( 'Lucas', 19, 'Correa'); SELECT * FROM t1; SELECT ID, Producto FROM t1;\n";                                                        FifoNode* nodo_1 = new FifoNode;
   nodo_1->comando = input;
   fifo_obj->head = nodo_1;
   return fifo_obj;
                                                                            };


FIFO* define_test_3(){                                                                                                                       FIFO* fifo_obj = new FIFO;                           std::string input_1 = "    CREATE    TABLE  t1    ( ID STRING PRIMARY KEY , Edad INT, Producto STRING) ; INSERT INTO t1 VALUES( 'Carlos', 49, 'Secadora'); INSERT INTO t1 VALUES( 'Andrea', 34, 'Lavadora'); INSERT INTO t1 ( ID, Edad, Producto) VALUES( 'Edu', 29, 'Plancha'); INSERT INTO t1 VALUES ('Cassandra', 47, 'Batidora'), ( 'Lucas', 19, 'Correa'); SELECT * FROM t1; SELECT ID, Producto FROM t1;\n";

	std::string input_2 = "    CREATE    TABLE  t2    ( ID STRING PRIMARY KEY , Edad INT, Producto STRING) ; INSERT INTO t2 VALUES( 'Carlos', 49, 'Secadora'); INSERT INTO t2 VALUES( 'Andrea', 34, 'Lavadora'); INSERT INTO t2 ( ID, Edad, Producto) VALUES( 'Edu', 29, 'Plancha'); INSERT INTO t2 VALUES ('Cassandra', 47, 'Batidora'), ( 'Lucas', 19, 'Correa'); SELECT ID, Producto FROM t2;\n";

	std::string input_3 = "exit\n";
	FifoNode* nodo_1 = new FifoNode;
	nodo_1->comando = input_1;                                    fifo_obj->head = nodo_1;
	FifoNode* nodo_2 = new FifoNode;                nodo_1->nxt_node = nodo_2;                                    nodo_2->comando = input_2;
   FifoNode* nodo_3 = new FifoNode;                                            nodo_2->nxt_node = nodo_3;
   nodo_3->comando = input_3;                                                                                                                              return fifo_obj;                                                                                                                                     };
