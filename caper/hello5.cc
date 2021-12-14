#include <mapbox/variant.hpp>
#include "hello5_ast.hpp"
#include "hello5.hpp"
#include <iostream>

class unexpected_char : public std::exception {
};

using Value = mapbox::util::variant<int, Term, Expr>;

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

// パーサーは variant に所属しているASTノードをパース結果から返す
struct SemanticAction {
    void syntax_error() {}

    void stack_overflow() {}

    template<class T>
    void downcast(T &x, Value y) { x = mapbox::util::get<T>(y); }

    template<class T>
    void upcast(Value &x, const T &y) { x = y; }

    // 式を返す
    Expr MakeExpr(const Term &x) { return Expr(x); }

    // +演算を返す
    Expr MakeAdd(const Expr &x, const Term &y) {
        std::cerr << "expr " << x << " + " << y << std::endl;
        return BinOpExpr<Add>(x, y);
    }

    // -演算を返す
    Expr MakeSub(const Expr &x, const Term &y) {
        std::cerr << "expr " << x << " - " << y << std::endl;
        return BinOpExpr<Sub>(x, y);
    }

    // 開始(Term)を返す
    Term MakeTerm(int x) { return Term(x); }

    // 乗算Nodeを返す
    Term MakeMul(const Term &x, int y) {
        std::cerr << "expr " << x << " * " << y << std::endl;
        return BinOpTerm<Mul>(x, y);
    }

    // 除算Nodeを返す
    Term MakeDiv(const Term &x, int y) {
        std::cerr << "expr " << x << " / " << y << std::endl;
        return BinOpTerm<Div>(x, y);
    }
};

// これが std::vistor の役割で ASTノードを受け取って処理をする実体
// ここで x.lhs とか x.rhs で再帰的に木構造を下っていく、ので variant が再帰的に定義されていると嬉しい
struct calculator {
    int operator()(int n) const { return n; }

    int operator()(const Term &x) const {
        return mapbox::util::apply_visitor(*this, x);
    }

    int operator()(const BinOpTerm <Mul> &x) const {
        // ここで x.func(visit, visit) とかしているのが EvaluatorExpression の違いだな
        return mapbox::util::apply_visitor(*this, x.lhs) *
               mapbox::util::apply_visitor(*this, x.rhs);
    }

    int operator()(const BinOpTerm <Div> &x) const {
        return mapbox::util::apply_visitor(*this, x.lhs) /
               mapbox::util::apply_visitor(*this, x.rhs);
    }

    int operator()(const Expr &x) const {
        return mapbox::util::apply_visitor(*this, x);
    }

    int operator()(const BinOpExpr <Add> &x) const {
        return mapbox::util::apply_visitor(*this, x.lhs) +
               mapbox::util::apply_visitor(*this, x.rhs);
    }

    int operator()(const BinOpExpr <Sub> &x) const {
        return mapbox::util::apply_visitor(*this, x.lhs) -
               mapbox::util::apply_visitor(*this, x.rhs);
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
        token = s.get(v);
        if (parser.post(token, v)) { break; }
    }

    Value v;
    if (parser.accept(v)) {
        std::cerr << "accpeted\n";
        std::cerr << mapbox::util::apply_visitor(calculator(), v)
                  << std::endl;
    }

    return 0;
}