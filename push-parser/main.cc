#include <iostream>
#include "lex.h"

int main() {
  std::string in = "(1 + 2) * 3";
  lex_t lex;
  lex_init(lex);

  lex.in = in;

  std::cout << "input: " << in << std::endl;
  while (lex.pos < lex.in.size()) {
    auto token = lex_scan_token(lex);
    lex_log_token(token);
  }

}