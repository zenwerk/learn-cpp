#include <iostream>
#include <string>
#include <variant>
#include <vector>


template<typename... Ts>
class OverloadedLambdas : public Ts ... {
public:
  using Ts::operator()...;
};

template<typename... Ts> OverloadedLambdas(Ts &&...) -> OverloadedLambdas<Ts...>;

using MyVariant = std::variant<int, std::string, double, float>;


int main() {
  std::vector<MyVariant> vs{1, "foo", 3.14f, 1.414};

  // パターンマッチ関数群
  // visit で適用させるラムダ式の返り値の型は1つに統一すること
  OverloadedLambdas match_funcs{
    [](int &val) { std::cout << "int: " << val << std::endl; },
    [](std::string &val) { std::cout << "string: " << val << std::endl; },
    [](double &val) { std::cout << "double: " << val << std::endl; },
    [](float &val) { std::cout << "float: " << val << std::endl; },
  };

  // マッチさせる
  for (auto i: vs)
    std::visit(match_funcs, i);

  return 0;
}