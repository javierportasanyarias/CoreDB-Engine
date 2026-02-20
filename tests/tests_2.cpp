
// #include "tests_2.h"
#include <string>
#include <iostream>
// Para usar subprocesos:
#include <unistd.h>   // fork, pipe, dup2, execl, read, write, close
#include <sys/wait.h> // waitpid
#include <cstring>    // strlen
#include "data_structs.h"
#include "bateria_tests.h"
#include "logging.h"
#include <fcntl.h> // Necesario para fcntl
#include <poll.h>


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

    //close(pipe_1[0]);
    close(pipe_2[1]);

    // Ejecitamos el programa:
    execl("./app/sql_app.exe", "app", nullptr);
    // Solo llega si hay error a esto:
    perror("ERROR: ");
    _exit(1);

};
// FUNCION PARA VER SI LA ESCRITURA HA FUNCIONA
void comprobar_escritura(ssize_t& bytes_msg){
   if(bytes_msg<0){                                                     Logger::log(LogLevel::OUTPUT, "Error del sistema", true, false);                                                                 }else if(bytes_msg >0){
      Logger::log(LogLevel::OUTPUT, "Mensaje escrito con exito", true, false);                                                         }else{                                                               Logger::log(LogLevel::OUTPUT, "No se ha escrito nada", true,
 false);
   };
   Logger::flush();
};
//---------------------------------
// -- FUNCIOBES AUXILARES DEL PADRE:
void write_to_child(int (&pipe_in)[2], const std::string& cmd) {
    std::string s = cmd;
    //std::string s = cmd;
   ssize_t bytes_msg =  write(pipe_in[1], s.c_str(), s.size());
   comprobar_escritura(bytes_msg); 
    usleep(100000); // 0.1 segundos
    char caracter = '\n';
    bytes_msg = write(pipe_in[1], &caracter, 1);
    comprobar_escritura(bytes_msg);
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





std::string read_until_marker_4(int (&pipe_out)[2], const std::string& marker="__END__"){                                                std::string output = "";
    char buf[128];
    // PONER EL PIPE EN MODO NO BLOQUEANTE
    //int flags = fcntl(pipe_out[0], F_GETFL, 0);
    //fcntl(pipe_out[0], F_SETFL, flags | O_NONBLOCK);
    //usleep(5000000); // 0.5 segundos
    struct pollfd fds[1];
    fds[0].fd = pipe_out[0];
    fds[0].events = POLLIN; // Queremos leer
    int ret = poll(fds, 1, 2000);
    if(ret > 0){
       Logger::log(LogLevel::OUTPUT, "RET > 0: EL PIPE HAN TENIDO CAMBIOS", true, false);
    } else if(ret < 0){
       Logger::log(LogLevel::OUTPUT, "RET = 0: TIMEOUT DE RET", true, false);
    } else {
       Logger::log(LogLevel::OUTPUT, "RET < 0: ERROR DEL SISTEMA", true, false);
    };
    Logger::flush();
    ssize_t n;                                                        int aux_count = 0;
    bool cond1 = false;                                               while ((n = read(pipe_out[0], &buf, sizeof(buf))) > 0) {
        output.append(buf, n);
	Logger::log(LogLevel::OUTPUT, "ITERACION DE LECTURA: ", false, false);
	Logger::log(LogLevel::OUTPUT, aux_count, true, false);
	Logger::flush();
	aux_count+= 1;
	size_t posicion = output.find(marker);
        if (posicion != std::string::npos) {
		    break;
	};
	if(aux_count == 100){
           break;
	};

    };
    // Restaurar el modo bloqueante por si acaso
    //fcntl(pipe_out[0], F_SETFL, flags);
    return output;
};


void ejecutar_proceso_padre(int (&pipe_1)[2], int (&pipe_2)[2], pid_t& pid, FIFO*& fifo_obj){

    close(pipe_1[0]);
    close(pipe_2[1]);

    //FifoNode* c_n_ptr = new FifoNode;

    FifoNode* c_n_ptr = fifo_obj->head;

    int c_1 = 0;
    std::string output_1 = "";

    while(c_n_ptr){

       // Recibimos el input:
       //std::cout<<"COMANDO: "<<c_1+1<<std::endl;
       Logger::log(LogLevel::OUTPUT, "COMANDO: ", false, false);
       Logger::log(LogLevel::OUTPUT, c_1+1, true, false);
       Logger::flush();
       //std::cout.flush();
       //std::cout<<"> "<<c_n_ptr->comando<<std::endl;
       Logger::log(LogLevel::OUTPUT, "> ", false, false);
       Logger::log(LogLevel::OUTPUT, c_n_ptr->comando, true, false);
       //std::cout.flush();
       output_1 = "";
       write_to_child(pipe_1, c_n_ptr->comando);
       Logger::log(LogLevel::OUTPUT, "Comando enviado", false, false);
       Logger::flush();
       usleep(5000000); // 0.5 segundos
       output_1 = read_until_marker_4(pipe_2);
       //usleep(100000); // Espera 0.1 segundos
       //std::cout<<"Este es el output: "<<std::endl;
       Logger::log(LogLevel::OUTPUT, "Este es el output: ", true, false);
       Logger::flush();
       //std::cout<<output_1<<std::endl;
       Logger::log(LogLevel::OUTPUT, output_1, true, false);
       Logger::flush();
       // Para sincronizacion con el hijo:
       //char handshake = '\n';
       //write(pipe_1[1], &handshake, 1);
       //write(pipe_1[1], handshake.c_str(), handshake.size());
       c_n_ptr = c_n_ptr->nxt_node;
       //std::cout.flush();
       Logger::flush();
       c_1+=1;
    };

    close(pipe_1[1]);
    close(pipe_2[0]);

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
    waitpid(pid, nullptr, WNOHANG);

}; 

// -----------------------------
// Main del test runner
// -----------------------------

int main()
{
    Logger::level = LogLevel::OUTPUT;
    Logger::log(LogLevel::OUTPUT, "== SQL Engine Test Runner ===", true, false);
    Logger::log(LogLevel::OUTPUT, "Que test quieres realizar? : ", false, false);
    Logger::flush(false);

    std::string input = "";

    std::getline(std::cin, input);

    // Obtenemos la cola del test:
    FIFO* fifo_obj = new FIFO;
    if(input == "test1"){
       fifo_obj = define_test_1();
    } else if(input == "test2") {
       fifo_obj = define_test_2();
    } else if(input == "test3"){
       fifo_obj = define_test_3();
    };

    run_test_subproceso(fifo_obj);
    Logger::flush();


    return 0;
};
