#ifndef PUSH_PARSER_PARSE_H
#define PUSH_PARSER_PARSE_H

#include <memory>
#include <stack>
#include "lex.h"
#include "node.h"

struct node;

struct pstate_t {
  unsigned pstate;
  std::shared_ptr<node> pnode;
};

struct parse_t {
  std::stack<pstate_t> stack;
  std::shared_ptr<node> root;
};

void parse_init(parse_t& parse);
void parse_push_tok(parse_t& p, lex_token_t& tok);


#endif