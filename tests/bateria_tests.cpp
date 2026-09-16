#include "bateria_tests.h"

// -- AUXILIAR FUNCTION FOR TEST DEFINITION:
FIFO* define_test_1() {
  FIFO* fifo_obj = new FIFO;
  std::string input =
      "    CREATE    TABLE  t1    ( ID STRING PRIMARY KEY , Edad INT, Producto "
      "STRING) ; INSERT INTO t1 VALUES( 'Carlos', 49, 'Secadora'); INSERT INTO "
      "t1 VALUES( 'Andrea', 34, 'Lavadora'); INSERT INTO t1 ( ID, Edad, "
      "Producto) VALUES( 'Edu', 29, 'Plancha'); INSERT INTO t1 VALUES "
      "('Cassandra', 47, 'Batidora'), ( 'Lucas', 19, 'Correa'); SELECT * FROM "
      "t1; SELECT ID, Producto FROM t1;";

  FifoNode* nodo_1 = new FifoNode;
  nodo_1->comando = input;
  fifo_obj->head = nodo_1;
  FifoNode* nodo_2 = new FifoNode;
  nodo_1->nxt_node = nodo_2;
  nodo_2->comando = input;
  FifoNode* nodo_3 = new FifoNode;
  nodo_2->nxt_node = nodo_3;
  std::string input_2 = "exit";
  nodo_3->comando = input_2;
  return fifo_obj;
};

FIFO* define_test_2() {
  FIFO* fifo_obj = new FIFO;
  std::string input =
      "    CREATE    TABLE  t1    ( ID STRING PRIMARY KEY , Edad INT, Producto "
      "STRING) ; INSERT INTO t1 VALUES( 'Carlos', 49, 'Secadora'); INSERT INTO "
      "t1 VALUES( 'Andrea', 34, 'Lavadora'); INSERT INTO t1 ( ID, Edad, "
      "Producto) VALUES( 'Edu', 29, 'Plancha'); INSERT INTO t1 VALUES "
      "('Cassandra', 47, 'Batidora'), ( 'Lucas', 19, 'Correa'); SELECT * FROM "
      "t1; SELECT ID, Producto FROM t1;";
  FifoNode* nodo_1 = new FifoNode;
  nodo_1->comando = input;
  fifo_obj->head = nodo_1;
  FifoNode* nodo_2 = new FifoNode;
  nodo_1->nxt_node = nodo_2;
  nodo_2->comando = "exit";
  return fifo_obj;
};

FIFO* define_test_3() {
  FIFO* fifo_obj = new FIFO;
  std::string input_1 =
      "    CREATE    TABLE  t1    ( ID STRING PRIMARY KEY , Edad INT, Producto "
      "STRING) ; INSERT INTO t1 VALUES( 'Carlos', 49, 'Secadora'); INSERT INTO "
      "t1 VALUES( 'Andrea', 34, 'Lavadora'); INSERT INTO t1 ( ID, Edad, "
      "Producto) VALUES( 'Edu', 29, 'Plancha'); INSERT INTO t1 VALUES "
      "('Cassandra', 47, 'Batidora'), ( 'Lucas', 19, 'Correa'); SELECT * FROM "
      "t1; SELECT ID, Producto FROM t1; DROP TABLE t1 ;";
  std::string input_2 =
      "    CREATE    TABLE  t2    ( ID STRING PRIMARY KEY , Edad INT, Producto "
      "STRING) ; INSERT INTO t2 VALUES( 'Carlos', 49, 'Secadora'); INSERT INTO "
      "t2 VALUES( 'Andrea', 34, 'Lavadora'); INSERT INTO t2 ( ID, Edad, "
      "Producto) VALUES( 'Edu', 29, 'Plancha'); INSERT INTO t2 VALUES "
      "('Cassandra', 47, 'Batidora'), ( 'Lucas', 19, 'Correa'); SELECT ID, "
      "Producto FROM t2 ; DROP TABLE t2 ;";

  std::string input_3 = "exit";
  FifoNode* nodo_1 = new FifoNode;
  nodo_1->comando = input_1;
  fifo_obj->head = nodo_1;
  FifoNode* nodo_2 = new FifoNode;
  nodo_1->nxt_node = nodo_2;
  nodo_2->comando = input_2;
  FifoNode* nodo_3 = new FifoNode;
  nodo_2->nxt_node = nodo_3;
  nodo_3->comando = input_3;
  return fifo_obj;
};

FIFO* define_test_4() {
  FIFO* fifo_obj = new FIFO;
  std::string input_1 =
      "    CREATE    TABLE  t1    ( ID STRING PRIMARY KEY , Edad INT, Producto "
      "STRING) ; INSERT INTO t1 VALUES( 'Carlos', 49, 'Secadora'); INSERT INTO "
      "t1 VALUES( 'Andrea', 34, 'Lavadora'); INSERT INTO t1 ( ID, Edad, "
      "Producto) VALUES( 'Edu', 29, 'Plancha'); INSERT INTO t1 VALUES "
      "('Cassandra', 47, 'Batidora'), ( 'Lucas', 19, 'Correa'); SELECT * FROM "
      "t1; SELECT ID, Producto FROM t1; DROP TABLE t1 ;";
  std::string input_2 =
      "    CREATE    TABLE  t2    ( ID STRING PRIMARY KEY , Edad INT, Producto "
      "STRING) ; INSERT INTO t2 VALUES( 'Carlos', 49, 'Secadora'); INSERT INTO "
      "t2 VALUES( 'Andrea', 34, 'Lavadora'); INSERT INTO t2 ( ID, Edad, "
      "Producto) VALUES( 'Edu', 29, 'Plancha'); INSERT INTO t2 VALUES "
      "('Cassandra', 47, 'Batidora'), ( 'Lucas', 19, 'Correa'); SELECT ID, "
      "Producto FROM t2 ;";

  std::string input_3 = "exit";
  FifoNode* nodo_1 = new FifoNode;
  nodo_1->comando = input_1;
  fifo_obj->head = nodo_1;
  FifoNode* nodo_2 = new FifoNode;
  nodo_1->nxt_node = nodo_2;
  nodo_2->comando = input_2;
  FifoNode* nodo_3 = new FifoNode;
  nodo_2->nxt_node = nodo_3;
  nodo_3->comando = input_3;
  return fifo_obj;
};

