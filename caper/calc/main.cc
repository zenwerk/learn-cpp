#include <iostream>
#include <memory>
#include "calc_ast.h"
#include "calc.hpp"


class unexpected_char : public std::exception {
};

template<class It>
class scanner {
public:
    typedef int char_type;

public:
    scanner(It b, It e) : b_(b), e_(e), c_(b), unget_(EOF) {}

    calc::Token get(std::unique_ptr<Node> &v) {
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
            v = std::make_unique<Number>(n);
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

    // Base -> Derived
    template<class T>
    void downcast(std::unique_ptr<T> &x, const std::unique_ptr<Node> &&y) {
        auto result = static_cast<T*>(y.get());
        y.release();
        x.reset(result);
    }

    // Derived -> Base
    template<class T>
    void upcast(std::unique_ptr<Node> &x, std::unique_ptr<T> &&y) {
        auto result = static_cast<Node*>(y.get());
        y.release();
        x.reset(result);
    }

    // TODO: アクションの定義
    // TODO: ASTノードを返すセマンティックアクションを定義する
    std::unique_ptr<Expr> MakeExpr(std::unique_ptr<Term> x) {
        return std::make_unique<TermExpr>(std::move(x));
    }

    std::unique_ptr<Expr> MakeAdd(std::unique_ptr<Expr> x, std::unique_ptr<Term> y) {
        std::cerr << "expr " << x << " + " << y << std::endl;
        auto expr = MakeExpr(std::move(y));
        return std::make_unique<AddExpr>(std::move(x), std::move(expr));
    }

    std::unique_ptr<Expr> MakeSub(std::unique_ptr<Expr> x, std::unique_ptr<Term> y) {
        std::cerr << "expr " << x << " - " << y << std::endl;
        auto expr = MakeExpr(std::move(y));
        return std::make_unique<SubExpr>(std::move(x), std::move(expr));
    }

    std::unique_ptr<Term> MakeTerm(std::unique_ptr<Number> x) {
        return std::make_unique<NumberTerm>(std::move(x));
    }

    std::unique_ptr<Term> MakeMul(std::unique_ptr<Term> x, std::unique_ptr<Number> y) {
        std::cerr << "expr " << x << " * " << y << std::endl;
        auto term = std::make_unique<NumberTerm>(std::move(y));
        return std::make_unique<MulTerm>(std::move(x), std::move(term));
    }

    std::unique_ptr<Term> MakeDiv(std::unique_ptr<Term> x, std::unique_ptr<Number> y) {
        std::cerr << "expr " << x << " / " << y << std::endl;
        auto term = std::make_unique<NumberTerm>(std::move(y));
        return std::make_unique<DivTerm>(std::move(x), std::move(term));
    }
};

int main() {
    // スキャナ
    typedef std::istreambuf_iterator<char> is_iterator;
    is_iterator b(std::cin);
    is_iterator e;
    scanner<is_iterator> s(b, e);

    SemanticAction sa;

    // 値全体の集合を表す型が `Node*`
    // cpgファイルが示すように、各文法要素の型は`Node*`とは異なるので、それにあわせて変換が必要になる -> upcast/downcast
    // downcast = Node* → Expr*/Term*/Number* の変換
    // upcast は downcast の逆操作
    calc::Parser<std::unique_ptr<Node>, SemanticAction> parser(sa);

    calc::Token token;
    for (;;) {
        std::unique_ptr<Node> v;
        token = s.get(v); // ここは変えれる

        if (parser.post(token, v)) { break; }
    }

    std::unique_ptr<Node> v;
    if (parser.accept(v)) {
        std::cerr << "accpeted\n";
        std::cerr << v->calc() << std::endl;
        std::cerr << "exit\n";
    }

    return 0;
}