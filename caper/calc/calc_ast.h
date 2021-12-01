#include <memory>

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
    std::unique_ptr<Expr> lhs;
    std::unique_ptr<Expr> rhs;

    AddExpr(std::unique_ptr<Expr> x, std::unique_ptr<Expr> y) : lhs(std::move(x)), rhs(std::move(y)) {}

    // Move constructor
    AddExpr(AddExpr &&r) noexcept: lhs(std::move(r.lhs)), rhs(std::move(r.rhs)) {}

    int calc() override { return lhs->calc() + rhs->calc(); }
};

struct SubExpr : public Expr {
    std::unique_ptr<Expr> lhs;
    std::unique_ptr<Expr> rhs;

    SubExpr(std::unique_ptr<Expr> x, std::unique_ptr<Expr> y) : lhs(std::move(x)), rhs(std::move(y)) {}
    SubExpr(SubExpr &&r) noexcept: lhs(std::move(r.lhs)), rhs(std::move(r.rhs)) {}

    int calc() override { return lhs->calc() - rhs->calc(); }
};

struct TermExpr : public Expr {
    std::unique_ptr<Term> term;

    explicit TermExpr(std::unique_ptr<Term> x) : term(std::move(x)) {}

    int calc() override { return term->calc(); }
};

struct MulTerm : public Term {
    std::unique_ptr<Term> lhs;
    std::unique_ptr<Term> rhs;

    MulTerm(std::unique_ptr<Term> x, std::unique_ptr<Term> y) : lhs(std::move(x)), rhs(std::move(y)) {}

    int calc() override { return lhs->calc() * rhs->calc(); }
};

struct DivTerm : public Term {
    std::unique_ptr<Term> lhs;
    std::unique_ptr<Term> rhs;

    DivTerm(std::unique_ptr<Term> x, std::unique_ptr<Term> y) : lhs(std::move(x)), rhs(std::move(y)) {}

    int calc() override { return lhs->calc() / rhs->calc(); }
};

struct NumberTerm : public Term {
    std::unique_ptr<Number> number;

    explicit NumberTerm(std::unique_ptr<Number> x) : number(std::move(x)) {}

    int calc() override { return number->calc(); }
};
