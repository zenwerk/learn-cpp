#include <iostream>
#include "lex.h"
#include "parse.h"

int main() {
  std::string in = "--1 / 2 - (-3 * 4 + 5)\n";
  lex_t lex;
  lex_init(lex);

  lex.in = in;

  parse_t parse;
  parse_init(parse);

  std::cout << "input: " << in << std::endl;
  while (lex.pos < lex.in.size()) {
    auto token = lex_scan_token(lex);
    //lex_log_token(token);
    parse_push_tok(parse, token);
  }

}