FIFO* define_test_5() {
  FIFO* fifo_obj = new FIFO;
  std::string input_1 =
      " INSERT INTO t2 VALUES( 'Rigoberto1', 112, 'Secadora'); INSERT INTO t2 "
      "VALUES('Rigoberto2', 112, 'Secadora') ; SELECT * FROM t2; SELECT ID, "
      "Producto FROM t2;";

  std::string input_2 = "exit";
  FifoNode* nodo_1 = new FifoNode;
  nodo_1->comando = input_1;
  fifo_obj->head = nodo_1;
  FifoNode* nodo_2 = new FifoNode;
  nodo_1->nxt_node = nodo_2;
  nodo_2->comando = input_2;
  return fifo_obj;
};

FIFO* define_test_6() {
  FIFO* fifo_obj = new FIFO;
  std::string input_1 = " SELECT * FROM t2; SELECT ID, Producto FROM t2;";
  std::string input_2 = "exit";
  FifoNode* nodo_1 = new FifoNode;
  nodo_1->comando = input_1;
  fifo_obj->head = nodo_1;
  FifoNode* nodo_2 = new FifoNode;
  nodo_1->nxt_node = nodo_2;
  nodo_2->comando = input_2;
  return fifo_obj;
};

// COMPLETE TEST FOR ASSERTING MEMORY LEAKS:

FIFO* define_test_7() {
  FIFO* fifo_obj = new FIFO;
  std::string input_1 =
      "         CREATE    TABLE  t6    ( ID STRING PRIMARY KEY , Edad INT, "
      "Producto UNKNOWN) ; INSERT INTO t6 VALUES( 'Carlos', 49, 'Secadora'); "
      "INSERT INTO t6 VALUES( 'Andrea', 34, 'Lavadora'); INSERT INTO t6 ( ID, "
      "Edad, Producto) VALUES( 'Edu', 29, 'Plancha'); INSERT INTO t6 VALUES "
      "('Cassandra', 47, 'Batidora'), ( 'Lucas', 19, 'Correa'); SELECT ID, "
      "Producto FROM t6;";
  std::string input_2 =
      "    INSERT INTO t6 VALUES('Rigoberto1', 11, 'Esternocleidomastoideo1'), "
      "( 'Rigoberto2', 22, 'Esternocleidomastoideo2'), ('Rigoberto3', 33, "
      "'Esternocleidomastoideo3'), ('Rigoberto4', 44, "
      "'Esternocleidomastoideo4'), ('Rigoberto5', 55, "
      "'Esternocleidomastoideo5');";

  std::string input_3 =
      "     CREATE    TABLE  t7    ( ID STRING PRIMARY KEY , Edad INT, "
      "Producto STRING) ; INSERT INTO t7 VALUES( 'Andrea', 34, 'Lavadora'); "
      "INSERT INTO t7 ( ID, Edad, Producto) VALUES( 'Edu', 29, 'Plancha'); "
      "INSERT INTO t7 VALUES ('Cassandra', 47, 'Batidora'), ( 'Lucas', 19, "
      "'Correa'); SELECT ID, Producto FROM t7;";

  std::string input_4 =
      "     CREATE    TABLE  t8    ( ID INT PRIMARY KEY , Edad FLOAT) ; INSERT "
      "INTO t8 VALUES( 1, 1 ); INSERT INTO t8 ( ID, Edad) VALUES( 2, 2 ); "
      "INSERT INTO t8 VALUES (3, 3), ( 4, 4); SELECT ID, Edad FROM t8;";

  std::string input_5 =
      "     CREATE    TABLE  t9    ( ID INT PRIMARY KEY , Edad FLOAT) ; INSERT "
      "INTO t9 VALUES( 11, 11 ); INSERT INTO t9 ( ID, Edad) VALUES( 22, 22 ); "
      "INSERT INTO t9 VALUES (33, 33), ( 44, 44); SELECT ID, Edad FROM t9;";

  std::string input_6 = "  DROP TABLE t6;";

  std::string input_7 = "exit";

  FifoNode* node_1 = new FifoNode;
  node_1->comando = input_1;
  fifo_obj->head = node_1;
  FifoNode* node_2 = new FifoNode;
  node_1->nxt_node = node_2;
  node_2->comando = input_2;
  FifoNode* node_3 = new FifoNode;
  node_2->nxt_node = node_3;
  node_3->comando = input_3;
  FifoNode* node_4 = new FifoNode;
  node_3->nxt_node = node_4;
  node_4->comando = input_4;

  FifoNode* node_5 = new FifoNode;
  node_4->nxt_node = node_5;
  node_5->comando = input_5;

  FifoNode* node_6 = new FifoNode;
  node_5->nxt_node = node_6;
  node_6->comando = input_6;

  FifoNode* node_7 = new FifoNode;
  node_6->nxt_node = node_7;
  node_7->comando = input_7;
  return fifo_obj;
};
