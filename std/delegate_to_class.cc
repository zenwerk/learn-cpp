// from: https://stackoverflow.com/questions/21303412/c-return-template-function
#include <iostream>

static bool foo() {
  return true;
}

static char bar() {
  return 'a';
}

template<class T>
struct uniRndTypeHelper;

template<>
struct uniRndTypeHelper<bool> {
  static bool (*get())() { return foo; }
};

template<>
struct uniRndTypeHelper<char> {
  static char (*get())() { return bar; }
};

template<typename T>
T (*uniRndType())() {
  return uniRndTypeHelper<T>::get();
}

int main() {
  auto a = uniRndType<bool>();
  auto b = uniRndType<char>();

  std::cout << a() << std::endl;
  std::cout << b() << std::endl;
}