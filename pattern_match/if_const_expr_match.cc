#include <iostream>
#include <string>
#include <variant>
#include <vector>


using MyVariant = std::variant<int, std::string, double, float>;

template<typename T>
constexpr bool false_v = false;


int main() {
  std::vector<MyVariant> vs{1, "foo", 3.14f, 1.414};

  // パターンマッチ関数群
  // visit で適用するラムダ式の返り値の型は１つに統一すること
  auto match_funcs = [](auto &&val) {
    using T = std::decay_t<decltype(val)>;
    if constexpr (std::is_same_v<T, int>)
      std::cout << "int: " << val << std::endl;
    else if constexpr (std::is_same_v<T, std::string>)
      std::cout << "string: " << val << std::endl;
    else if constexpr (std::is_same_v<T, double>)
      std::cout << "double: " << val << std::endl;
    else if constexpr (std::is_same_v<T, float>)
      std::cout << "float: " << val << std::endl;
    else
      // 未対応の型が残っているとき、コンパイルに失敗するようにする
      static_assert(false_v<T>);
  };

  // マッチさせる
  for (auto i: vs)
    std::visit(match_funcs, i);

  return 0;
}