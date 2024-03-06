#include <iostream>
#include <regex>
#include "lex.h"


static const std::regex digit(R"(\d+)");
static const std::regex alpha("[_a-zA-Z]+");

static bool is_digit(const std::string &in) {
  return std::regex_match(in, digit);
}

static bool is_alpha(const std::string &in) {
  return std::regex_match(in, alpha);
}

struct tokname_t {
  unsigned int type;
  std::string str;
};

auto lex_token_list = std::vector<tokname_t>{
  {TOK_PLUS, "+"},
  {TOK_MINUS, "-"},
  {TOK_SLASH, "/"},
  {TOK_ASTERISK, "*"},
  {TOK_LP, "("},
  {TOK_RP, ")"},
  {TOK_NL, "\\n"},
  {TOK_NUM, "NUMBER"},
  {TOK_EOF, "EOF"},
  {TOK_ERR, "ERROR"},
};

std::vector<std::string> lex_type_list = {
  "TOK_EOF",
  "TOK_ERR",
  "TOK_NUM",
  "TOK_PLUS",
  "TOK_MINUS",
  "TOK_SLASH",
  "TOK_ASTERISK",
  "TOK_LP",
  "TOK_RP",
  "TOK_NL",
  "TOK_SEMICOLON",
};

void log_tk(lex_token_t& tk) {
  std::cout << lex_type_list[tk.type-1] << ":\"" << tk.lexeme << "\"" << std::endl;
}

void lex_log_token(lex_token_t& tk) {
  for (auto &t : lex_token_list) {
    if (t.type == tk.type) {
      if (t.type == TOK_NUM) {
        std::cout << "token: " << tk.lexeme << ", line " << tk.line << std::endl;
      } else {
        std::cout << "token: " << t.str << ", line " << tk.line << std::endl;
      }
      return;
    }
  }
  std::cout << "unknown token: " << tk.lexeme << ", line " << tk.line << std::endl;
}

void lex_init(lex_t& lex) {
  lex.in = "";
  lex.filename = "";
  lex.pos = 0;
  lex.line = 1;
}

//void print_ch(const std::string& ch, int32_t line) {
//  if (ch == " ") return;
//  std::cout << "scan_token: ";
//  if (ch == "\n") std::cout << "\\n";
//  else if (ch == "\t") std::cout << "\\t";
//  else if (ch == "\r") std::cout << "\\r";
//  else if (ch.empty()) std::cout << "EOF";
//  else std::cout << ch;
//
//  std::cout << " -- Line:" << line << std::endl;
//}

lex_token_t lex_create_token(lex_t& lex, tok_type type, const std::string& lexeme) {
  return lex_token_t{type, lexeme, lex.line};
}

// create_number_token は数値トークンを読み込みレキサーに設定する
lex_token_t lex_create_number_token(lex_t& lex, std::string &tk) {
  while (is_digit(lex_peek(lex))) {
    tk += lex_advance(lex);
  }
  return lex_token_t{TOK_NUM, tk, lex.line};
}

//Token lex_create_identifier_token(lex_t& lex, std::string &tk) {
//  while (is_digit(lex_peek(lex)) || is_alpha(lex_peek(lex))) {
//    tk += lex_advance(lex);
//  }
//  // 予約後かチェックする
//  if (tk == "return") return Token{TokenType::RETURN, tk, line};
//  else if (tk == "if") return Token{TokenType::IF, tk, line};
//  else if (tk == "else") return Token{TokenType::ELSE, tk, line};
//  else if (tk == "for") return Token{TokenType::FOR, tk, line};
//  else if (tk == "def") return Token{TokenType::DEF, tk, line};
//  else if (tk == "dev") return Token{TokenType::DEV, tk, line};
//  else if (tk == "in") return Token{TokenType::IN, tk, line};
//  else if (tk == "out") return Token{TokenType::OUT, tk, line};
//
//  return Token{TokenType::IDENT, tk, line};
//}

// scan_token は1文字読み込み字句を解析する
lex_token_t lex_scan_token(lex_t& lex) {
  // TODO: 先に this->state.top() でレキサーの状態を確認

  while (true) {
    std::string ch = lex_advance(lex);
    //print_ch(ch, line);
    if (ch == " " || ch == "\t" || ch == "\r") continue;
    else if (ch == "(") return lex_create_token(lex, TOK_LP, ch);
    else if (ch == ")") return lex_create_token(lex, TOK_RP, ch);
    else if (ch == "+") return lex_create_token(lex, TOK_PLUS, ch);
    else if (ch == "-") return lex_create_token(lex, TOK_MINUS, ch);
    else if (ch == "*") return lex_create_token(lex, TOK_ASTERISK, ch);
    else if (ch == "/") return lex_create_token(lex, TOK_SLASH, ch);
    else if (ch == ";") return lex_create_token(lex, TOK_SEMICOLON, ch);
    else if (is_digit(ch)) return lex_create_number_token(lex, ch);
    //else if (is_alpha(ch)) return lex_create_identifier_token(lex, ch);
    else if (ch.empty()) return lex_create_token(lex, TOK_EOF, ch);
    else if (ch == "\n") {
      auto tk = lex_create_token(lex, TOK_NL, "\\n");
      lex.line++;
      return tk;
    }
    else return lex_create_token(lex, TOK_ERR, ch);
  }
}

// peek はレキサーが現在指している文字を返す
std::string lex_peek(lex_t& lex) {
  if (lex.pos > lex.in.size()) return "";

  unsigned char c = lex.in[lex.pos];
  size_t char_size;

  if ((c & 0x80) == 0) char_size = 1;  // 1バイト文字
  else if ((c & 0xE0) == 0xC0) char_size = 2;  // 2バイト文字
  else if ((c & 0xF0) == 0xE0) char_size = 3;  // 3バイト文字
  else if ((c & 0xF8) == 0xF0) char_size = 4;  // 4バイト文字
  else {
    std::cerr << "Invalid UTF-8 sequence." << std::endl;
    return "";
  }

  std::string ch = lex.in.substr(lex.pos, char_size);
  return ch;
}

// advance はレキサーを1文字進める
std::string lex_advance(lex_t& lex) {
  std::string ch = lex_peek(lex);
  lex.pos += ch.size();

  return ch;
}
