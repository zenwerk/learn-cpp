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

    AddExpr(std::shared_ptr<Expr> x, std::shared_ptr<Expr> y) : lhs(std::move(x)), rhs(std::move(y)) {}

    // Move constructor
    AddExpr(AddExpr &&r) noexcept: lhs(std::move(r.lhs)), rhs(std::move(r.rhs)) {}

    int calc() override { return lhs->calc() + rhs->calc(); }
};

struct SubExpr : public Expr {
    std::shared_ptr<Expr> lhs;
    std::shared_ptr<Expr> rhs;

    SubExpr(std::shared_ptr<Expr> x, std::shared_ptr<Expr> y) : lhs(std::move(x)), rhs(std::move(y)) {}
    SubExpr(SubExpr &&r) noexcept: lhs(std::move(r.lhs)), rhs(std::move(r.rhs)) {}

    int calc() override { return lhs->calc() - rhs->calc(); }
};

struct TermExpr : public Expr {
    std::shared_ptr<Term> term;

    explicit TermExpr(std::shared_ptr<Term> x) : term(std::move(x)) {}

    int calc() override { return term->calc(); }
};

struct MulTerm : public Term {
    std::shared_ptr<Term> lhs;
    std::shared_ptr<Term> rhs;

    MulTerm(std::shared_ptr<Term> x, std::shared_ptr<Term> y) : lhs(std::move(x)), rhs(std::move(y)) {}

    int calc() override { return lhs->calc() * rhs->calc(); }
};

struct DivTerm : public Term {
    std::shared_ptr<Term> lhs;
    std::shared_ptr<Term> rhs;

    DivTerm(std::shared_ptr<Term> x, std::shared_ptr<Term> y) : lhs(std::move(x)), rhs(std::move(y)) {}

    int calc() override { return lhs->calc() / rhs->calc(); }
};

struct NumberTerm : public Term {
    std::shared_ptr<Number> number;

    explicit NumberTerm(std::shared_ptr<Number> x) : number(std::move(x)) {}

    int calc() override { return number->calc(); }
};
