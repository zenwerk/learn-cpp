﻿// 2014/03/22 Naoyuki Hirayama
// All Rights Reserved.

#include <iostream>
#include "optional.hpp"

class unexpected_char : public std::exception {};

template <class It>
class scanner {
public:
    typedef int char_type;
    int eof() { return std::char_traits<char_type>::eof(); }

public:
    scanner(It b, It e) : b_(b), e_(e), c_(b), unget_(eof()) {}

    opt::Token get(int& v) {
        int c;
        do {
            c = getc();
        } while (isspace(c));

        // 記号類
        if (c == eof()) {
            return opt::token_eof;
        } else {
            v = c;
            switch (c) {
                case '(': return opt::token_LParen;
                case ')': return opt::token_RParen;
                case ',': return opt::token_Comma;
                case '*': return opt::token_Star;
            }
        }

        // 整数
        if (isdigit(c)) {
            int n = 0;
            while (c != eof()&& isdigit(c)) {
                n *= 10;
                n += c - '0';
                c = getc();
            }
            ungetc(c);
            v = n;
            return opt::token_Number;
        }


        std::cerr << char(c) << std::endl;
        throw unexpected_char();
    }

private:
    char_type getc() {
        int c;
        if (unget_ != eof()) {
            c = unget_;
            unget_ = eof();
        } else if (c_ == e_) {
            c = eof(); 
        } else {
            c = *c_++;
        }
        return c;
    }

    void ungetc(char_type c) {
        if (c != eof()) {
            unget_ = c;
        }
    }

private:
    It              b_;
    It              e_;
    It              c_;
    char_type       unget_;

};

struct SemanticAction {
    void syntax_error() {}
    void stack_overflow() {}
    void downcast(int& x, int y) { x = y; }
    void upcast(int& x, int y) { x = y; }

    template <class T>
    int Document(const T& x) {
        if (x) {
            return *x;
        } else {
            return 42;
        }
    }
};

int main( int, char** )
{
    // スキャナ
    typedef std::istreambuf_iterator<char> is_iterator;
    is_iterator b( std::cin );   // 即値にするとVC++が頓珍漢なことを言う
    is_iterator e;
    scanner<is_iterator> s(b, e);

    SemanticAction sa;
    opt::Parser<int, SemanticAction> parser(sa);

    opt::Token token;
    for(;;) {
        int v;
        token = s.get( v );
        if (parser.post(token, v)) { break; }
    }

    if (parser.error()) {
        std::cerr << "error occured: " << opt::token_label(token) << std::endl;
        exit(1);
    }

    int v;
    if (parser.accept(v)) {
        std::cout << "accepted\n";
        std::cout << v << std::endl;
    }

    return 0;
}
