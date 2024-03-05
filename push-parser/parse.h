#ifndef PUSH_PARSER_PARSE_H
#define PUSH_PARSER_PARSE_H

#include <stack>
#include "lex.h"

struct node;

struct pstate_t {
  unsigned pstate;
  //struct node **pnode;
};

struct parse_t {
  std::stack<pstate_t> stack;
  node *root;
};

void parse_init(parse_t& parse);
void parse_push_tok(parse_t& p, lex_token_t& tok);


#endif