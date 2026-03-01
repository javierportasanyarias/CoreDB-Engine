#include "logging.h"
#include <iostream>
#include <limits>

// Definición del miembro estático
LogLevel Logger::level = LogLevel::INFO;

// Sobrecarga string
void Logger::log(LogLevel msgLevel,
                 const std::string& msg,
                 bool flush_bool,
                 bool flag)
{
    if (msgLevel < level) return;

    if (flag) {
        switch (msgLevel) {
            case LogLevel::DEBUG: std::cout << "[DEBUG] "; break;
            case LogLevel::INFO:  std::cout << "[INFO] ";  break;
            case LogLevel::WARN:  std::cout << "[WARN] ";  break;
            case LogLevel::ERROR: std::cout << "[ERROR] "; break;
            case LogLevel::OUTPUT: break;
        }
    }

    std::cout << msg;
    if (flush_bool) std::cout << std::endl;
}

/* Sobrecarga para char* (Esta sobrecarga la ponemos
   porque si se pone una "..." se 'lia' y no sabe
   usar la de std::string o std::variant)
*/
void Logger::log(LogLevel msgLevel,
		const char* msg,
		bool flush_bool,
		bool flag)
{
    // Simplemente "empujamos" el puntero a la función de string que ya tenías
    log(msgLevel, std::string(msg), flush_bool, flag);
}

// Sobrecarga int
void Logger::log(LogLevel msgLevel,
                 int msg,
                 bool flush_bool,
                 bool flag)
{
    if (msgLevel < level) return;

    if (flag) {
        switch (msgLevel) {
            case LogLevel::DEBUG: std::cout << "[DEBUG] "; break;
            case LogLevel::INFO:  std::cout << "[INFO] ";  break;
            case LogLevel::WARN:  std::cout << "[WARN] ";  break;
            case LogLevel::ERROR: std::cout << "[ERROR] "; break;
            case LogLevel::OUTPUT: break;
        }
    }

    std::cout << msg;
    if (flush_bool) std::cout << std::endl;
}

// Sobrecarga para el std::variant de Values:
void Logger::log(LogLevel msgLevel,                                                          std::variant<int, float, bool, std::string>  msg,
                 bool flush_bool,
                 bool flag)
{
    if (msgLevel < level) return;

    if (flag) {
        switch (msgLevel) {
            case LogLevel::DEBUG: std::cout << "[DEBUG] "; break;
            case LogLevel::INFO:  std::cout << "[INFO] ";  break;
            case LogLevel::WARN:  std::cout << "[WARN] ";  break;
            case LogLevel::ERROR: std::cout << "[ERROR] "; break;                       case LogLevel::OUTPUT: break;
        }                                                                       }

    std::visit([](auto&& arg) {
        std::cout << arg;                                                       }, msg);
    if (flush_bool) std::cout << std::endl;
}



void Logger::flush() {
    std::cout << std::endl;
    std::cout.flush();
};
// Funcion para input:
void Logger::login(std::string& input){
   input = "";
   std::getline(std::cin, input);
};
//Funcion para limpiar buffer de entrada:
void Logger::clear_in() {
    // 1. Resetear los flags de error (failbit, badbit, eofbit)
    // Sin esto, si el pipe falló una vez, no volverá a leer nunca.
    std::cin.clear();

    // 2. Comprobar si hay bytes físicos esperando en el buffer
    std::streamsize bytes_pendientes = std::cin.rdbuf()->in_avail();

    if (bytes_pendientes > 0) {
        // 3. Descartar todo lo que haya hasta el próximo salto de línea
        // Usamos bytes_pendientes para no bloquearnos esperando datos que no existen
        std::cin.ignore(bytes_pendientes, '\n');
    };
};

