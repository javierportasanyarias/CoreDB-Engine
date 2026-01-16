
// #include "tests_1.h"
#include <string>
#include <iostream>
// Para usar subprocesos:
#include <unistd.h>   // fork, pipe, dup2, execl, read, write, close
#include <sys/wait.h> // waitpid
#include <cstring>    // strlen


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
};


// ==============================================
// == TESTEO CON SUBPROCESOS ====================
// ==============================================
void ejecutar_proceso_hijo(int (&pipe_1)[2], int (&pipe_2)[2], pid_t& pid){

    close(pipe_1[1]);
    close(pipe_2[0]);

    dup2(pipe_1[0], STDIN_FILENO);
    dup2(pipe_2[1], STDOUT_FILENO);

    close(pipe_1[0]);
    close(pipe_2[1]);

    // Ejecitamos el programa:
    execl("./app/sql_app.exe", "app", nullptr);
    // Solo llega si hay error a esto:
    perror("ERROR: ");
    _exit(1);

};


void ejecutar_proceso_padre(int (&pipe_1)[2], int (&pipe_2)[2], pid_t& pid, std::string& comando){

    close(pipe_1[0]);
    close(pipe_2[1]);

    // Escribimos el comando:
    std::string cmd1= comando + "\n";
    write(pipe_1[1], cmd1.c_str(), cmd1.size());

    // Segundo input:
    std::string comando_2 = "    CREATE TABLE  t1    ( ID STRING PRIMARY KEY , Edad INT, Producto STRING) ; INSERT INTO t1 VALUES( 'Carlos', 49, 'Secadora'); INSERT INTO t1 VALUES( 'Andrea', 34, 'Lavadora'); INSERT INTO t1 ( ID, Edad, Producto) VALUES( 'Edu', 29, 'Plancha'); INSERT INTO t1 VALUES ('assandra', 47, 'Batidora'), ( 'Lucas', 19, 'Correa'); SELECT * FROM t1; SELECT ID, Producto FROM t1;  DROP TABLE t1  ;\n";
    std::string output_2 = "";
    write(pipe_1[1], comando_2.c_str(), comando_2.size());
    close(pipe_1[1]);
    // Recibimos el input:                                            
    std::string output_1 = "";                                        char buffer[2560];                                                ssize_t n;                                                                                                                          while ((n = read(pipe_2[0], buffer, sizeof(buffer))) > 0) {
         output_1.append(buffer, n);
        //if (output_1.find(">") != std::string::npos) break; // detectamos fin de salida                                                 
    };                                                                std::cout<<output_1<<std::endl;
    close(pipe_2[0]);

    //waitpid(pid, nullptr, 0);


};


void run_test_subproceso(std::string& query){
    // Declaeamos los pipelines:
    int pipe_hijo_padre[2];
    int pipe_padre_hijo[2];
    pipe(pipe_hijo_padre);
    pipe(pipe_padre_hijo);
    
    // Fork del proceso:
    pid_t pid = fork();

    // Ahora los procesos:
    if(pid == 0){
       // == HIJO ============
       ejecutar_proceso_hijo(pipe_hijo_padre, pipe_padre_hijo, pid);
    } else {
       // == PADRE ===========
       ejecutar_proceso_padre(pipe_hijo_padre, pipe_padre_hijo, pid, query);
    };

    // Esperamos a que el hijo termine:
    waitpid(pid, nullptr, 0);


}; 

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

    // Antiguo metodo:
    //run_test(query);
    // Nuevo metodo:
    run_test_subproceso(query);


    return 0;
};
