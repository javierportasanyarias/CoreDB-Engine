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
        };
    };

    std::cout << msg;
    if (flush_bool) std::cout << std::endl;
};

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
};

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
        };
    };

    std::cout << msg;
    if (flush_bool) std::cout << std::endl;
};

// Sobrecarga para el std::variant de Values:
void Logger::log(LogLevel msgLevel,                                                          
                 std::variant<int, float, bool, std::string, std::vector<char>>  msg,
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
        };                                                                       
    };

    //std::visit([](auto&& arg) {
        //std::cout << arg;                                                       
    //}, msg);
    std::visit(
        // Función lambda:
        [](const auto& arg){
            using type = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<type, std::vector<char>>){
                // Caso vector de caracteres:
                for (char c : arg){
                    std::cout << c;
                };
                std::cout.flush();
            } else if constexpr (std::is_same_v<type, bool>){
                // Hacemos más expresiva la muestra de booleanos:
                if(arg){
                    std::cout << "TRUE";
                }else{
                    std::cout << "FALSE";
                };
            }else{
                std::cout << arg;
            };
        },
        msg
    );
    if (flush_bool) std::cout << std::endl;
};

// Sobrecarga para std::variant constante y pasado por referencia:
/*void Logger::log(LogLevel msgLevel,                                                          
                 const std::variant<int, float, bool, std::string, std::vector<char>>& msg,
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
        };                                                                       
    };

    std::visit(
        [](const auto& arg){
            using type = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<type, std::vector<char>>){
                for (char c : arg){
                    std::cout << c;
                };
            } else if constexpr (std::is_same_v<type, bool>){
                if(arg){
                    std::cout << "TRUE";
                }else{
                    std::cout << "FALSE";
                };
            }else{
                std::cout << arg;
            };
        },
        msg
    );
    if (flush_bool) std::cout << std::endl;
};*/


// character buffer logger:
void Logger::log_buffer(LogLevel msgLevel,
                        const char* buffer,
                        uint32_t buffer_len,
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
        };
    };

    // We proceed to print the buffer both as raw hexadecimal and converted to alphanumeric characters
    // Main purpose of this logger is to help debug string buffering in I/O operations.

    std::stringstream ss_hex; // Raw hexadecimal
    std::stringstream ss_txt; // Alphanumeric characters
    
    // Ecabezado del texto:
    ss_hex << "[DUMP HEX] ";
    ss_txt << "[DUMP TXT] ";

    // Raw hexadecimal configuration:
    ss_hex << std::setfill('0') << std::hex;

    for (uint32_t i = 0; i < buffer_len; ++i){

        // Raw hexadecimal processing:
        uint8_t byte = static_cast<uint8_t>(buffer[i]);
        ss_hex << std::setw(2) << (int)byte << " ";

        // Alphanumeric characters processing:
        // We only take into account ASCII alphanumeric characters, leaving out control
        if (byte >= 32 && byte <= 126) {
            ss_txt << (char)byte << "  "; // Añadimos espacios para alinear visualmente con el hex
        } else {
            ss_txt << ".  "; // Un punto para metadatos o bytes nulos (\00)
        }
    };
    // Once finished, we print both stringstreams' content:
    std::cout << ss_hex.str();
    if (flush_bool) std::cout << std::endl;
    std::cout << ss_txt.str();
    if (flush_bool) std::cout << std::endl;
};



void Logger::flush(bool endl) {
    if(endl){
       std::cout << std::endl;
    };
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

