#include <iostream>
#include "node.h"

void node_dump(node& n) {
  switch (n.type) {
    case ND_NUM:
      std::cout << "ND_NUM: " << n.fname << ":" << n.line << " " << static_cast<node_int&>(n).val << std::endl;
      break;
    case ND_BINOP:
      std::cout << "ND_BINOP: " << n.fname << ":" << n.line << " " << static_cast<node_binop&>(n).op << std::endl;
      node_dump(*static_cast<node_binop&>(n).lhs);
      node_dump(*static_cast<node_binop&>(n).rhs);
      break;
  }
}