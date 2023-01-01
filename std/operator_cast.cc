#include <iostream>

// C++ では `operator 型名()` でその型独自のキャストを実装できる

class Bar {
    int *num;

public:
    Bar() : num(nullptr) {}
    explicit Bar(int *num_) : num(num_) {}

    void print() {
        std::cout << "Bar: " << *num << std::endl;
    }

    // 明示的な bool 型へのキャストを実装する
    explicit operator bool() const {
        if (num == nullptr)
            return false;
        return *num > 0;
    }
};

class Foo {
    int num;

public:
    explicit Foo(int num_) : num(num_) {}

    // キャストを利用してBarを生成して返す (テクニック？)
    // あまり良い実装とは思えない...
    operator Bar() {
        return Bar(&num);
    }
};

Bar get_bar(Foo &foo) {
    // 暗黙の型変換によって Bar が生成されて返される
    return foo;
}

int main() {
    Foo foooo(777);
    Bar barrr = foooo;  // Bar を生成して返している
    barrr.print();

    Foo foo(1);
    auto bar = get_bar(foo);
    bar.print();
    bool buzz = (bool)bar;

    if (bar && buzz) {
        std::cout << "Bar::num > 0" << std::endl;
    }

    return 0;
}