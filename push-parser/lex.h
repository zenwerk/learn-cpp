#ifndef PUSH_PARSER_LEXER_H
#define PUSH_PARSER_LEXER_H

#include <string>
#include <memory>
#include <stack>
#include <utility>


enum TOK_TYPE {
  TOK_EOF = 1,
  TOK_ERR,
  TOK_NUM,
  /* operators */
  TOK_PLUS, // +
  TOK_MINUS, // -
  TOK_SLASH, // /
  TOK_ASTERISK, // *
  TOK_LP, // (
  TOK_RP, // )
  TOK_NL, // \n
};

struct lex_t {
  std::string in; // 解析対象
  std::string filename;
  size_t pos; // 現在の位置
  uint32_t line;
};

struct lex_token_t {
  unsigned int type; // 字句の種類
  std::string lexeme; // 実際の文字
  uint32_t line; // 何行目か
};

void lex_init(lex_t&);
std::string lex_peek(lex_t &lex);
std::string lex_advance(lex_t &lex);
lex_token_t lex_scan_token(lex_t&);
lex_token_t lex_create_token(lex_t&, unsigned int, const std::string&);
lex_token_t lex_create_number_token(lex_t&, std::string&);
lex_token_t lex_create_identifier_token(lex_t&, std::string&);
void log_tk(lex_token_t&);
void lex_log_token(lex_token_t&);


#endif