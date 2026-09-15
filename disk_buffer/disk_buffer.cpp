#include "disk_in.h"
#include "disk_buffer.h"


//====================================================
//========= tableRowIterator only disk ===============
//====================================================


// == Constructor method =============================
disk_buffer::tableRowIterator_only_disk_part::tableRowIterator_only_disk_part(const std::string& table_name_str){

   counter = 0;
   table_ptr = global_table_dict.at(table_name_str);
   eof = false;
   
};


// == EOF query ======================================
bool disk_buffer::tableRowIterator_only_disk_part::is_eof() const {

   return eof;
};


// For obtaining the next row ========================
std::map<std::string, Values> disk_buffer::tableRowIterator_only_disk_part::get_next_row_only_disk_part(disk_in::read_table_iterator* table_reader_obj){

   // Perform the read:
   Logger::flush();
   Logger::log(LogLevel::DEBUG, "<<<<<<<< INSIDE DISK ITERATOR >>>>>>>>>>", true, false);
   Logger::flush();
   std::map<std::string, Values> map_row_returned;
   uint32_t n_f_disk = table_reader_obj->current_partition_n_rows;
   Logger::log(LogLevel::DEBUG, "Reading ", false, true);
   Logger::log(LogLevel::DEBUG, n_f_disk, false, false);
   Logger::log(LogLevel::DEBUG, " rows in total", true, false);
   Logger::log(LogLevel::DEBUG, "Counter value is: ", false, true);
   Logger::log(LogLevel::DEBUG, counter, true, false);

   std::vector<std::string>& col_names = table_ptr->metadata_ptr->column_names;
   // Guard conditions, to save checks:
   if(counter >= n_f_disk){
      this->eof = true;
      Logger::log(LogLevel::DEBUG, "Row will return empty ('get_next_row_only_disk_part')");
      return map_row_returned;
   };
   table_data_buffer*& ptr_data = this->table_ptr->data_buffer_ptr;
   if(!ptr_data){
      this->eof = true;
      Logger::log(LogLevel::ERROR, "'data_buffer_ptr' pointer does NOT exist within the table object");
      return map_row_returned;
   }
   std::map<std::string, std::vector<Values>>& columns_map = ptr_data->columns;
   if(columns_map.empty()){
      this->eof = true;
      Logger::log(LogLevel::ERROR, "Column map is empty");
      return map_row_returned;
   };

   // Iterate through each column:

   for (const std::string& col_name : col_names) {

      Logger::log(LogLevel::DEBUG, "Reading from the info retrieved from disk");
      Logger::log(LogLevel::DEBUG, "Counter value: ", false, true);
      Logger::log(LogLevel::DEBUG, counter, true, false);
      Logger::log(LogLevel::DEBUG, "Column name: ", false, true);
      Logger::log(LogLevel::DEBUG, col_name, true, false);

      Logger::log(LogLevel::DEBUG, "We write the value in <<<row to return>>>");
      Logger::log(LogLevel::DEBUG, "Column name: ", false, true);
      Logger::log(LogLevel::DEBUG, col_name, true, false);
      Logger::log(LogLevel::DEBUG, "Counter value: ", false, true);
      Logger::log(LogLevel::DEBUG, counter, true, false);
      map_row_returned[col_name] = columns_map.at(col_name)[counter];
      
      Logger::log(LogLevel::DEBUG, "Row retrieved data read from disk successfully");

   }; // End of column iteration
   counter += 1;
   if(counter >= (n_f_disk)){
      this->eof = true;
   };
   return map_row_returned;
}; // End of 'get_next_row' method*/



//====================================================
//== tableRowIterator only RAM =======================
//====================================================

// == Constructor method =============================
disk_buffer::tableRowIterator_only_ram::tableRowIterator_only_ram(const std::string& table_name_str){
   /*
   Constructor of the 'tableRowIterator_only_ram' class
   */
   counter = 0;
   table_ptr = global_table_dict.at(table_name_str);
   // Only count rows in live RAM if available:

   this->n_rows_total = table_ptr->metadata_ptr->n_rows_ram;
   this->eof = (n_rows_total == 0);
};

