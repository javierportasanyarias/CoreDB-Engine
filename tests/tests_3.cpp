
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>
#include <string>

// From V2 testing edtition:
#include "bateria_tests.h"
#include "data_structs.h"
#include "logging.h"

// Global variables:
volatile sig_atomic_t global_pid = -1;
struct sigaction sa;

std::string read_until_marker_4(int (&pipe_out)[2],
                                const std::string& marker = "__END__") {
  std::string output = "";
  char buf[1];
  ssize_t n;
  int aux_count = 0;
  bool cond1 = false;
  TestLogger::log(TestlogLevel::OUTPUT, "Se ha entrado en lafuncion de lectura",
                  true, false);
  while ((n = read(pipe_out[0], &buf, sizeof(buf))) > 0) {
    output.append(buf, n);
    // TestLogger::log(TestlogLevel::OUTPUT, "Iteracion de lectura: ", false,
    // false); TestLogger::log(TestlogLevel::OUTPUT, aux_count+1, true, false);
    // TestLogger::log(TestlogLevel::OUTPUT, "El output es asi:", true, false);
    // TestLogger::log(TestlogLevel::OUTPUT, output, true, false);
    // TestLogger::flush();
    aux_count += 1;
    size_t posicion = output.find(marker);
    size_t posicion2 = output.find(marker + "\n");
    if (posicion != std::string::npos) {
      break;
    } else if (posicion2 != std::string::npos) {
      break;
    };
    // if(aux_count == 100){
    // break;
    //};
  };
  return output;
};

void sigaction_child_manager(int msg) {
  const char* input = "\nHas matado al hijo\n";
  write(STDOUT_FILENO, input, 20);
  //_exit(0);
};

void sigaction_parent_manager(int msg) {
  const char* input = "\nAntes de morir, esperare a que mi hijo muera\n";
  write(STDOUT_FILENO, input, 46);
  // Killing the child:
  kill(global_pid, SIGTERM);
  // Waiting for child to die:
  int status;
  waitpid(global_pid, &status, 0);
  const char* input2 = "\nEl hijo ya ha muerto. Puedo morir ahora\n";
  write(STDOUT_FILENO, input2, 41);
  _exit(0);
};

// ==========================================
// == CHILD PROCESS =========================
// ==========================================

void execute_child_process(int (&pipe1)[2], int (&pipe2)[2]) {
  sa.sa_handler = sigaction_child_manager;
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("Error al configurar el sigaction del hijo");
  };
  close(pipe1[1]);
  close(pipe2[0]);
  dup2(pipe1[0], STDIN_FILENO);
  dup2(pipe2[1], STDOUT_FILENO);
  close(pipe1[0]);
  close(pipe2[1]);
  // execlp("./main.exe", "./main.exe", NULL);
  execl("./app/sql_app.exe", "app", nullptr);
  perror("ERROR: error al ejecutar main.exe");
  _exit(1);
};

//== PARENT PROCESS ===========
void execute_father_process(int (&pipe1)[2], int (&pipe2)[2], pid_t& pid,
                            FIFO*& fifo_obj) {
  //== PARENT ===================
  FifoNode* c_n_ptr = fifo_obj->head;

  // Defining sigaction handler function:
  sa.sa_handler = sigaction_parent_manager;
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("Error al configurar el sigaction del hijo");
  };
  close(pipe1[0]);
  close(pipe2[1]);
  int counter = 0;
  std::string input = "";
  std::string handshake = "handshake\n";
  std::string output = "";
  bool espera_hijo = true;
  int status;
  pid_t resultado = 0;
  global_pid = pid;
  // Processes interaction main loop
  while (espera_hijo) {
    resultado = waitpid(pid, &status, WNOHANG);
    if (resultado == 0) {
      TestLogger::log(TestlogLevel::OUTPUT, "El hijo sigue vivo", true, false);
      // Here we write, read and send the handshake.
      // Retrieving the input:
      input = c_n_ptr->comando;
      input = input + "\n";
      TestLogger::log(TestlogLevel::OUTPUT, "Comando: ", false, false);
      TestLogger::log(TestlogLevel::OUTPUT, counter + 1, true, false);
      TestLogger::log(TestlogLevel::OUTPUT, "Input desde el padre:", true,
                      false);
      TestLogger::log(TestlogLevel::OUTPUT, input, true, false);
      TestLogger::flush();
      // Reding output:
      write(pipe1[1], input.c_str(), input.length());
      TestLogger::flush();
      usleep(500000);  // Waiting for 0.5s
      if (input != "exit" && input != "exit\n") {
        // Writing/sending handshake:
        write(pipe1[1], handshake.c_str(), handshake.length());
        usleep(500000);  // Waiting for 0.5s
      };
      output = read_until_marker_4(pipe2);
      TestLogger::log(TestlogLevel::OUTPUT, "Este es el output: ", true, false);
      TestLogger::log(TestlogLevel::OUTPUT, output, true, false);
      usleep(500000);  // Waiting for 0.5s
      TestLogger::flush();
      // Advancing to the next test SQL command:
      c_n_ptr = c_n_ptr->nxt_node;
    } else if (resultado == pid) {
      std::cout << "El hijo ha muerto" << std::endl;
      espera_hijo = false;
    } else {
      perror("Error en el waitpid");
      espera_hijo = false;
    };
    TestLogger::flush();
    // usleep(5000000); // Waiting for 5s
    counter += 1;
  };
  close(pipe1[1]);
  close(pipe2[0]);
};

//== PROCESSES TESTS ==========
int run_test_subprocess(FIFO*& fifo_obj) {
  int pipe1[2];  // Parent-> Child
  int pipe2[2];  // Child -> Parent
  if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
    perror("Error al crear los pipes");
    return 1;
  };

  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  pid_t pid = fork();
  switch (pid) {
    case -1:
      perror("Error del proceso");
      _exit(1);
    case 0:
      // == Child: ============================
      // Defining sigaction handler function:
      execute_child_process(pipe1, pipe2);
    default:
      // == Parent: =========================
      execute_father_process(pipe1, pipe2, pid, fifo_obj);
  };

  return 0;
};

// == MAIN LOOP ================
int main() {
  TestLogger::level = TestlogLevel::OUTPUT;
  TestLogger::log(TestlogLevel::OUTPUT,
                  "== SQL Engine Test Runner V3 ===", true, false);
  TestLogger::log(TestlogLevel::OUTPUT, "Que test quieres realizar? : ", false,
                  false);
  TestLogger::flush(false);
  std::string input = "";
  std::getline(std::cin, input);
  // Obtaining test FIFO queue object:
  FIFO* fifo_obj = new FIFO;
  if (input == "test1") {
    fifo_obj = define_test_1();
  } else if (input == "test2") {
    fifo_obj = define_test_2();
  } else if (input == "test3") {
    fifo_obj = define_test_3();
  } else if (input == "test4") {
    fifo_obj = define_test_4();
  } else if (input == "test5") {
    fifo_obj = define_test_5();
  } else if (input == "test6") {
    fifo_obj = define_test_6();
  } else if (input == "test7") {
    fifo_obj = define_test_7();
  };

  run_test_subprocess(fifo_obj);
  TestLogger::flush();

  return 0;
};
