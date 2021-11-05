#include <functional>
#include <variant>
#include <vector>

struct Add;
struct Sub;
struct Mul;
struct Div;

template<class OpTag>
struct BinOpTerm;
template<class OpTag>
struct BinOpExpr;

struct UnaryTerm;
struct BinaryTerm;
struct UnaryExpr;
struct BinaryExpr;

template<class T, class OpTag>
struct BinOp {
    T lhs;
    T rhs;

    BinOp(const T &x, const T &y)
        : lhs(x), rhs(y) {}
};

//using TermBinOperations = std::vector<BinOpTerm<Mul>, BinOpTerm<Div>>;
using Term = std::variant<UnaryTerm, BinaryTerm, int>;

struct UnaryTerm {
    std::unique_ptr<Term> rhs;
    std::function<Term(Term)> func;
};

struct BinaryTerm {
    std::unique_ptr<Term> lhs;
    Term rhs;
    std::function<Term(Term, Term)> func;
};

//typedef std::variant<
//        int,
//        boost::recursive_wrapper<BinOpTerm<Mul> >,
//        boost::recursive_wrapper<BinOpTerm<Div> > >
//        Term;

using ExprBinOperations = std::variant<BinOpExpr<Add>, BinOpExpr<Sub>>;
using Expr = std::variant<Term, ExprBinOperations>;

//typedef std::variant <
//Term,
//boost::recursive_wrapper<BinOpExpr<Add> >,
//boost::recursive_wrapper<BinOpExpr<Sub> >>
//        Expr;

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