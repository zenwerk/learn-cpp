#include <iostream>
#include "node.h"

void node_dump(node& n, int depth) {
  for (int i = 0; i < depth; i++)
    std::cout << "  ";

  switch (n.type) {
  case ND_NUM:
    std::cout << "ND_NUM:" << static_cast<node_int&>(n).val << " | " << &n << std::endl;
    break;
  case ND_BINOP:
    std::cout << "ND_BINOP:" << static_cast<node_binop&>(n).op << " | " << &n << std::endl;
    ++depth;
    node_dump(*static_cast<node_binop&>(n).lhs, depth);
    node_dump(*static_cast<node_binop&>(n).rhs, depth);
    break;
  case ND_UNARY:
    std::cout << "ND_UNARY:" << static_cast<node_unary&>(n).op << " | " << &n << std::endl;
    ++depth;
    node_dump(*static_cast<node_unary&>(n).expr, depth);
    break;
  default:
    std::cerr << "unknown node type" << std::endl;
  }
}