#pragma once

namespace textUtils {
std::string space_text(const std::string& input);

// Node for token simple linked list:
class NodeList1 {
 public:
  std::string val;
  NodeList1* nxt_node;
  NodeList1() : nxt_node(nullptr), val(""){};
};

class simpleLinkedList {
  /*
  Simple linked list, but with a tail pointer so it does not have to be
  traversed in each node addition, or while quiering current last node.
  */

 public:
  NodeList1* head;
  NodeList1* tail;

  simpleLinkedList() : head(nullptr), tail(nullptr){};

  ~simpleLinkedList() { clear(); };

  void add_node(const std::string& value);

  void print_list();

  void clear();
};

std::string normalize_spaces(const std::string& input);

simpleLinkedList* create_token_list(const std::string& input);

simpleLinkedList* process_text_pipeline(std::string& input);
};  // namespace textUtils
