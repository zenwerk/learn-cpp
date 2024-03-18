#include <cstdint>
#include <string>
#include <memory>

enum nd_type {
  ND_NUM, // number
  ND_BINOP, // binary operator
};

struct node {
  nd_type type;
  const std::string fname;
  uint32_t line;
};

struct node_int : node {
  int32_t val;
};

struct node_binop : node {
  std::string op;
  std::shared_ptr<node> lhs;
  std::shared_ptr<node> rhs;
};

void node_dump(node& n);