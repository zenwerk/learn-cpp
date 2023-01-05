#include <iostream>


class Base {
public:
  Base() {
    std::cout << __FUNCTION__ << std::endl;
  }

  ~Base() {
    std::cout << __FUNCTION__ << std::endl;
  }

  void x() {
    std::cout << __FUNCTION__ << std::endl;
  }

  void f() {
    std::cout << __FUNCTION__ << std::endl;
  }
};

class Derived : public Base {
public:
  Derived() {
    std::cout << __FUNCTION__ << std::endl;
  }

  ~Derived() {
    std::cout << __FUNCTION__ << std::endl;
  }

  void f() {
    std::cout << __FUNCTION__ << std::endl;
  }
};

int main(int argc, char const *argv[]) {
  {
    std::cout << "=== Call Constructor ===" << std::endl;
    Derived derived;
    derived.x(); // call Base::x
    derived.f(); // call Derived::f
    std::cout << "=== Call Destructor ===" << std::endl;
  }

  {
    std::cout << "=== Call Constructor With Pointer ===" << std::endl;
    Base *b = new Derived();
    b->f();
    std::cout << "=== Call (non virtual) Destructor With Pointer ===" << std::endl;
    delete b;
  }

  {
    std::cout << "=== Call Constructor With Pointer ===" << std::endl;
    Base *b = new Derived();
    b->f();
    std::cout << "=== Not deleted (memory leak) ===" << std::endl;
  }

  return 0;
}
