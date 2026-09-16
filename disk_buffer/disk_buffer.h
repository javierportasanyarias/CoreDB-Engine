#pragma once

#include "disk_in.h"

// Forward declarations:
namespace disk_in {
class read_table_iterator;
};

namespace disk_buffer {

//====================================================
//========= tableRowIterator only RAM ================
//====================================================
class tableRowIterator_only_ram {
  /*
  Similar to the 'tableRowIterator' class, but only iterates over the data added
  in the same session (live RAM).
  It has the following attributes:
     -> counter: to track the row being iterated/retrieved
     -> table_ptr: pointer to the table from which rows are to be obtained
     -> eof: condition indicating if there are no more rows to iterate:
        * If true: the end has been reached and there are no more rows to return
        * If false: there are still one or more rows to iterate
  Upon creation of the class:
     -> The data in live RAM is counted.
     -> The initial value of the eof variable is calculated.
  */
 public:
  uint32_t counter;
  table* table_ptr;
  uint32_t n_rows_total;
  bool eof;

  tableRowIterator_only_ram(const std::string& table_name_str);

  // To check eof:
  bool is_eof() const;

  // == TO GET THE NEXT ROW FROM live RAM:
  std::map<std::string, Values> get_next_row_ram();
};

//====================================================
//========= tableRowIterator only disk ===============
//====================================================

class tableRowIterator_only_disk_part {
 public:
  uint32_t counter;
  table* table_ptr;
  bool eof;

  // Constructor method:
  tableRowIterator_only_disk_part(const std::string& table_name_str);

  // To check eof:
  bool is_eof() const;

  // == TO GET THE NEXT ROW FROM disk:
  std::map<std::string, Values> get_next_row_only_disk_part(
      disk_in::read_table_iterator* table_reader_obj);
};

//====================================================
//========= FSM iterator for disk and RAM ============
//====================================================

class tableRowIterator {
  /*
  Class to iterate and return rows, coming from disk or the current session.
  This class encapsulates a finite state machine, managing thus
  the reading of rows from both RAM and disk partitions.
  It has the following attributes:
     -> table_ptr: pointer to the table from which rows are to be obtained
     -> first_execution: boolean to track if it is the first execution or not
     -> eof: condition indicating if there are no more rows to iterate:
        * If true: the end has been reached and there are no more rows to return
        * If false: there are still one or more rows to iterate
        It includes rows defined in the current session or read from disk.
        It is true if both eof_ram and eof_disk are true.
     -> eof_ram: condition to measure if the end of rows
           defined in the session has been reached.
     -> eof_partition: condition to measure if a partition has been fully read.
     -> eof_disk: condition to measure if the end of rows
        defined in disk data has been reached.
     ->fsm_state: This is the state of the finite state machine, its states are
  as follows:
        * 1:
        * 2:
        * 3:
        * 4:
        * 255:
        This state is stored in an unsigned 1-byte integer, to save memory.
     -> table_name: name of the table
     -> Iterators: These are objects that will allow us to obtain rows from the
  session or disk:
        * iterator_ram: iterator exclusively prepared to return rows defined in
  that same session.
        * table_reader_obj: object that reads and temporarily loads into memory
  the table data from each partition. It retrieves and loads this information
  partition by partition.
        * disk_iterator: iterator that returns rows exclusively retrieved by
  reading from disk.
  */
 public:
  // uint32_t counter;
  table* table_ptr;
  bool first_execution;
  bool eof;
  bool eof_ram;
  bool eof_partition;  // If a partition has been fully read
  bool eof_disk;
  uint8_t fsm_state;
  std::string table_name;

  // Auxiliary iterators:
  tableRowIterator_only_ram* iterator_ram;
  disk_in::read_table_iterator* table_reader_obj;
  tableRowIterator_only_disk_part* disk_iterator;

  // Constructor method
  tableRowIterator(const std::string& table_name_str);

  // To check eof:
  bool is_eof() const;

  // Control unit:
  std::map<std::string, Values> control_unit();

  // To return the next row:
  std::map<std::string, Values> get_next_row();
};

//====================================================
//== tableRowIterator only WAL (inverse insertion order)
//====================================================

class tableRowIterator_only_ram_for_wal_inverse_order {
  /*
  Similar to the 'tableRowIterator' class, but only iterates over the data added
  in a single data insertion operation.
  It will return the last N rows inserted in a data insertion operation
  It has the following attributes:
     -> counter: to track the row being iterated/retrieved.
        In this case, it will start counting from the end to the beginning, to
        retrieve the last N rows.
     -> table_ptr: pointer to the table from which rows are to be obtained
     -> eof: condition indicating if there are no more rows to iterate:
        * If true: the last N rows added in the data insertion operation in live
  RAM have been retrieved.
        * If false: there are still one or more rows to iterate
  Upon creation of the class automatically:
     -> The data in live RAM is counted.
     -> The initial value of the eof variable is calculated.
  */
 public:
  int32_t counter;  // Special case, this can take negative values
  uint32_t n_rows_inserted;
  uint32_t n_rows_total;
  uint32_t lower_limit;
  table* table_ptr;
  bool eof;

  tableRowIterator_only_ram_for_wal_inverse_order(
      const std::string& table_name_str, const int num_rows_to_insert);

  // To check eof:
  bool is_eof() const;

  // == TO GET THE NEXT ROW FROM live RAM:
  std::map<std::string, Values> get_next_row_ram();
};

//====================================================
//== tableRowIterator only WAL =======================
//====================================================

class tableRowIterator_only_ram_for_wal {
  /*
  Similar to the 'tableRowIterator' class, but only iterates over the data added
  in a single data insertion operation.
  It will return the last N rows inserted in a data insertion operation
  It has the following attributes:
     -> counter: to track the row being iterated/retrieved.
        In this case, it will start counting from the end to the beginning, to
        retrieve the last N rows.
     -> table_ptr: pointer to the table from which rows are to be obtained
     -> eof: condition indicating if there are no more rows to iterate:
        * If true: the last N rows added in the data insertion operation in live
  RAM have been retrieved.
        * If false: there are still one or more rows to iterate
  Upon creation of the class automatically:
     -> The data in live RAM is counted.
     -> The initial value of the eof variable is calculated.
  */
 public:
  uint32_t counter;
  uint32_t n_rows_inserted;
  uint32_t n_rows_total;
  table* table_ptr;
  bool eof;

  tableRowIterator_only_ram_for_wal(const std::string& table_name_str,
                                    const int num_rows_to_insert);

  // To check eof:
  bool is_eof() const;

  // == TO GET THE NEXT ROW FROM live RAM:
  std::map<std::string, Values> get_next_row_ram();
};

};  // namespace disk_buffer