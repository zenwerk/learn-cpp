#include <iostream>

// ラムダ式のオーバーロード: https://yohhoy.hatenadiary.jp/entry/20200715/p1
// Modern C++17 Snippets: https://404-notfound.jp/archives/649

// templateで関数オブジェクトを複数受け取る
template<typename... Ts>
class OverloadedLambdas : public Ts ... {
public:
  // using 宣言で基底クラス（ラムダ式のクロージャクラス）が提供するoperator()群をoverloadedクラス自身から公開する
  using Ts::operator()...;
};
// テンプレート引数<Ts...>の補助推論 (C++20からは不要になるらしい)
template<typename... Ts> OverloadedLambdas(Ts &&...) -> OverloadedLambdas<Ts...>;


int main() {
  OverloadedLambdas print_type{
    [&](int &n) { std::cout << "int: " << n << std::endl; },
    [&](char &c) { std::cout << "char: " << c << std::endl; },
    [&](float &n) { std::cout << "float: " << n << std::endl; }
  };

  int foo{42};
  print_type(foo);

  char bar{'c'};
  print_type(bar);

  float buzz{3.14};
  print_type(buzz);

  return 0;
}


/*
#include <iostream>
#include <variant>

struct Fluid {};
struct LightItem {};
struct HeavyItem {};
struct FragileItem {};

template<class... Ts>
struct overload : Ts ... {
    using Ts::operator()...;
};
template<class... Ts> overload(Ts...) -> overload<Ts...>; // line not needed in C++20...

int main() {
    std::variant<Fluid, LightItem, HeavyItem, FragileItem> package;

    std::visit(overload{
        [](Fluid &) { std::cout << "fluid\n"; },
        [](LightItem &) { std::cout << "light item\n"; },
        [](HeavyItem &) { std::cout << "heavy item\n"; },
        [](FragileItem &) { std::cout << "fragile\n"; }
    }, package);
}
*/