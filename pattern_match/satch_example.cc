//
// https://github.com/Naotonosato/Satch
//

#include <iostream>
#include <string>
#include <variant>
#include <vector>

#include "include/pattern_match.hpp"

using MyVariant = std::variant<int, std::string, double, float>;

int main() {
  MyVariant a = 1;
  std::vector<MyVariant> vs{1, "foo", 3.14f, 1.414};
  std::variant<int, char, std::string> v = 3;

  // lambda の返り値の型はすべて同じじゃないとコンパイルエラー
  auto result = satch::Match{v}(
    satch::Type<int>(), [](auto &&val) {
      return 0;
    },
    satch::Type<char>(), [](auto &&val) {
      return 1;
    },
    satch::Type<std::string>(), [](auto &&val) {
      return 2;
    },
    // Default 説がないとコンパイルエラー
    satch::Default(), [](auto &&variant) {
      std::cout << "variant.index() = " << variant.index() << std::endl;
      return 3;
    }
  );

  std::cout << "Result is " << result << std::endl;

  return 0;
}