
// #include "tests_1.h"
#include <string>
#include <iostream>


std::string write_bash_command(std::string& input){

   // std::string command ="cd .. && echo \"" + input + "\" | ./app/sql_app.exe";
   // std::string command ="echo \"" + input + "\" | ./app/sql_app.exe";
   std::string command = "printf \"" + input + "\\nexit\\n\" | ./app/sql_app.exe";
   // std::string command ="cd ..";
   return command;
};



std::string define_test_1(std::string& input){

   std::string str_return = "";
   // std::

   if(input == "default"){

   input = "    CREATE    TABLE  t1    ( ID STRING PRIMARY KEY , Edad INT, Producto STRING) ; INSERT INTO t1 VALUES( 'Carlos', 49, 'Secadora'); INSERT INTO t1 VALUES( 'Andrea', 34, 'Lavadora'); INSERT INTO t1 ( ID, Edad, Producto) VALUES( 'Edu', 29, 'Plancha'); INSERT INTO t1 VALUES ('Cassandra', 47, 'Batidora'), ( 'Lucas', 19, 'Correa'); SELECT * FROM t1; SELECT ID, Producto FROM t1;  DROP TABLE t1  ; ";

   };

   return input;
};


void run_test(std::string& input)
{
    std::cout << "\n=============================\n";
    std::cout << "Running test: "<< "\n";
    std::cout << "=============================\n";

    // Obtenemos le comando:
    std::string command = write_bash_command(input);

    system(command.c_str());

    std::cout << "\n========== END TEST ==========\n";
}

// -----------------------------
// Main del test runner
// -----------------------------

int main()
{
    std::cout << "=== SQL Engine Test Runner ===\n";
    std::cout<< "Que test quieres realizar? : ";

    std::string input = "";

    std::getline(std::cin, input);

    // Obtenemos la string del test:
    std::string query = define_test_1(input);

    run_test(query);


    return 0;
};