// == EOF query ======================================
bool disk_buffer::tableRowIterator_only_ram::is_eof() const {
   /*
   Simple function to return the boolean attribute 'eof'
   */
   return eof;
};

// For obtaining the next row ('RAM') ================
std::map<std::string, Values> disk_buffer::tableRowIterator_only_ram::get_next_row_ram(){
   /*
   Method similar to 'get_next_row' of tableRowIterator. Here, only
   data from the same session or live RAM will be returned.
   Works as follows:
      1. Iterate through each column of the table.
      2. Given the 'counter' attribute, extract the corresponding element from each vector of columns,
         always from the live RAM data.
      3. Once the loop is completed, update the 'eof' attribute.
      4. Increment the row counter by one.
   */
   std::map<std::string, Values> map_row_returned;

   std::vector<std::string>& col_names = table_ptr->metadata_ptr->column_names;
   // Guard conditions, to save checks:
   if(counter >= this->n_rows_total){
      this->eof = true;
      Logger::log(LogLevel::DEBUG, "Row will return empty ('get_next_row_ram')");
      return map_row_returned;
   };
   table_data*& ptr_data = this->table_ptr->data_ptr;
   if(!ptr_data){
      this->eof = true;
      Logger::log(LogLevel::ERROR, "'data_ptr' pointer does NOT exist within the table object");
      return map_row_returned;
   };
   std::map<std::string, std::vector<Values>>& columns_map = ptr_data->columns;
   if(columns_map.empty()){
      this->eof = true;
      Logger::log(LogLevel::ERROR, "Column map is empty");
      return map_row_returned;
   };


   // Iterate through each column:
   for (const std::string& col_name : col_names) {

      // Read from live RAM:
      map_row_returned[col_name] = columns_map.at(col_name)[counter];
   };
   counter += 1;
   if(counter >= this->n_rows_total){
      this->eof = true;
   };
   return map_row_returned;
}; // End of 'get_next_row_ram' method

//====================================================
//========= FSM iterator for disk and RAM ============
//====================================================

// == Constructor method =============================
disk_buffer::tableRowIterator::tableRowIterator(const std::string& table_name_str){
   /*
   Constructor of the 'tableRowIterator' class
   */


   Logger::flush();
   Logger::log(LogLevel::DEBUG, "////////////////////////////////////////////////");
   Logger::log(LogLevel::DEBUG, "FSM iterator (RAM and disk) initialized");

   // Get the table pointer:
   table_ptr = global_table_dict.at(table_name_str);
   first_execution = true;
   eof = false; // This is the general false, only if both are true this will be true
   eof_ram = false;
   eof_partition = true; // Starts being true so that it only reads from disk once
   eof_disk = false;
   fsm_state = 0;
   table_name = this->table_ptr->metadata_ptr->name;

   // Auxiliary iterators:
   this->iterator_ram = nullptr;
   this->table_reader_obj = nullptr;
   this->disk_iterator = nullptr;
   Logger::log(LogLevel::DEBUG, "Variables have been initialized successfully");

};

bool disk_buffer::tableRowIterator::is_eof() const {

   return eof;
};

