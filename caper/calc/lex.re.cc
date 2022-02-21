#include <cstdlib>
#include "calc_ast.h"
#include "calc.hpp"


bool yylex(const char *cursor, std::vector<std::pair<calc::Token, std::shared_ptr<Node>>> &tokens)
{
    int num;
    char *marker;
    const char *token;

    tokens.empty();

    loop:
    token = cursor;
    /*!re2c
        re2c:api:style = free-form;
        re2c:define:YYCTYPE  = char;
        re2c:define:YYCURSOR = cursor;
        re2c:define:YYMARKER = marker;
        re2c:yyfill:enable   = 0;

        D = [0-9];
        N = [1-9];

        *      { return false; }
        [\x00] { tokens.push_back(std::make_pair(calc::token_eof, nullptr)); return true; }
        [ \t]  { goto loop; }

        "+"  { tokens.push_back(std::make_pair(calc::token_ADD, nullptr)); goto loop; }
        "-"  { tokens.push_back(std::make_pair(calc::token_SUB, nullptr)); goto loop; }
        "*"  { tokens.push_back(std::make_pair(calc::token_MUL, nullptr)); goto loop; }
        "/"  { tokens.push_back(std::make_pair(calc::token_DIV, nullptr)); goto loop; }
        "("  { tokens.push_back(std::make_pair(calc::token_LP, nullptr)); goto loop; }
        ")"  { tokens.push_back(std::make_pair(calc::token_RP, nullptr)); goto loop; }
        "\n" { tokens.push_back(std::make_pair(calc::token_NL, nullptr)); goto loop; }

        "0" {
            num = 0;
            tokens.push_back(std::make_pair(calc::token_Number, std::make_shared<Number>(0)));
            goto loop;
        }

        N{1}D* {
            size_t size = cursor - token;
            char *yytext = (char *)calloc(size, sizeof(char));
            memcpy(yytext, token, size);
            num = atoi(yytext);
            tokens.push_back(std::make_pair(calc::token_Number, std::make_shared<Number>(num)));
            free(yytext);
            goto loop;
        }
    */
}
