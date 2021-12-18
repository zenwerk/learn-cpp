#include <iostream>
#include <memory>


// unique_ptr を関数やコンストラクタの引数に渡す方法
void How_to_pass_unique_ptr() {
    // Pass by Value
    {
        class Base {
        public:
            Base() = default;

            // unique_ptr を値で受け取ることは、所有権の移動を意味する.
            // 引数を「値」で受け取ると、そうなることが保証される.
            explicit Base(std::unique_ptr<Base> n) : next(std::move(n)) {
                // unique_ptr `n` を「値」で受け取るので C++ は自動的に一時的な領域を確保する
                // この場合 std::move を介して && にキャストされた値から unique_ptr<Base>`n` を構築する
                // このテンポラリの構築処理によってパラメータから引数 `n` に実際に値が移動される
                std::cout << "Pass by Value: " << __FUNCTION__ << std::endl;
            }

            virtual ~Base() = default;

            void setNext(std::unique_ptr<Base> n) {
                next = std::move(n);
            }

        protected :
            std::unique_ptr<Base> next;
        };

        auto base = std::make_unique<Base>();
        // std::move は実際に「移動」する処理はしない. 実際に行うのは右辺値参照(&&)へキャストだけ.
        Base newBase(std::move(base));
        Base fromTemp(std::make_unique<Base>());
        // この場合、元となった値は移動され所有権は破棄される
        if (base == nullptr)
            std::cout << "base は move されました 1" << std::endl;
    }

    //=============================================================
    // Pass by Non-const l-value reference
    {
        class Base {
        public:
            Base() = default;

            // unique_ptr を左辺値参照で受け取ることは、所有権の移動を「保証」しない
            // 実際に移動されるかは処理によるためドキュメント or コードを読んで確認する必要がある
            // よって「ムーブセマンティクス」のインターフェイスとしては推奨されない
            explicit Base(std::unique_ptr<Base> &n) : next(std::move(n)) {
                std::cout << "Pass by Non-const l-value reference: " << __FUNCTION__ << std::endl;
            }

            virtual ~Base() = default;

            void setNext(std::unique_ptr<Base> n) {
                next = std::move(n);
            }

        protected :
            std::unique_ptr<Base> next;
        };

        auto base = std::make_unique<Base>();
        Base newBase(base);
        // Base fromTemp(std::make_unique<Base>()); // 左辺値しか受け取れないのでコンパイルエラー
        if (base == nullptr)
            std::cout << "base は move されました 2" << std::endl;
    }

    //=============================================================
    // Pass by const l-value reference
    {
        class Base {
        public:
            Base() = default;

            // const左辺値参照はmoveできない
            // よって const & のシグネチャを持つ関数は、所有権の移動を主張しない「非ムーブ」処理であることがわかる
            explicit Base(std::unique_ptr<Base> const &n) {
                std::cout << "Pass by const l-value reference: " << __FUNCTION__ << std::endl;
            }

            virtual ~Base() = default;

            void setNext(std::unique_ptr<Base> n) {
                next = std::move(n);
            }

        protected :
            std::unique_ptr<Base> next;
        };

        auto base = std::make_unique<Base>();
        Base newBase(base);
        Base fromTemp(std::make_unique<Base>());
        if (base != nullptr)
            std::cout << "base は move されません" << std::endl;
    }

    //=============================================================
    // Pass by r-value reference
    {
        class Base {
        public:
            Base() = default;

            // unique_ptr を右辺値参照で受け取るのは、非const左辺値参照として受け取る場合とほぼ同じだが
            // && のシグネチャで受け取る場合、実際に移動されるかは「保証されない」
            // よってドキュメント or 実装を確認する必要がある
            explicit Base(std::unique_ptr<Base> &&n) : next(std::move(n)) {
                std::cout << "Pass by r-value reference: " << __FUNCTION__ << std::endl;
            }

            virtual ~Base() = default;

            void setNext(std::unique_ptr<Base> n) {
                next = std::move(n);
            }

        protected :
            std::unique_ptr<Base> next;
        };

        auto base = std::make_unique<Base>();
        // 以下の2点が異なる
        // Base newBase(base); // && で受け取る場合は `std::move` が必須
        Base newBase(std::move(base));
        Base fromTemp(std::make_unique<Base>()); // && なのでコンパイルエラーにならない
    }
}

// unique_ptr を使って ASTNode を作る
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

struct TermExpr : public Expr {
    std::unique_ptr<Term> term;

    explicit TermExpr(std::unique_ptr<Term> x) : term(std::move(x)) {}

    int calc() override { return term->calc(); }
};

struct NumberTerm : public Term {
    std::unique_ptr<Number> number;

    explicit NumberTerm(std::unique_ptr<Number> x) : number(std::move(x)) {}

    int calc() override { return number->calc(); }
};

std::unique_ptr<Expr> MakeExpr(std::unique_ptr<Term> x) {
    //return (std::unique_ptr<Expr> &&) x; // これでも動く
    return std::make_unique<TermExpr>(std::move(x));
}

std::unique_ptr<Expr> MakeAdd(std::unique_ptr<Expr> x, std::unique_ptr<Term> y) {
    auto expr = MakeExpr(std::move(y));
    return std::make_unique<AddExpr>(std::move(x), std::move(expr));
}

void nodeTest() {
    auto num1 = std::make_unique<NumberTerm>(std::make_unique<Number>(1));
    auto num2 = std::make_unique<NumberTerm>(std::make_unique<Number>(2));

    auto expr1 = MakeExpr(std::move(num1));
    //auto expr2 = MakeExpr(std::move(num2));
    //auto expr1 = std::make_unique<TermExpr>(std::move(num1));
    //auto expr2 = std::make_unique<TermExpr>(std::move(num2));

    //auto addExpr = std::make_unique<AddExpr>(std::move(expr1), std::move(expr2));
    auto addExpr = MakeAdd(std::move(expr1), std::move(num2));
    std::cout << addExpr->calc() << std::endl;
}



int main() {
    How_to_pass_unique_ptr();

    nodeTest();

    return 0;
}