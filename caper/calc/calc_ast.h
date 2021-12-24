#include <memory>
#include <utility>
#include <string>

struct Node {
    virtual ~Node() = default;

    virtual int calc() = 0;

    virtual std::string str() = 0;
};

struct Expr : public Node {
};

struct Term : public Node {
};

struct Factor : public Node {
};

struct Literal : public Node {
};

struct Number : public Node {
    int number;

    explicit Number(int n) : number(n) {}

    int calc() override { return number; }

    std::string str() override {
        return "Raw[" + std::to_string(number) + "]";
    }
};

struct AddExpr : public Expr {
    std::shared_ptr<Expr> lhs;
    std::shared_ptr<Expr> rhs;

    int calc() override {
        return lhs->calc() + rhs->calc();
    }

    std::string str() override {
        return "Add[" + lhs->str() + "+" + rhs->str() + "]";
    }
};

struct SubExpr : public Expr {
    std::shared_ptr<Expr> lhs;
    std::shared_ptr<Expr> rhs;

    int calc() override {
        return lhs->calc() - rhs->calc();
    }

    std::string str() override {
        return "Sub[" + lhs->str() + "-" + rhs->str() + "]";
    }
};

struct TermExpr : public Expr {
    std::shared_ptr<Term> term;

    int calc() override {
        return term->calc();
    }

    std::string str() override {
        return "Term[" + term->str() + "]";
    }
};

struct MulTerm : public Term {
    std::shared_ptr<Term> lhs;
    std::shared_ptr<Term> rhs;

    int calc() override {
        return lhs->calc() * rhs->calc();
    }

    std::string str() override {
        return "Mul[" + lhs->str() + "*" + rhs->str() + "]";
    }
};

struct DivTerm : public Term {
    std::shared_ptr<Term> lhs;
    std::shared_ptr<Term> rhs;

    int calc() override {
        return lhs->calc() / rhs->calc();
    }

    std::string str() override {
        return "Div[" + lhs->str() + "/" + rhs->str() + "]";
    }
};

struct FactorTerm : public Term {
    std::shared_ptr<Factor> factor;

    int calc() override {
        return factor->calc();
    }

    std::string str() override {
        return "Fac[" + factor->str() + "]";
    }
};

struct LiteralFactor : public Factor {
    std::shared_ptr<Literal> literal;

    int calc() override {
        return literal->calc();
    }

    std::string str() override {
        return "Lit[" + literal->str() + "]";
    }
};

struct UMinusFactor : public Factor {
    std::shared_ptr<Factor> factor;

    int calc() override {
        return -1 * factor->calc();
    }

    std::string str() override {
        return "Umi[" + factor->str() + "]";
    }
};

struct NumberLiteral : public Literal {
    std::shared_ptr<Number> number;

    int calc() override {
        return number->calc();
    }

    std::string str() override {
        return std::to_string(number->number);
    }
};

struct NestedExprLiteral : public Literal {
    std::shared_ptr<Expr> expr;

    int calc() override {
        return expr->calc();
    }

    std::string str() override {
        return "Nest[" + expr->str() + "]";
    }
};
