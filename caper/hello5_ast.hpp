#include <mapbox/variant.hpp>
#include <iostream>

struct Add;
struct Sub;
struct Mul;
struct Div;

template<class OpTag>
struct BinOpTerm;
template<class OpTag>
struct BinOpExpr;


typedef mapbox::util::variant<
    int,
    mapbox::util::recursive_wrapper<BinOpTerm<Mul> >,
    mapbox::util::recursive_wrapper<BinOpTerm<Div> > >
    Term;
typedef mapbox::util::variant<
    Term,
    mapbox::util::recursive_wrapper<BinOpExpr<Add> >,
    mapbox::util::recursive_wrapper<BinOpExpr<Sub> > >
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

std::ostream &operator<<(std::ostream &os, const Expr &x) {
    os <<  "EXPR";
    return os;
}

template<class T>
std::ostream &operator<<(std::ostream &os, const BinOpTerm<T> &x) {
    os << x.lhs << " ? " << x.rhs;
    return os;
}

template<>
std::ostream &operator<<<Mul>(std::ostream &os, const BinOpTerm<Mul> &x) {
    os << x.lhs << " * " << x.rhs;
    return os;
}

template<>
std::ostream &operator<<<Div>(std::ostream &os, const BinOpTerm<Div> &x) {
    os << x.lhs << " / " << x.rhs;
    return os;
}

template<class T>
std::ostream &operator<<(std::ostream &os, const BinOpExpr<T> &x) {
    os << x.lhs << " ? " << x.rhs;
    return os;
}

template<>
std::ostream &operator<<<Add>(std::ostream &os, const BinOpExpr<Add> &x) {
    os << x.lhs << " + " << x.rhs;
    return os;
}

template<>
std::ostream &operator<<<Sub>(std::ostream &os, const BinOpExpr<Sub> &x) {
    os << x.lhs << " - " << x.rhs;
    return os;
}