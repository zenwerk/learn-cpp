#include <variant>

struct Add;
struct Sub;
struct Mul;
struct Div;

template<class OpTag>
struct BinOpTerm;
template<class OpTag>
struct BinOpExpr;

typedef std::variant<
        int,
        boost::recursive_wrapper<BinOpTerm<Mul> >,
        boost::recursive_wrapper<BinOpTerm<Div> > >
        Term;

typedef std::variant <
Term,
boost::recursive_wrapper<BinOpExpr<Add> >,
boost::recursive_wrapper<BinOpExpr<Sub> >>
        Expr;

template<class OpTag>
struct BinOpTerm {
    Term lhs;
    Term rhs;

    BinOpTerm(const Term &x, const Term &y)
            : lhs(x), rhs(y) {}
};

template<class OpTag>
struct BinOpExpr {
    Expr lhs;
    Expr rhs;

    BinOpExpr(const Expr &x, const Expr &y)
            : lhs(x), rhs(y) {}
};