#include <iostream>
#include <string>
#include "parse.h"

/*
<expression> ::= <term> | <expression> "+" <term> | <expression> "-" <term>
<term> ::= <factor> | <term> "*" <factor> | <term> "/" <factor>
<factor> ::= <number> | "(" <expression> ")"
<number> ::= <digit> | <number> <digit>
<digit> ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
*/

static bool parse_debug = true;

enum PARSE_STATE {
  PARSE_PROG, PARSE_PROG_1,
  PARSE_EXPR, PARSE_EXPR_1,
  PARSE_MUL, PARSE_MUL_1,
  PARSE_PRIMARY, PARSE_PRIMARY_1,
  PARSE_NUMBER, PARSE_NUMBER_1,
  PARSE_DIGIT, PARSE_DIGIT_1,
  PARSE_ENDL,
  PARSE_ERROR,
};

/*
 * name of parse state (debug only)
 */
std::vector<std::string> parse_state_list = {
  "PARSE_PROG", "PARSE_PROG_1",
  "PARSE_EXPR", "PARSE_EXPR_1",
  "PARSE_MUL", "PARSE_MUL_1",
  "PARSE_PRIMARY", "PARSE_PRIMARY_1",
  "PARSE_NUMBER", "PARSE_NUMBER_1",
  "PARSE_DIGIT", "PARSE_DIGIT_1",
  "PARSE_ENDL",
  "PARSE_ERROR",
};


void parse_found(parse_t& p) {
  // TODO: parse を完了したので parse の root にASTが格納されている
  std::cout << "TODO: parse found !!" << std::endl;
}

void parse_begin(parse_t& p, unsigned int new_state) {
  p.stack.push({new_state});
}

void parse_end(parse_t& p) {
  if (!p.stack.empty())
    p.stack.pop();
  else {
    std::cerr << "Error: stack is empty at parse_end." << std::endl;
    exit(1);
  }
}

void parse_set_state(parse_t& p, unsigned int new_state) {
  if (!p.stack.empty())
    p.stack.top().pstate = new_state;
  else {
    std::cerr << "Error: stack is empty at parse_set_state." << std::endl;
    exit(1);
  }
}

void parse_init(parse_t& p) {
  p.root = nullptr;
  p.stack = std::stack<pstate_t>();
  parse_begin(p, PARSE_PROG);
}

void parse_error(parse_t& p, lex_token_t& tok, const std::string& msg) {
  std::cerr << "Error: " << msg << ", got " << tok.lexeme << ", line " << tok.line << std::endl;
  // clear stack
  //p.stack = std::stack<pstate_t>();
  parse_set_state(p, PARSE_ERROR);
}

void parse_push_tok(parse_t& p, lex_token_t& tok) {
  unsigned int tok_type = tok.type;
#define CONSUME do { tok_type = 0; } while(0)
#define CONSUMED (tok_type == 0)

  if (parse_debug)
    log_tk(tok);
  unsigned max_loop = 20;

  for (;;) {
    if (parse_debug && (max_loop-- <= 0)) {
      std::cerr << "ERROR: max_loop" << std::endl;
      exit(1);
    }

    if (parse_debug) {
      for (int i = 1; i < p.stack.size(); i++)
        std::cout << ".  ";
      std::cout << parse_state_list[p.stack.top().pstate] << std::endl;
    }

    auto top = p.stack.top();

    if (tok.type == TOK_ERR) {
      parse_error(p, tok, "error token");
      CONSUME;
    }

    switch (top.pstate) {
    /*
     * prog: expr (";" | "\n")*
     */
    case PARSE_PROG:
      if (CONSUMED || tok_type == TOK_EOF)
        return;
      parse_set_state(p, PARSE_ENDL);
      parse_begin(p, PARSE_EXPR);
      break;
    /*
     * expr: mul ("+" mul | "-" mul)*
     */
    case PARSE_EXPR:
      if (CONSUMED)
        return;
      parse_set_state(p, PARSE_EXPR_1);
      parse_begin(p, PARSE_MUL);
      break;
    case PARSE_EXPR_1:
      if (CONSUMED)
        return;
      if (tok_type == TOK_PLUS) {
        // TODO: create plus node
      } else if (tok_type == TOK_MINUS) {
        // TODO: create minus node
      } else {
        parse_end(p);
        break;
      }
      CONSUME;
      parse_begin(p, PARSE_MUL);
      break;
    /*
     * mul: primary ("*" primary | "/" primary)*
     */
    case PARSE_MUL:
      parse_set_state(p, PARSE_MUL_1);
      parse_begin(p, PARSE_PRIMARY);
      break;
    case PARSE_MUL_1:
      if (CONSUMED)
        return;
      if (tok_type == TOK_ASTERISK) {
        // TODO: create mul node
      } else if (tok_type == TOK_SLASH) {
        // TODO: create div node
      } else {
        parse_end(p);
        break;
      }
      CONSUME;
      parse_begin(p, PARSE_PRIMARY);
      break;
    /*
     * primary: number | "(" expr ")"
     */
    case PARSE_PRIMARY:
      if (CONSUMED)
        return;
      if (tok_type == TOK_NUM) {
        // TODO: create number node
        parse_end(p);
      } else if (tok_type == TOK_LP) {
        parse_set_state(p, PARSE_PRIMARY_1);
        parse_begin(p, PARSE_EXPR);
      } else {
        parse_error(p, tok, "expected number or \"(\"");
        break;
      }
      CONSUME;
      break;
    case PARSE_PRIMARY_1:
      if (CONSUMED)
        return;
      if (tok_type != TOK_RP) {
        parse_error(p, tok, "expected \")\"");
        break;
      }
      // TODO: create expr node
      parse_end(p);
      CONSUME;
      break;
    case PARSE_ENDL:
      if (CONSUMED)
        return;
      if (tok_type == TOK_SEMICOLON || tok_type == TOK_NL) {
        CONSUME;
        parse_found(p);
        parse_end(p);
        break;
      }
      parse_error(p, tok, "expected \";\" or new line");
      break;
    case PARSE_ERROR:
      if (CONSUMED)
        return;
      if (tok_type == TOK_EOF) {
        parse_set_state(p, PARSE_PROG);
        break;
      }
      if (tok_type == TOK_SEMICOLON || tok_type == TOK_NL)
        parse_set_state(p, PARSE_PROG);
      CONSUME;
      break;
    default:
      std::cerr << "Error: unknown state [" << top.pstate << "]" << std::endl;
      exit(1);
    }

  }

}