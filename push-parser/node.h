#include <cstdint>
#include <string>
#include <memory>
#include <utility>

enum nd_type {
  ND_NUM = 1, // number
  ND_BINOP, // binary operator
  ND_UNARY, // unary operator
};

struct node {
  nd_type type;
  const std::string fname;
  uint32_t line{};

  node() = default;
  explicit node(nd_type type, std::string fname = "", uint32_t line = 0) : type(type), fname(std::move(fname)), line(line) {}
};

struct node_int : node {
  int32_t val;

  explicit node_int(int32_t val = 0) : node(ND_NUM), val(val) {}
};

struct node_binop : node {
  std::string op;
  node *lhs;
  node *rhs;

  explicit node_binop(std::string op = "", node *lhs = nullptr, node *rhs = nullptr) : node(ND_BINOP), op(std::move(op)), lhs(lhs), rhs(rhs) {}
};

struct node_unary : node {
  std::string op;
  node *expr;

  explicit node_unary(std::string op = "", node *expr = nullptr) : node(ND_UNARY), op(std::move(op)), expr(expr) {}
};

void node_dump(node& n, int depth = 0);