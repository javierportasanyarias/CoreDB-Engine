#include "disk_io.h"
#include "disk_metadata.h"
#include "disk_wal_read.h"
#include "execution_planning.h"
#include "process_tokens.h"
#include "text_utils.h"

int main() {
  /////////////////////////////////////////////////////////////////
  // EXECUTION LOOP:

  std::string input = "";
  std::string handshake = "";

  // Setting logging level:
  Logger::level = LogLevel::OUTPUT;
  /*
  All log levels:
     * DEBUG: Messages for debugging purposes and code analysis.
     * INFO: General information.
     * WARN: Messages for non-fatal issues or deprecated instructions or
  processes.
     * ERROR: Error messages, giving further information about program
  termination or failure to deliver certain tasks.
     * OUTPUT: Output messages. Such as tables or queries visualization.
  */

  // Reding all table's metadata within disk beforehand:
  disk_metadata::read_all_tables_metadata();

  // Reading WAL data (if there is any):
  disk_wal_read::read_wal();

  while (true) {
    // Main program loop

    Logger::log(LogLevel::OUTPUT, "\n> ", false, false);
    Logger::login(input);
    Logger::flush();

    // Exit condition:
    if (input == "exit" || input == "exit\n") {
      break;
    };

    textUtils::simpleLinkedList* token_list = nullptr;
    execPlan::Queue* excec_queue = nullptr;

    try {
      // Input text processing (token list construction):
      token_list = textUtils::process_text_pipeline(input);
      Logger::flush(LogLevel::DEBUG);
      {
        std::string tmp_str = "EOS";
        token_list->add_node(tmp_str);
      };

      // Constructing execution queue;
      excec_queue = process_token_list(token_list);
      // We no longer will be needing the token list, thus we erase it:
      delete token_list;
      token_list = nullptr;

      if (Logger::level == LogLevel::DEBUG) {
        excec_queue->printNodeTypes();
      };

      // Carrying execution queue;
      excec_queue->execute_queue_tasks();
      // Deleting execution queue;
      execPlan::delete_task_queue(excec_queue);

      if (Logger::level == LogLevel::DEBUG) {
        Logger::log(LogLevel::DEBUG, "handshake: ", false, true);
        Logger::login(handshake);
      };
      Logger::flush(LogLevel::DEBUG);
      // Only printing '__END__' if DEBUG logging mode is active
      Logger::log(LogLevel::DEBUG, "__END__", true, false);

    } catch (const std::exception& e) {
      Logger::log(LogLevel::ERROR, "Captured exception: ", false, true);
      Logger::log(LogLevel::ERROR, e.what(), true, false);
      Logger::log(LogLevel::ERROR, "ERROR: Program shutdown due to error", true,
                  false);

      if (token_list != nullptr) {
        delete token_list;
        token_list = nullptr;
      };

      if (excec_queue != nullptr) {
        execPlan::delete_task_queue(excec_queue);
        excec_queue = nullptr;
      };

      // Global table dictionary cleanup:
      clear_global_table_dict();

      // Flushing output buffer:
      Logger::flush(LogLevel::ERROR, true);

      // Flushing inputs (ignoring all pending input characters):
      Logger::flush_input(LogLevel::ERROR, true);

      // std::abort();
      return 0;  // Normal exist, not a program abortion in order to enable
                 // better memory managment
    };

  };  // Main loop end

  // Outside the main loop, program will be soon be terminated

  // Writing all table data defined within session:
  disk_io::write_dump();
  // Only printing '__END__' if DEBUG logging mode is active
  Logger::log(LogLevel::DEBUG, "__END__", true, false);

  // Global table dictionary cleanup:
  clear_global_table_dict();

  return 0;
};  // Main program end
