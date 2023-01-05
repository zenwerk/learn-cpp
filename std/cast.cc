#include <iostream>
#include <memory>


struct Base {
  int i{};

  Base() = default;

  explicit Base(int i_) : i(i_) {}

  virtual void print() const {
    std::cout << "Base:" << i << std::endl;
  }

  void non_virtual_print() const {
    std::cout << __FUNCTION__ << std::endl;
  }
};

struct Derived : Base {
  float f;
  using Base::Base;

  void print() const override {
    std::cout << "Derived:" << i << std::endl;
  }

  void non_virtual_print() const {
    std::cout << __FUNCTION__ << ": " << f << std::endl;
  }
};


void shared_ptr_static_pointer_cast() {
  std::cout << "=================\nstatic_pointer_cast\n=================\n";

  // Case1
  {
    std::cout << "Base -> Derived" << std::endl;
    std::shared_ptr<Base> base(new Base(1));
    std::shared_ptr<Derived> derived = std::static_pointer_cast<Derived>(base);
    if (base == derived) {
      base->print();
      derived->print();
      derived->non_virtual_print();
    } else {
      std::cout << "Case1: base != derived" << std::endl;
    }
  }

  // Case2
  {
    std::cout << "Derived -> Base" << std::endl;
    std::shared_ptr<Derived> derived(new Derived(2));
    std::shared_ptr<Base> base = std::static_pointer_cast<Base>(derived);
    derived->f = 3.14;
    if (derived == base) {
      derived->print();
      base->print();
      base->non_virtual_print();
    } else {
      std::cout << "Case1: derived != base" << std::endl;
    }
  }
}

void shared_ptr_dynamic_pointer_cast() {
  std::cout << "===================\ndynamic_pointer_cast\n===================\n";

  // Case1
  {
    std::shared_ptr<Derived> derived(new Derived(1));
    std::shared_ptr<Base> base = std::static_pointer_cast<Base>(derived);
    if (std::shared_ptr<Derived> result = std::dynamic_pointer_cast<Derived>(base)) {
      result->f = 3.14;
      result->print();
      result->non_virtual_print();
    } else {
      std::cout << "Case1 Failed: std::dynamic_pointer_cast<Derived>(Base)" << std::endl;
    }
  }

  // Case2
  {
    std::shared_ptr<Derived> derived(new Derived(1));
    if (std::shared_ptr<Base> result2 = std::dynamic_pointer_cast<Base>(derived)) {
      result2->print();
    } else {
      std::cout << "Case2 Failed: std::dynamic_pointer_cast<Base>(Derived)" << std::endl;
    }
  }

  // Case3
  {
    std::shared_ptr<Base> base(new Base(99));
    if (std::shared_ptr<Derived> result = std::dynamic_pointer_cast<Derived>(base)) {
      result->print();
      result->non_virtual_print();
    } else {
      std::cout << "Case3 Failed: std::dynamic_pointer_cast<Derived>(Base)" << std::endl;
    }
  }
}

void unique_ptr_cast() {
  std::cout << "=====================\nunique_ptr_cast()\n=====================\n";

  // dynamic_cast: Base -> Derived
  {
    auto base = std::make_unique<Base>(1);
    std::unique_ptr<Derived> derived;
    if (auto result = dynamic_cast<Derived *>(base.get())) {
      base.release();
      derived.reset(result);
    } else {
      std::cout << "Failed: dynamic_cast<Derived*>(base.get())" << std::endl;
    }
    if (derived) derived->print();
  }

  // dynamic_case: Derived -> Base
  {
    auto derived = std::make_unique<Derived>(2);
    std::unique_ptr<Base> base;
    if (auto result = dynamic_cast<Base *>(derived.get())) {
      derived.release();
      base.reset(result);
    } else {
      std::cout << "Failed: dynamic_cast<Base*>(derived.get())" << std::endl;
    }
    if (base) base->print();
  }

  // static_cast: Base -> Derived
  {
    auto base = std::make_unique<Base>(1);
    std::unique_ptr<Derived> derived;
    if (auto result = static_cast<Derived *>(base.get())) {
      base.release();
      derived.reset(result);
    } else {
      std::cout << "Failed: static_cast<Derived*>(base.get())" << std::endl;
    }
    if (derived) derived->print();
  }

  // static_cast: Derived -> Base
  {
    auto derived = std::make_unique<Derived>(2);
    std::unique_ptr<Base> base;
    if (auto result = static_cast<Base *>(derived.get())) {
      derived.release();
      base.reset(result);
    } else {
      std::cout << "Failed: static_cast<Base*>(derived.get())" << std::endl;
    }
    if (base) base->print();
  }
}

int main() {
  shared_ptr_static_pointer_cast();
  shared_ptr_dynamic_pointer_cast();

  unique_ptr_cast();

  return 0;
}