// == Control Unit method =============================
std::map<std::string, Values> disk_buffer::tableRowIterator::control_unit(){
   /*
   This method is the Control Unit of the Finite State Machine
   that retrieves all rows, whether from the same session in memory
   or from previous sessions that come from disk.
   
   The auxiliary iterators and disk reading are created on the HEAP,
   so the finite state machine itself is responsible for deleting them.
   */
   Logger::log(LogLevel::DEBUG, "CU cycle");
   std::map<std::string, Values> map_row_returned = {};
   switch(this->fsm_state){
      case 0: {
         Logger::log(LogLevel::DEBUG, "State 0. FSM just started");
         Logger::log(LogLevel::DEBUG, "RAM reading");
         // Check if there is data in RAM:
         if(this->table_ptr->data_ptr && !(this->table_ptr->data_ptr->columns.empty())){
            Logger::log(LogLevel::DEBUG, "There is data in the table's 'RAM'");
            Logger::log(LogLevel::DEBUG, "Creating RAM iterator:");
            this->iterator_ram = new disk_buffer::tableRowIterator_only_ram(this->table_name);
            Logger::log(LogLevel::DEBUG, "RAM iterator created successfully");
            this->fsm_state = 1;
            break;
         };
         // In case there is no data in RAM, switch directly to disk reading
         Logger::log(LogLevel::DEBUG, "There is NO data inside the 'RAM'. Switching to 'disk' reading");
         this->eof_ram = true;
         this->fsm_state = 2;
         break;
      };
      case 1: {
         // Row iteration only in RAM
         Logger::log(LogLevel::DEBUG, "State 1. Reading a 'RAM' row:");
         map_row_returned = this->iterator_ram->get_next_row_ram();
         Logger::log(LogLevel::DEBUG, "RAM row retrieved successfully");
         
         // Check if what was returned is empty:
         if(map_row_returned.empty()){
            Logger::log(LogLevel::DEBUG, "RAM row has returned empty.");
            Logger::log(LogLevel::DEBUG, "This means there is no more data left within the table's 'RAM'");
            this->eof_ram = true;
            // Change state:
            this->fsm_state = 2;
            // Delete the RAM iterator:
            delete this->iterator_ram;
            this->iterator_ram = nullptr;
            Logger::log(LogLevel::DEBUG, "RAM iterator deletion completed");
            break;
         };
         this->fsm_state = 1;
         break;
      };
      case 2: {
         // Check beforehand if there is data on DISK:
         Logger::log(LogLevel::DEBUG, "State 2. Starting 'disk' reading");
         Logger::log(LogLevel::DEBUG, "Inspecting beforehand if there is data within the disk");
         if(std::filesystem::exists("data/" + this->table_name)){
            Logger::log(LogLevel::DEBUG, "There is data in the disk");
            this->fsm_state = 3;
            Logger::log(LogLevel::DEBUG, "Proceeding to create iterator for both reading and 'disk'");
            // In that case create the disk read object:
            this->table_reader_obj = new disk_in::read_table_iterator(this->table_ptr);
            // Also initialize the disk iterator:
            this->disk_iterator = new disk_buffer::tableRowIterator_only_disk_part(this->table_name); 

            break;
         };
         Logger::log(LogLevel::DEBUG, "There is NO data within the disk");
         this->eof_disk = true;
         this->eof = true; // Because RAM was already explored
         this->fsm_state = 255;
         break;
      };
      case 3: {
         // Partition reading
         Logger::log(LogLevel::DEBUG, "State 3. Partition reading:");
         // Before reading the partition, 
         this->eof_disk = this->table_reader_obj->read_table();
         Logger::log(LogLevel::DEBUG, "Disk partition read successfully");
         Logger::log(LogLevel::DEBUG, "Restarting disk iterator counter");
         this->disk_iterator->counter = 0;
         Logger::log(LogLevel::DEBUG, "Disk iterator counter will be restarted each time a new partition is read");
         if(this->eof_disk){
            Logger::log(LogLevel::DEBUG, "All partitions have been read (disk EOF has been reached)");
            this->fsm_state = 255; // This means we have reached the end
            this->eof = true; // Because RAM also reached its end or does not exist
            // When reaching the end, delete the read and disk iterators:
            Logger::log(LogLevel::DEBUG, "Deleting reading and disk iterators");
            delete this->table_reader_obj;
            this->table_reader_obj = nullptr;
            delete this->disk_iterator;
            this->disk_iterator = nullptr;
            break;
         };
         this->fsm_state = 4;
         break;
      };
      case 4: {
         // Row iteration only disk
         Logger::log(LogLevel::DEBUG, "State 4. Row reading:");
         map_row_returned =  this->disk_iterator->get_next_row_only_disk_part(table_reader_obj);
         Logger::log(LogLevel::DEBUG, "Row read from disk successfully");
         if(map_row_returned.empty()){
            Logger::log(LogLevel::DEBUG, "Row has returned empty from disk.");
            // End of partition reached:
            this->eof_partition = true;
            // Clear table data from disk:
            this->table_reader_obj->disk_data_ptr->columns.clear();
            // Change state:
            this->fsm_state = 3;
            break;
         };
         this->fsm_state = 4;
         break;
      };
   };
   // Update general eof:
   this->eof = this->eof_ram && this->eof_disk;
   return map_row_returned;
};


