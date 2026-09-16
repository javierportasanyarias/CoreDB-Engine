#include "execution_planning.h"

#include "execution.h"
#include "node_for_trees.h"

execPlan::queueNode1::queueNode1()
    : nxt_node_queue(nullptr), prv_node_queue(nullptr){};

execPlan::Queue::Queue() : first_ptr(nullptr), last_ptr(nullptr){};

void execPlan::Queue::add_node_to_queue(queueNode1* node_queue_to_add) {
  if (!node_queue_to_add) return;
  node_queue_to_add->nxt_node_queue = nullptr;
  node_queue_to_add->prv_node_queue = nullptr;

  if (!first_ptr) {
    // Empty queue:
    first_ptr = last_ptr = node_queue_to_add;
  } else {
    // Queue not empty:
    last_ptr->nxt_node_queue = node_queue_to_add;
    node_queue_to_add->prv_node_queue = last_ptr;
    last_ptr = node_queue_to_add;
  };
};

execPlan::queueNode1* execPlan::Queue::pop_front_node() {
  /*
  Method that deletes the queue's first element.
  */
  if (!first_ptr) return nullptr;
  queueNode1* removed = first_ptr;
  if (first_ptr == last_ptr) {
    first_ptr = last_ptr = nullptr;
  } else {
    first_ptr = first_ptr->nxt_node_queue;
    first_ptr->prv_node_queue = nullptr;
  };
  removed->nxt_node_queue = removed->prv_node_queue = nullptr;
  return removed;
};

void execPlan::Queue::printNodeTypes() const {
  /*
  For debug purposes only.
  This method prompts all the queue's nodes in an orderly fashion.
  */
  queueNode1* current = first_ptr;
  int index = 0;

  while (current != nullptr) {
    Logger::log(LogLevel::DEBUG, "Node ", false, true);
    Logger::log(LogLevel::DEBUG, index, false, false);
    Logger::log(LogLevel::DEBUG, ": ", false, false);

    if (std::holds_alternative<NodeType1*>(current->nodePtr)) {
      Logger::log(LogLevel::DEBUG, "NodeType1", true, false);
      const NodeType1* node_pointer = std::get<NodeType1*>(current->nodePtr);
      recursive_tree_print(node_pointer);
    } else if (std::holds_alternative<NodeType2*>(current->nodePtr)) {
      Logger::log(LogLevel::DEBUG, "NodeType2", true, false);
    } else if (std::holds_alternative<NodeType3*>(current->nodePtr)) {
      Logger::log(LogLevel::DEBUG, "NodeType3", true, false);
      const NodeType3* node_pointer = std::get<NodeType3*>(current->nodePtr);
      insert_data_node_print(node_pointer);
    } else if (std::holds_alternative<QueryNode*>(current->nodePtr)) {
      Logger::log(LogLevel::DEBUG, "QueryNode", true, false);
    } else if (std::holds_alternative<DropTableNode*>(current->nodePtr)) {
      Logger::log(LogLevel::DEBUG, "DropTableNode", true, false);
    } else {
      Logger::log(LogLevel::DEBUG, "Node unknown type", true, false);
    };
    Logger::flush(LogLevel::DEBUG);
    current = current->nxt_node_queue;
    index++;
  };
  if (index == 0) {
    Logger::log(LogLevel::DEBUG, "(Queue is empty)\n");
  };
};

void execPlan::aux_delete_queue_node_content(QueryNode*& node) {
  /*
  This node is a special case, as each child is held within a particular
  variable. Those variables are:
      * select_node
      * from_node
  */
  delete node->select_node;
  node->select_node = nullptr;
  delete node->from_node;
  node->from_node = nullptr;

  delete node;
  node = nullptr;
};

void execPlan::aux_delete_queue_node_content(DropTableNode*& node) {
  // This node does not bear any children.
  delete node;
  node = nullptr;
};

void execPlan::aux_delete_queue_node_content(NodeType3*& node) {
  // This node does not bear any children.
  delete node;
  node = nullptr;
};

void execPlan::aux_delete_queue_node_content(NodeType2*& node) {
  // Rcursive node deletion across it's children:
  for (auto& node_child_ptr : node->children) {
    execPlan::aux_delete_queue_node_content(node_child_ptr);
  };
  delete node;
  node = nullptr;
};

void execPlan::aux_delete_queue_node_content(NodeType1*& node) {
  // Rcursive node deletion across it's children:
  for (auto& node_child_ptr : node->children) {
    execPlan::aux_delete_queue_node_content(node_child_ptr);
  };
  // Finally, node deletion:
  delete node;
  node = nullptr;
};

