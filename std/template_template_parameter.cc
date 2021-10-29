#include <iostream>
#include <vector>

template<
    // Type はテンプレート引数を２つ取る型
    template<class, class> class Type
>
struct Something {
    Type<int, std::allocator<int> > container;
};

template<class T, class U>
struct Foo {};

// 「テンプレート変数を一つとる型」と「型」を一つ受け取るテンプレート
template<template<class> class Container, typename T>
struct Bar {
    Container<T> container;
};

int main() {
    Something<Foo> a{};

    Bar<std::vector, int> b;

    b.container.push_back(1);

    std::cout << b.container.size() << std::endl;
}
