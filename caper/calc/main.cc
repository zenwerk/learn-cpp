#include <iostream>
#include <memory>
#include <utility>
//#include "calc_ast.h"
//#include "calc.hpp"

#include "lex.cc"


struct SemanticAction {
    void syntax_error() {}

    void stack_overflow() {}

    // Base -> Derived
    template<typename T>
    void downcast(std::shared_ptr<T> &x, const std::shared_ptr<Node> &y) {
        if (auto result = std::dynamic_pointer_cast<T, Node>(y)) {
            x = result;
        } else {
            std::cerr << "Failed: dynamic_pointer_cast<T, Node>(y)" << std::endl;
            exit(1);
        }
    }

    // Derived -> Base
    template<typename T>
    void upcast(std::shared_ptr<Node> &x, const std::shared_ptr<T> &y) {
        x = std::static_pointer_cast<Node, T>(y);
    }

    // Semantic Actions
    std::shared_ptr<Expr> MakeExpr(std::shared_ptr<Term> x) {
        std::cerr << "MakeExpr: " << x->str() << std::endl;
        auto te = new TermExpr;
        te->term = x;
        std::cerr << "  end: MakeExpr" << std::endl;
        return std::shared_ptr<Expr>(te);
    }

    std::shared_ptr<Expr> MakeAdd(std::shared_ptr<Expr> x, std::shared_ptr<Term> y) {
        std::cerr << "MakeAdd " << x->str() << " + " << y->str() << std::endl;
        auto add = new AddExpr;
        add->lhs = x;
        add->rhs = MakeExpr(y);
        std::cerr << "  end: MakeAdd" << std::endl;
        return std::shared_ptr<Expr>(add);
    }

    std::shared_ptr<Expr> MakeSub(std::shared_ptr<Expr> x, std::shared_ptr<Term> y) {
        std::cerr << "MakeSub " << x->str() << " - " << y->str() << std::endl;
        auto sub = new SubExpr;
        sub->lhs = x;
        sub->rhs = MakeExpr(y);
        std::cerr << "  end: MakeSub" << std::endl;
        return std::shared_ptr<Expr>(sub);
    }

    std::shared_ptr<Term> MakeTerm(std::shared_ptr<Factor> x) {
        std::cerr << "MakeTerm " << x->str() << std::endl;
        auto nt = new FactorTerm;
        nt->factor = x;
        std::cerr << "  end: MakeTerm" << std::endl;
        return std::shared_ptr<Term>(nt);
    }

    std::shared_ptr<Term> MakeMul(std::shared_ptr<Term> x, std::shared_ptr<Factor> y) {
        std::cerr << "MakeMul " << x->str() << " * " << y->str() << std::endl;
        auto mul = new MulTerm;
        mul->lhs = x;
        mul->rhs = MakeTerm(y);
        std::cerr << "  end: MakeMul" << std::endl;
        return std::shared_ptr<Term>(mul);
    }

    std::shared_ptr<Term> MakeDiv(std::shared_ptr<Term> x, std::shared_ptr<Factor> y) {
        std::cerr << "MakeDiv " << x->str() << " / " << y->str() << std::endl;
        auto div = new DivTerm;
        div->lhs = x;
        div->rhs = MakeTerm(y);
        std::cerr << "  end: MakeDiv" << std::endl;
        return std::shared_ptr<Term>(div);
    }

    std::shared_ptr<Factor> MakeFactor(std::shared_ptr<Literal> x) {
        std::cerr << "MakeFactor " << x->str() << std::endl;
        auto factor = new LiteralFactor;
        factor->literal = x;
        std::cerr << "  end: MakeFactor" << std::endl;
        return std::shared_ptr<Factor>(factor);
    }

    std::shared_ptr<Factor> MakeUminus(std::shared_ptr<Factor> x) {
        std::cerr << "MakeUminus " << x->str() << std::endl;
        auto uminus = new UMinusFactor;
        uminus->factor = x;
        std::cerr << "  end: MakeUminus" << std::endl;
        return std::shared_ptr<Factor>(uminus);
    }

    std::shared_ptr<Literal> MakeNumber(std::shared_ptr<Number> x) {
        std::cerr << "MakeNumber " << x->str() << std::endl;
        auto number = new NumberLiteral;
        number->number = x;
        std::cerr << "  end: MakeNumber" << std::endl;
        return std::shared_ptr<Literal>(number);
    }

    std::shared_ptr<Literal> MakeNestedExpr(std::shared_ptr<Expr> x) {
        std::cerr << "MakeNextedExpr " << x->str() << std::endl;
        auto nestedExpr = new NestedExprLiteral;
        nestedExpr->expr = x;
        std::cerr << "  end: MakeNestedExpr" << std::endl;
        return std::shared_ptr<Literal>(nestedExpr);
    }

    std::shared_ptr<Expr> ReturnExpr(std::shared_ptr<Expr> x) {
        return x;
    }
};

int main() {
    SemanticAction sa;

    calc::Parser<std::shared_ptr<Node>, SemanticAction> parser(sa);
    std::vector<std::pair<calc::Token, std::shared_ptr<Node>>> tokens;
    std::string input;

    while(std::cout << "> " << std::flush, std::getline(std::cin, input)) {
        if (yylex(input.c_str(), tokens)) {
            for (const auto &token: tokens) {
                std::cout << "Result: " << calc::token_label(token.first) << " -- "
                          << ((token.second != nullptr) ? token.second->str() : "nullptr") << std::endl;
                if (parser.post(token.first, token.second)) { break; }
            }
        } else {
            std::cerr << "yylex failed" << std::endl;
            continue;
        }

        std::shared_ptr<Node> v;
        if (parser.accept(v)) {
            std::cerr << "Accepted" << std::endl;
            std::cerr << v->str() << std::endl;
            std::cout << v->calc() << std::endl;
        } else {
            std::cerr << "Not Accepted" << std::endl;
        }

        parser.reset();
    }

    return 0;
}