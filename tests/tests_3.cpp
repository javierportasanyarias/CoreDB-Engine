
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <string>
// De la veedion V2 del testeo:
#include "data_structs.h"                                                   
#include "bateria_tests.h"
#include "logging.h"

// Vsriables globales:
volatile sig_atomic_t global_pid = -1;
struct sigaction sa;

std::string read_until_marker_4(int (&pipe_out)[2], const std::string& marker="__END__"){
    std::string output = "";
    char buf[1];
    ssize_t n;
    int aux_count = 0;
    bool cond1 = false;
    Logger::log(LogLevel::OUTPUT, "Se ha entrado en lafuncion de lectura", true, false);
    while ((n = read(pipe_out[0], &buf, sizeof(buf))) > 0) {
        output.append(buf, n);
	Logger::log(LogLevel::OUTPUT, "Iteracion de lectura: ", false, false);
	Logger::log(LogLevel::OUTPUT, aux_count+1, true, false);
	Logger::log(LogLevel::OUTPUT, "El output es asi:", true, false);
	Logger::log(LogLevel::OUTPUT, output, true, false);
        Logger::flush();
        aux_count+= 1;
        size_t posicion = output.find(marker);
	size_t posicion2 = output.find(marker+"\n");
        if (posicion != std::string::npos) {
                    break;
	} else if(posicion2 != std::string::npos) {                                             break;                                                          };
        //if(aux_count == 100){
           //break;
        //};

    };
    return output;                                                               };


std::string response_selector(int& counter){

   std::string output = "";
   if(counter ==0){
      output = "uno";
   } else if(counter == 1){
      output = "dos";
   } else{
      output = "exit";
   };
   return output;
};

void manejador_sigaction_hijo(int msg){
   const char* input = "\nHas matado al hijo\n";
   write(STDOUT_FILENO, input, 20);
   //_exit(0);
};


void manejador_sigaction_padre(int msg){
   const char* input = "\nAntes de morir, esperare a que mi hijo muera\n";
   write(STDOUT_FILENO, input, 46);
   // Matamos al hijo:
   kill(global_pid, SIGTERM);
   // Esperamos a que el hijo muera:
   int status;
   waitpid(global_pid, &status, 0);
   const char* input2 = "\nEl hijo ya ha muerto. Puedo morir ahora\n";
   write(STDOUT_FILENO, input2, 41);
   _exit(0);
};
// ==========================================
// == PROCESO HIJO ==========================
// ==========================================
void ejecutar_proceso_hijo(int (&pipe1)[2], int (&pipe2)[2]){
   sa.sa_handler = manejador_sigaction_hijo;
   if(sigaction(SIGINT, &sa, NULL) == -1){
      perror("Error al configurar el sigaction del hijo");                     };
   close(pipe1[1]);
   close(pipe2[0]);
   dup2(pipe1[0], STDIN_FILENO);
   dup2(pipe2[1], STDOUT_FILENO);
   close(pipe1[0]);                                                            close(pipe2[1]);                                                            //execlp("./main.exe", "./main.exe", NULL);
   execl("./app/sql_app.exe", "app", nullptr);
   perror("ERROR: error al ejecutar main.exe");
   _exit(1);
};

//== PROCESO PADRE ============
void ejecutar_proceso_padre(int (&pipe1)[2], int (&pipe2)[2], pid_t& pid, FIFO*& fifo_obj){
   // == El padre: ======================
   FifoNode* c_n_ptr = fifo_obj->head;
                                                                               // Definimos ma funcion de sigaction:
   sa.sa_handler = manejador_sigaction_padre;
   if(sigaction(SIGINT, &sa, NULL) == -1){
      perror("Error al configurar el sigaction del hijo");
   };
   close(pipe1[0]);
   close(pipe2[1]);
   int counter = 0;
   std::string input= "";
   std::string handshake = "handshake\n";
   std::string output = "";
   bool espera_hijo = true;
   int status;
   pid_t resultado = 0;
   global_pid = pid;
   // Bucle principal de interaccion:
   while(espera_hijo){
      resultado = waitpid(pid, &status, WNOHANG);
      if(resultado == 0){
	 Logger::log(LogLevel::OUTPUT, "El hijo sigue vivo", true, false);
         // Aqui escribimos y leemos y enviamos el handshake:
         // Recuperamos el input:
	 input = c_n_ptr->comando;
	 input = input + "\n";
	 Logger::log(LogLevel::OUTPUT, "Comando: ", false, false);
	 Logger::log(LogLevel::OUTPUT, counter+1, true, false);
         Logger::log(LogLevel::OUTPUT, "Input desde el padre:", true, false);
	 Logger::log(LogLevel::OUTPUT, input, true, false);
         Logger::flush();
        // Leemos el output:
	write(pipe1[1], input.c_str(), input.length());
        Logger::flush();
	usleep(500000); // Esperamos 0.5 s
        // Escribimos el handshake:
        write(pipe1[1], handshake.c_str(), handshake.length());
        usleep(500000); // Esperamos 0.5 s
        output = read_until_marker_4(pipe2);
	Logger::log(LogLevel::OUTPUT, "Este es el output: ", true, false);
        Logger::log(LogLevel::OUTPUT, output, true, false);
        usleep(500000); // Esperamos 0.5 s
        Logger::flush();
	// Avanzamos de comando:
        c_n_ptr = c_n_ptr->nxt_node;	
      }else if(resultado == pid){
        std::cout<<"El hijo ha muerto"<<std::endl;
        espera_hijo = false;
      } else{
        perror("Error en el waitpid");
        espera_hijo = false;
      };
        Logger::flush();
        //usleep(5000000); // Esperamos 5 s
        counter += 1;
   };
   close(pipe1[1]);
   close(pipe2[0]);
};
// == TESTEO PROCESOS: ===============

int run_test_subproceso(FIFO*& fifo_obj){
   int pipe1[2]; // Padre-> hijo
   int pipe2[2]; // Hijo -> padre
   if (pipe(pipe1) ==-1 || pipe(pipe2) == -1){                                    perror("Error al crear los pipes");                                         return 1;
   };                                                                          // Definimos parte de sigaction:
   //struct sigaction sa; // Ya definida como global
   sigemptyset(&sa.sa_mask);                                                   sa.sa_flags = SA_RESTART;                                                
   pid_t pid = fork();                                                                                                                                     switch(pid){

   case -1:                                                                       perror("Error del proceso");
      _exit(1);
   case 0:                                                                        // == El hijo: ==========================
      // Denimos la funcion de sigaction:
      ejecutar_proceso_hijo(pipe1, pipe2);
   default:                                                                       // == El padre: =======================
      ejecutar_proceso_padre(pipe1, pipe2, pid, fifo_obj);                            

   };

   return 0;
};

// == BUCLE PRINCIPAL ==========
int main(){

    Logger::level = LogLevel::OUTPUT;
    Logger::log(LogLevel::OUTPUT, "== SQL Engine Test Runner V3 ===", true, false);
    Logger::log(LogLevel::OUTPUT, "Que test quieres realizar? : ", false, false);                                                                           Logger::flush(false);                                                                                                                                   std::string input = "";                                                 
    std::getline(std::cin, input);                                                                                                                          // Obtenemos la cola del test:
    FIFO* fifo_obj = new FIFO;                                                  if(input == "test1"){                                                          fifo_obj = define_test_1();                                              } else if(input == "test2") {
       fifo_obj = define_test_2();
    } else if(input == "test3"){
       fifo_obj = define_test_3();                                              };

    run_test_subproceso(fifo_obj);
    Logger::flush();

                                                                                return 0;
};