////////////////////////////////////////////////////////////////////////

void execPlan::Queue::delete_current_queue_node(
    queueNode1*& queue_node_to_del) {
  /*
  Auxiliar function for deleting a queue's node:
  */

  if (!queue_node_to_del) return;
  std::visit(
      [](auto* node_ptr) { execPlan::aux_delete_queue_node_content(node_ptr); },
      queue_node_to_del->nodePtr);
  delete queue_node_to_del;
  queue_node_to_del = nullptr;
};

void execPlan::delete_queue_node(queueNode1*& queue_node_to_del) {
  /*
  Auxiliar function for deleting a queue's node:
  */

  if (!queue_node_to_del) return;
  std::visit(
      [](auto* node_ptr) { execPlan::aux_delete_queue_node_content(node_ptr); },
      queue_node_to_del->nodePtr);
  delete queue_node_to_del;
  queue_node_to_del = nullptr;
};

void execPlan::delete_task_queue(execPlan::Queue*& queue) {
  // Safeguard:
  if (!queue) return;

  ////////////////
  queueNode1* current_node = queue->first_ptr;
  while (current_node != nullptr) {
    queueNode1* next_node = current_node->nxt_node_queue;

    // Deleting also node content:
    execPlan::delete_queue_node(current_node);

    delete current_node;
    current_node = next_node;
  };

  queue->first_ptr = nullptr;
  queue->last_ptr = nullptr;

  // Finally, we proceed to queue object deletion:
  delete queue;
  queue = nullptr;
};

void execPlan::delete_whole_task_queue(execPlan::Queue*& queue) {
  /*
  Function that deletes the task queue whole, not
  just it's pointers, but the underlying data structures it
  holds for CRUD operations.
  */
  execPlan::queueNode1* c_q_n = queue->first_ptr;

  // while (c_q_n) {
  // execPlan::delete_queue_node(c_q_n);
  //};

  // Once deleted all the items, we can delete it whole:
  execPlan::delete_task_queue(queue);
};

void execPlan::Queue::execute_queue_tasks() {
  /// Function that executes the queue's tasks

  int index = 0;
  execPlan::queueNode1* c_q_n = first_ptr;

  Logger::log(LogLevel::DEBUG, "Node ", false, true);
  Logger::log(LogLevel::DEBUG, index, false, false);
  Logger::log(LogLevel::DEBUG, ": ", false, false);

  size_t node_type;

  while (c_q_n) {
    node_type = c_q_n->nodePtr.index();
    switch (node_type) {
      case 0: {
        Logger::log(LogLevel::DEBUG, "NodeType1: Table creation execution",
                    true, false);

        NodeType1* node = std::get<NodeType1*>(c_q_n->nodePtr);
        recursive_metadata_fill_lv1(node);
        break;
      };
      case 1: {
        Logger::log(LogLevel::DEBUG,
                    "NodeType2 has been encountered while executing", false,
                    false);
        break;
      };
      case 2: {
        Logger::log(LogLevel::DEBUG, "NodeType3: Data insertion execution",
                    true, false);

        NodeType3* node = std::get<NodeType3*>(c_q_n->nodePtr);

        Logger::log(LogLevel::DEBUG, "NodeType3 retrieved");
        fill_table_with_values_v4(node);
        break;
      };
      case 3: {
        Logger::log(LogLevel::DEBUG, "Query execution: ");

        QueryNode* node = std::get<QueryNode*>(c_q_n->nodePtr);
        show_table_query(node);
        break;
      };
      case 4: {
        DropTableNode* node = std::get<DropTableNode*>(c_q_n->nodePtr);
        drop_table_from_global_dict(node);

        Logger::log(LogLevel::DEBUG,
                    "Table successfully deleted: " + node->table_name);
        // Just in case of any global dict corrupt entry, the map is sanitized:
        sanitize_global_dict();
        break;
      };
      default:
        Logger::log(LogLevel::DEBUG,
                    "Unknown execution node: Unable to execute");
        break;
    };  // Switch statement ends

    Logger::log(LogLevel::DEBUG, "Finished operation", false, true);
    index++;
    Logger::flush(LogLevel::DEBUG);
    execPlan::queueNode1* next_queue_node = c_q_n->nxt_node_queue;
    // Node deletion, once it has been executed:
    execPlan::Queue::delete_current_queue_node(c_q_n);
    c_q_n = next_queue_node;
  };
  if (index == 0) {
    Logger::log(LogLevel::DEBUG, "(Queue is empty)", true, true);
  };

  // Before existing, we update first and last node pointers to null:
  first_ptr = nullptr;
  last_ptr = nullptr;
};
