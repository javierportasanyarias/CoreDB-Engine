
// #include "tests_2.h"
#include <string>
#include <iostream>
// Para usar subprocesos:
#include <unistd.h>   // fork, pipe, dup2, execl, read, write, close
#include <sys/wait.h> // waitpid
#include <cstring>    // strlen
#include "data_structs.h"
#include "bateria_tests.h"


// ==============================================
// == TESTEO CON SUBPROCESOS ====================
// ==============================================
//---------------------------------
// -- FUNCION EJECUTAR UN PROCESO HIJO:
void ejecutar_proceso_hijo(int (&pipe_1)[2], int (&pipe_2)[2], pid_t& pid){

    close(pipe_1[1]);
    close(pipe_2[0]);

    dup2(pipe_1[0], STDIN_FILENO);
    close(pipe_1[0]);
    dup2(pipe_2[1], STDOUT_FILENO);

    close(pipe_2[1]);

    // Ejecitamos el programa:
    execl("./app/sql_app.exe", "app", nullptr);
    // Solo llega si hay error a esto:
    perror("ERROR: ");
    _exit(1);

};

//---------------------------------
// -- FUNCIOBES AUXILARES DEL PADRE:
void write_to_child(int (&pipe_in)[2], const std::string& cmd) {
    std::string s = cmd + "\n";
    write(pipe_in[1], s.c_str(), s.size());
};

std::string read_until_marker(int (&pipe_out)[2], const std::string& marker="__END__"){ 
    std::string output;
    char buf[128];
    ssize_t n;
    int aux_count = 0;
    bool cond1 = true;
    while ((n = read(pipe_out[0], buf, sizeof(buf))) > 0) {
    //while(cond1) {
        output.append(buf, n);
	aux_count+= 1;
        //std::cout<<aux_count<<output<<std::endl;
	//if(aux_count==100){
           //break;
	//};
        //if (output.find(marker) != std::string::npos) break;
    }
    // Opcional: quitar el marcador del output
    size_t pos = output.find(marker);
    if (pos != std::string::npos) output.erase(pos);
    return output;
};

bool find_string(std::string& cadena, const std::string& obj){
   size_t len_cad= cadena.length();
   size_t len_obj = obj.length();
   int aux_c1 = 0;
   bool aux_b1 = false;
   std::string buffer = "";

   for(int i=0; i<len_cad; i++){
      if(cadena[i] == obj[aux_c1] && !aux_b1){
         aux_b1 = true;
      };
      if(aux_b1){
         if(cadena[i] == obj[aux_c1]){
            buffer += cadena[i];
	    aux_c1 += 1;
	 } else {
            aux_b1 = false;
	    aux_c1 = 0;
	 };
      };

   };

   // Condicion final:
   if(buffer == obj){
      return true;
   }else {
      return false;
   };


};


std::string read_until_marker_2(
    int (&pipe_out)[2],
    const std::string marker = "Carlos"
) {
    std::string output = "";
    char buf[16348];
    ssize_t n;
    bool cond1 = true;
    bool cond2 = false;
    int aux_c = 0;

    while (cond1) {
	n = read(pipe_out[0], buf, sizeof(buf));
        output.append(buf, n);
	//std::cout<<output<<std::endl;
	//std::cout<<std::endl;
	//std::cout<<"---------------------------------"<<std::endl;
	//std::cout<<std::endl;
	aux_c += 1;

	if(aux_c == 1000){
           break;
        };
	
    };

    cond2 = find_string(output, marker);
        std::cout<<"=========================================="<<std::endl;
	std::cout<<"OUTPUT:"<<std::endl;
	std::cout<<output<<std::endl;
	std::cout<<std::endl;
	std::cout.flush();
	if(cond2){
           std::cout<<std::endl;
	   std::cout<<"++++++++++++++++++++++++++"<<std::endl;
           std::cout<<"Hemos encontrado: "<<marker<<std::endl;
	   std::cout<<"++++++++++++++++++++++++++"<<std::endl;
	   std::cout<<std::endl;
	   std::cout.flush();
	}else {
           std::cout<<std::endl;
           std::cout<<"++++++++++++++++++++++++++"<<std::endl;
           std::cout<<"NO se ha encontrado: "<<marker<<std::endl;
           std::cout<<"++++++++++++++++++++++++++"<<std::endl;
           std::cout<<std::endl;                                                       std::cout.flush();
	};
    std::cout<<"=========================================="<<std::endl;

    // eliminar marcador si existe
    //size_t pos = output.find(marker);
    //if (pos != std::string::npos) {
        //output.erase(pos);
    //};

    return output;
};


void ejecutar_proceso_padre(int (&pipe_1)[2], int (&pipe_2)[2], pid_t& pid, FIFO*& fifo_obj){

    close(pipe_1[0]);
    close(pipe_2[1]);

    FifoNode* c_n_ptr = new FifoNode;

    c_n_ptr = fifo_obj->head;

    int c_1 = 0;
    std::string output_1 = "";
    std::cout.flush();

    while(c_n_ptr){

       // Recibimos el input:
       std::cout<<"COMANDO: "<<c_1+1<<std::endl;
       std::cout.flush();
       //std::cout<<"> "<<c_n_ptr->comando<<std::endl;
       std::cout.flush();
       output_1 = "";
       write_to_child(pipe_1, c_n_ptr->comando);
       output_1 = read_until_marker_2(pipe_2);
       //std::cout<<"Este es el output: "<<std::endl;
       //std::cout<<output_1<<std::endl;
       c_n_ptr = c_n_ptr->nxt_node;
       std::cout.flush();
       c_1+=1;
    };

    close(pipe_1[1]);
    close(pipe_2[0]);
    std::cout.flush();

};


void run_test_subproceso(FIFO*& fifo_obj){
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
       ejecutar_proceso_padre(pipe_hijo_padre, pipe_padre_hijo, pid, fifo_obj);
    };

    // Esperamos a que el hijo termine:
    waitpid(pid, nullptr, 0);

    std::cout.flush();


}; 

// -----------------------------
// Main del test runner
// -----------------------------

int main()
{
    std::cout << "=== SQL Engine Test Runner ===\n";
    std::cout<< "Que test quieres realizar? : ";
    std::cout.flush();

    std::string input = "";

    std::getline(std::cin, input);

    // Obtenemos la cola del test:
    FIFO* fifo_obj = new FIFO;
    if(input == "test1"){
       fifo_obj = define_test_1();
    } else if(input == "test2") {
       fifo_obj = define_test_2();
    };

    // Antiguo metodo:
    //run_test(query);
    // Nuevo metodo:
    run_test_subproceso(fifo_obj);
    std::cout.flush();


    return 0;
};
