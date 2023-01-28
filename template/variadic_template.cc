// 可変長引数テンプレート
#include <iostream>

struct Foo {
};

void showTypes() {
  std::cout << std::endl;
}

template<typename H, typename ...T>
void showTypes(const H, T ...tail) {
  std::cout << typeid(H).name() << "\t";
  showTypes(tail...);
}

// C++20
//auto add(auto a, auto b) {return a + b;}

int main() {
  showTypes(1, 'a', "b", 3.14, Foo{});

  return 0;
}