#include <memory>
#include <utility>

struct Node {
    virtual ~Node() = default;

    virtual int calc() = 0;
};

struct Expr : public Node {
};

struct Term : public Node {
};

struct Number : public Node {
    int number;

    explicit Number(int n) : number(n) {}

    int calc() override { return number; }
};

struct AddExpr : public Expr {
    std::shared_ptr<Expr> lhs;
    std::shared_ptr<Expr> rhs;

    int calc() override { return lhs->calc() + rhs->calc(); }
};

struct SubExpr : public Expr {
    std::shared_ptr<Expr> lhs;
    std::shared_ptr<Expr> rhs;

    int calc() override { return lhs->calc() - rhs->calc(); }
};

struct TermExpr : public Expr {
    std::shared_ptr<Term> term;

    int calc() override { return term->calc(); }
};

struct MulTerm : public Term {
    std::shared_ptr<Term> lhs;
    std::shared_ptr<Term> rhs;

    int calc() override { return lhs->calc() * rhs->calc(); }
};

struct DivTerm : public Term {
    std::shared_ptr<Term> lhs;
    std::shared_ptr<Term> rhs;

    int calc() override { return lhs->calc() / rhs->calc(); }
};

struct NumberTerm : public Term {
    std::shared_ptr<Number> number;

    int calc() override { return number->calc(); }
};
