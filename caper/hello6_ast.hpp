#include <iostream>
#include <vector>
#include <memory>
#include <variant>
#include <string>

/*
 * 前方宣言
 */
struct Add;
struct Sub;
struct Mul;
struct Div;

template<class OpTag>
struct BinOpTerm;
template<class OpTag>
struct BinOpExpr;

/* 不動点コンビネータ */
template<template<typename> class K>
struct Fix : K<Fix<K>> {
    using K<Fix>::K;
};


template<class T>
using Term_ = std::variant<int, BinOpTerm<Mul>, BinOpTerm<Div>, std::vector<T>>;
using Term = Fix<Term_>;

template<class T>
using Expr_ = std::variant<Term, BinOpExpr<Add>, BinOpExpr<Sub>, std::vector<T>>;
using Expr = Fix<Expr_>;

//======? BinOpTermの定義には Term が必要
template<class OpTag>
struct BinOpTerm {
    std::unique_ptr<Term> lhs;
    std::unique_ptr<Term> rhs;
/*
    BinOpTerm() = default;
    BinOpTerm(BinOpTerm&& rhs) noexcept {
        swap(rhs);
    }
    BinOpTerm& operator=(BinOpTerm&& rhs_) noexcept {
        swap(rhs_);
        return *this;
    }

private:
    void swap(BinOpTerm& rhs_) noexcept {
        *this->lhs = rhs_.lhs;
        *this->rhs = rhs_.rhs;
        rhs_.lhs = nullptr;
        rhs_.rhs = nullptr;
    }
*/
};


template<class OpTag>
struct BinOpExpr {
    std::unique_ptr<Expr> lhs;
    std::unique_ptr<Expr> rhs;
/*
    BinOpExpr(const Expr &x, const Expr &y)
        : lhs(x), rhs(y) {}
    BinOpExpr() = default;
    BinOpExpr(BinOpExpr&& rhs) noexcept {
        swap(rhs);
    }
    BinOpExpr& operator=(BinOpExpr&& rhs_) noexcept {
        swap(rhs_);
        return *this;
    }

private:
    void swap(BinOpExpr& rhs_) noexcept {
        *this->lhs = rhs_.lhs;
        *this->rhs = rhs_.rhs;
        rhs_.lhs = nullptr;
        rhs_.rhs = nullptr;
    }
*/
};


int hoge() {
    using V = std::vector<Term>;
    Term t1 = {V{1, 2, V{3}}};
    Term t2 = {1};

    Term t3 = Term{1};
    Expr x = Term{1};
    Term t4 = 1;

    // move
    Expr xx = std::move(x);

    Expr xxx = std::move(t1);

    Expr xxxx = t4;

    auto a = BinOpTerm<Sub>{std::make_unique<Term>(Term{1}), std::make_unique<Term>(Term{1})};

    return 0;
}

void foo() {
    using A = std::variant<int, std::string>;
    using B = std::variant<A, char>;

    A a = "a";
    B b = a;

    std::cout << std::get<std::string>(std::get<A>(b)) << std::endl;
}