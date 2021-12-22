#include <iostream>
#include <memory>
#include <utility>
#include "hello7_shared_ptr_ast.h"
#include "hello7_shared_ptr.hpp"


class unexpected_char : public std::exception {
};

template<class It>
class scanner {
public:
    typedef int char_type;

public:
    scanner(It b, It e) : b_(b), e_(e), c_(b), unget_(EOF) {}

    calc::Token get(std::shared_ptr<Node> &v) {
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
            v = std::make_shared<Number>(n);
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
    template <typename T>
    void downcast(std::shared_ptr<T>& x, const std::shared_ptr<Node>& y)
    {
        x = std::dynamic_pointer_cast<T, Node>(y);
    }

    // Derived -> Base
    template <typename T>
    void upcast(std::shared_ptr<Node>& x, const std::shared_ptr<T>& y)
    {
        x = std::static_pointer_cast<Node, T>(y);
    }

    // Semantic Actions
    std::shared_ptr<Expr> MakeExpr(std::shared_ptr<Term> x) {
        auto te = new TermExpr;
        te->term = std::move(x);
        return std::shared_ptr<Expr>(te);
    }

    std::shared_ptr<Expr> MakeAdd(std::shared_ptr<Expr> x, std::shared_ptr<Term> y) {
        std::cerr << "expr " << x << " + " << y << std::endl;
        auto add = new AddExpr;
        add->lhs = x;
        add->rhs = MakeExpr(y);
        return std::shared_ptr<Expr>(add);
    }

    std::shared_ptr<Expr> MakeSub(std::shared_ptr<Expr> x, std::shared_ptr<Term> y) {
        std::cerr << "expr " << x << " - " << y << std::endl;
        auto sub = new SubExpr;
        sub->lhs = x;
        sub->rhs = MakeExpr(y);
        return std::shared_ptr<Expr>(sub);
    }

    std::shared_ptr<Term> MakeTerm(std::shared_ptr<Number> x) {
        auto nt = new NumberTerm;
        nt->number = std::move(x);
        return std::shared_ptr<Term>(nt);
    }

    std::shared_ptr<Term> MakeMul(std::shared_ptr<Term> x, std::shared_ptr<Number> y) {
        std::cerr << "expr " << x << " * " << y << std::endl;
        auto mul = new MulTerm;
        mul->lhs = x;
        mul->rhs = MakeTerm(y);
        return std::shared_ptr<Term>(mul);
    }

    std::shared_ptr<Term> MakeDiv(std::shared_ptr<Term> x, std::shared_ptr<Number> y) {
        std::cerr << "expr " << x << " / " << y << std::endl;
        auto div = new DivTerm;
        div->lhs = x;
        div->rhs = MakeTerm(y);
        return std::shared_ptr<Term>(div);
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
    calc::Parser<std::shared_ptr<Node>, SemanticAction> parser(sa);

    calc::Token token;
    for (;;) {
        std::shared_ptr<Node> v;
        token = s.get(v); // ここは変えれる

        if (parser.post(token, v)) { break; }
    }

    std::shared_ptr<Node> v;
    if (parser.accept(v)) {
        std::cerr << "accpeted\n";
        std::cerr << v->calc() << std::endl;
        std::cerr << "exit\n";
    }

    return 0;
}