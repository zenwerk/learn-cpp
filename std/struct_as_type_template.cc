#include <iostream>

/**
 * テンプレート引数が2つ以上必要なものを, 構造体一つだけ渡して引数を一つだけに削減する方法
 * 使いどころは不明. こういう書き方もできるという参考に.
 */

struct A {
    using X = int;
    using Y = std::string;
};

struct B {
    using X = double;
    using Y = int;
};

template<typename T>
struct C {
    C(typename T::X _x, typename T::Y _y) : x(_x), y(_y) {}

    typename T::X x;
    typename T::Y y;
};


int main() {
    C<A> a(1, "foo");
    C<B> b(3.14, 2);

    std::cout << a.x << " " << a.y << std::endl;
    std::cout << b.x << " " << b.y << std::endl;
}