// == Method to return the next row =============================
std::map<std::string, Values> disk_buffer::tableRowIterator::get_next_row(){
   /*
   This is the method that executes the finite state machine.
   It advances the state of the machine until it returns
   a row that is not empty or the final state is reached.
   */
   Logger::flush();
   Logger::log(LogLevel::DEBUG, "Another row has been requested");
   std::map<std::string, Values> map_row_returned;

   while(map_row_returned.empty() && this->fsm_state != 255) {
      Logger::log(LogLevel::DEBUG, "Making a Control Unit call");
      map_row_returned = this->control_unit();
   };

   if(this->fsm_state == 255){
      this->eof = true;
      Logger::log(LogLevel::DEBUG, "FSM end condition has been reached");
   };
   return map_row_returned;
};

//====================================================
//== tableRowIterator only WAL (inverse insertion order)
//====================================================


disk_buffer::tableRowIterator_only_ram_for_wal_inverse_order::tableRowIterator_only_ram_for_wal_inverse_order(const std::string& table_name_str, const int num_rows_to_insert){

   /*
   Constructor of the 'tableRowIterator_only_ram_for_wal_inverse_order' class
   */
   this->n_rows_inserted = num_rows_to_insert;

   table_ptr = global_table_dict.at(table_name_str);
   /*
   Do not count rows in live RAM because we will only retrieve
   the last inserted data, and if the insertion was successful,
   those rows are guaranteed to be in live RAM
   */
   this->n_rows_total = table_ptr->metadata_ptr->n_rows_ram;
   this->counter = this->n_rows_total - 1;
   this->lower_limit = this->n_rows_total - this->n_rows_inserted + 1;

   this->eof = (this->counter < 0);
}; // End of 'tableRowIterator_only_ram_for_wal_inverse_order' method

// == EOF query ======================================
bool disk_buffer::tableRowIterator_only_ram_for_wal_inverse_order::is_eof() const {
   /*
   Simple function to return the boolean attribute 'eof'
   */
   Logger::log(LogLevel::DEBUG, "Writing EOF has been reached for rows in WAL file");
   return eof;
};

