#include <variant>
#include "hello5_ast.hpp"
#include "hello5.hpp"
#include <iostream>

class unexpected_char : public std::exception {
};

typedef std::variant<int, Term, Expr> Value;

template<class It>
class scanner {
public:
    typedef int char_type;

public:
    scanner(It b, It e) : b_(b), e_(e), c_(b), unget_(EOF) {}

    calc::Token get(Value &v) {
        int c;
        do {
            c = getc();
        } while (isspace(c));

        // 記号類
        switch (c) {
            case '+':
                return calc::token_Add;
            case '-':
                return calc::token_Sub;
            case '*':
                return calc::token_Mul;
            case '/':
                return calc::token_Div;
            case EOF:
                return calc::token_eof;
        }

        // 整数
        if (isdigit(c)) {
            int n = 0;
            while (c != EOF && isdigit(c)) {
                n *= 10;
                n += c - '0';
                c = getc();
            }
            ungetc(c);
            v = n;
            return calc::token_Number;
        }


        std::cerr << char(c) << std::endl;
        throw unexpected_char();
    }

private:
    char_type getc() {
        int c;
        if (unget_ != EOF) {
            c = unget_;
            unget_ = EOF;
        } else if (c_ == e_) {
            c = EOF;
        } else {
            c = *c_++;
        }
        return c;
    }

    void ungetc(char_type c) {
        if (c != EOF) {
            unget_ = c;
        }
    }

private:
    It b_;
    It e_;
    It c_;
    char_type unget_;

};

struct SemanticAction {
    void syntax_error() {}

    void stack_overflow() {}

    template<class T>
    void downcast(T &x, Value y) { x = boost::get<T>(y); }

    template<class T>
    void upcast(Value &x, const T &y) { x = y; }

    Expr MakeExpr(const Term &x) { return Expr(x); }

    Expr MakeAdd(const Expr &x, const Term &y) {
        std::cerr << "expr " << x << " + " << y << std::endl;
        return BinOpExpr<Add>(x, y);
    }

    Expr MakeSub(const Expr &x, const Term &y) {
        std::cerr << "expr " << x << " - " << y << std::endl;
        return BinOpExpr<Sub>(x, y);
    }

    Term MakeTerm(int x) { return Term(x); }

    Term MakeMul(const Term &x, int y) {
        std::cerr << "expr " << x << " * " << y << std::endl;
        return BinOpTerm<Mul>(x, y);
    }

    Term MakeDiv(const Term &x, int y) {
        std::cerr << "expr " << x << " / " << y << std::endl;
        return BinOpTerm<Div>(x, y);
    }
};

struct calculator : public boost::static_visitor<int> {
    int operator()(int n) const { return n; }

    int operator()(const Term &x) const {
        return boost::apply_visitor(calculator(), x);
    }

    int operator()(const BinOpTerm <Mul> &x) const {
        return boost::apply_visitor(calculator(), x.lhs) *
               boost::apply_visitor(calculator(), x.rhs);
    }

    int operator()(const BinOpTerm <Div> &x) const {
        return boost::apply_visitor(calculator(), x.lhs) /
               boost::apply_visitor(calculator(), x.rhs);
    }

    int operator()(const Expr &x) const {
        return boost::apply_visitor(calculator(), x);
    }

    int operator()(const BinOpExpr <Add> &x) const {
        return boost::apply_visitor(calculator(), x.lhs) +
               boost::apply_visitor(calculator(), x.rhs);
    }

    int operator()(const BinOpExpr <Sub> &x) const {
        return boost::apply_visitor(calculator(), x.lhs) -
               boost::apply_visitor(calculator(), x.rhs);
    }

};

int main(int, char **) {
    // スキャナ
    typedef std::istreambuf_iterator<char> is_iterator;
    is_iterator b(std::cin);
    is_iterator e;
    scanner<is_iterator> s(b, e);

    SemanticAction sa;

    calc::Parser <Value, SemanticAction> parser(sa);

    calc::Token token;
    for (;;) {
        Value v;
        token = scanner.get(v);
        if (parser.post(token, v)) { break; }
    }

    Value v;
    if (parser.accept(v)) {
        std::cerr << "accpeted\n";
        std::cerr << boost::apply_visitor(calculator(), v)
                  << std::endl;
    }

    return 0;
}