// For obtaining the last next row ('RAM') ===========
std::map<std::string, Values> disk_buffer::tableRowIterator_only_ram_for_wal_inverse_order::get_next_row_ram(){
   /*
   Method similar to 'get_next_row' of tableRowIterator. Here, only
   data from the same session or live RAM will be returned.
   Works as follows:
      1. Iterate through each column of the table.
      2. Given the 'counter' attribute, extract the corresponding element from each vector of columns,
         always from the live RAM data.
      3. Once the loop is completed, update the 'eof' attribute.
      4. Decrement the row counter by one.
   */
   std::map<std::string, Values> map_row_returned;

   std::vector<std::string>& col_names = table_ptr->metadata_ptr->column_names;
   // Guard conditions, to save checks:
   if(this->counter < this->n_rows_total){
      this->eof = true;
      Logger::log(LogLevel::DEBUG, "Row will return empty ('tableRowIterator_only_ram_for_wal_inverse_order::get_next_row_ram')");
      return map_row_returned;
   };
   table_data*& ptr_data = this->table_ptr->data_ptr;
   if(!ptr_data){
      this->eof = true;
      Logger::log(LogLevel::ERROR, "'data_ptr' pointer does NOT exist within the table object");
      return map_row_returned;
   }
   std::map<std::string, std::vector<Values>>& columns_map = ptr_data->columns;
   if(columns_map.empty()){
      this->eof = true;
      Logger::log(LogLevel::ERROR, "Column map is empty");
      return map_row_returned;
   };

   // Iterate through each column:
   for (const std::string& col_name : col_names) {

      Logger::log(LogLevel::DEBUG, "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
      Logger::log(LogLevel::DEBUG, "Counter: ", false, true);
      Logger::log(LogLevel::DEBUG, this->counter, true, false);
      Logger::log(LogLevel::DEBUG, "n_rows_total: ", false, true);
      Logger::log(LogLevel::DEBUG, this->n_rows_total, true, false);
      Logger::log(LogLevel::DEBUG, "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
      // Read from live RAM:
      Logger::log(LogLevel::DEBUG, "Proceeding to fill the WAL row std::map");
      map_row_returned[col_name] = columns_map.at(col_name)[this->counter];
   };
   // General boundary conditions:
   if(this->counter < 0 || this->counter > this->n_rows_total || this->counter < this->lower_limit){
      this->eof = true;
   };

   this->counter -= 1;
   return map_row_returned;
}; // End of 'get_next_row_ram' method

//====================================================
//== tableRowIterator only WAL =======================
//====================================================


disk_buffer::tableRowIterator_only_ram_for_wal::tableRowIterator_only_ram_for_wal(const std::string& table_name_str, const int num_rows_to_insert){

   /*
   Constructor of the 'tableRowIterator_only_ram_for_wal' class
   */
   this->n_rows_inserted = num_rows_to_insert;

   table_ptr = global_table_dict.at(table_name_str);
   /*
   Do not count rows in live RAM because we will only retrieve
   the last inserted data, and if the insertion was successful,
   those rows are guaranteed to be in live RAM
   */


   this->n_rows_total = table_ptr->metadata_ptr->n_rows_ram;
   this->counter = this->n_rows_total - num_rows_to_insert;

   this->eof = (this->counter >= this->n_rows_total);
}; // End of 'tableRowIterator_only_ram_for_wal' method

// == EOF query ======================================
bool disk_buffer::tableRowIterator_only_ram_for_wal::is_eof() const {
   /*
   Simple function to return the boolean attribute 'eof'
   */
   Logger::log(LogLevel::DEBUG, "Writing EOF has been reached for rows in WAL file");
   return eof;
};

// For obtaining the next row ('RAM') ================
std::map<std::string, Values> disk_buffer::tableRowIterator_only_ram_for_wal::get_next_row_ram(){
   /*
   Method similar to 'get_next_row' of tableRowIterator. Here, only
   data from the same session or live RAM will be returned.
   Works as follows:
      1. Iterate through each column of the table.
      2. Given the 'counter' attribute, extract the corresponding element from each vector of columns,
         always from the live RAM data.
      3. Once the loop is completed, update the 'eof' attribute.
      4. Increment the row counter by one.
   */
   std::map<std::string, Values> map_row_returned;

   std::vector<std::string>& col_names = table_ptr->metadata_ptr->column_names;
   // Guard condition, to save checks:
   if(this->counter >= this->n_rows_total){
      this->eof = true;
      Logger::log(LogLevel::DEBUG, "Row will return empty ('tableRowIterator_only_ram_for_wal::get_next_row_ram')");
      return map_row_returned;
   };
   table_data*& ptr_data = this->table_ptr->data_ptr;
   if(!ptr_data){
      this->eof = true;
      Logger::log(LogLevel::ERROR, "'data_ptr' pointer does NOT exist within the table object");
      return map_row_returned;
   }
   std::map<std::string, std::vector<Values>>& columns_map = ptr_data->columns;
   if(columns_map.empty()){
      this->eof = true;
      Logger::log(LogLevel::ERROR, "Column map is empty");
      return map_row_returned;
   };

   // Iterate through each column:
   for (const std::string& col_name : col_names) {

      Logger::log(LogLevel::DEBUG, "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
      Logger::log(LogLevel::DEBUG, "Counter: ", false, true);
      Logger::log(LogLevel::DEBUG, this->counter, true, false);
      Logger::log(LogLevel::DEBUG, "n_rows_total: ", false, true);
      Logger::log(LogLevel::DEBUG, this->n_rows_total, true, false);
      Logger::log(LogLevel::DEBUG, "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
      // Read from live RAM:
      Logger::log(LogLevel::DEBUG, "Proceeding to fill the WAL row std::map");
      map_row_returned[col_name] = columns_map.at(col_name)[this->counter];
   };
   this->counter += 1;
   // General boundary conditions:
   if(this->counter < 0 || this->counter >= this->n_rows_total){
      this->eof = true;
   };
   return map_row_returned;
}; // End of 'get_next_row_ram' method