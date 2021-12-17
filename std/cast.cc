#include <iostream>
#include <memory>


struct Base {
    int i;

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
        std::cout << __FUNCTION__ << f << std::endl;
    }
};


void shared_ptr_static_pointer_cast() {
    std::cout << "=================\nstatic_pointer_cast\n=================\n";

    std::cout << "Base ==> Derived" <<std::endl;
    std::shared_ptr<Base> base1(new Base(1));
    std::shared_ptr<Derived> derived1 = std::static_pointer_cast<Derived>(base1);
    if (base1 == derived1) {
        base1->print();
        derived1->print();
        derived1->non_virtual_print();
    }

    std::cout << "Derived ===> Base" <<std::endl;
    std::shared_ptr<Derived> derived2(new Derived(2));
    std::shared_ptr<Base> base2 = std::static_pointer_cast<Base>(derived2);
    derived2->f = 3.14;
    if (derived2 == base2) {
        derived2->print();
        base2->print();
        base2->non_virtual_print();
    }
}

void shared_ptr_dynamic_pointer_cast() {
    std::cout << "===================\ndynamic_pointer_cast\n===================\n";
    std::shared_ptr<Derived> derived1(new Derived(1));
    std::shared_ptr<Base> base1 = std::static_pointer_cast<Base>(derived1);

    if (std::shared_ptr<Derived> result1 = std::dynamic_pointer_cast<Derived>(base1)) {
        result1->f = 3.14;
        result1->print();
        result1->non_virtual_print();
    } else {
        std::cout << "fail result1" << std::endl;
    }

    if (std::shared_ptr<Base> result2 = std::dynamic_pointer_cast<Base>(derived1)) {
        result2->print();
    } else {
        std::cout << "std::dynamic_pointer_cast<Base> Failed!" << std::endl;
    }

    std::shared_ptr<Base> base2(new Base(99));
    if (std::shared_ptr<Derived> result3 = std::dynamic_pointer_cast<Derived>(base2)) {
        result3->print();
        result3->non_virtual_print();
    } else {
        std::cout << "std::dynamic_pointer_cast<Derived> Failed!" << std::endl;
    }
}

void unique_ptr_cast() {
    std::cout << "=====================\nunique_ptr_cast()\n=====================\n";
    {
        // Base -> Derived
        auto base = std::make_unique<Base>(1);
        std::unique_ptr<Derived> derived;
        if (auto result = dynamic_cast<Derived*>(base.get())) {
            base.release();
            derived.reset(result);
        } else {
            std::cout << "Failed: dynamic_cast<Derived*>(base.get())" << std::endl;
        }
        if (derived) derived->print();
    }
    {
        // Derived -> Base
        auto derived = std::make_unique<Derived>(2);
        std::unique_ptr<Base> base;
        if (auto result = dynamic_cast<Base*>(derived.get())) {
            derived.release();
            base.reset(result);
        } else {
            std::cout << "Failed: dynamic_cast<Base*>(derived.get())" << std::endl;
        }
        if (base) base->print();
    }
    {
        // Base -> Derived
        auto base = std::make_unique<Base>(1);
        std::unique_ptr<Derived> derived;
        if (auto result = static_cast<Derived*>(base.get())) {
            base.release();
            derived.reset(result);
        } else {
            std::cout << "Failed: static_cast<Derived*>(base.get())" << std::endl;
        }
        if (derived) derived->print();
    }
    {
        // Derived -> Base
        auto derived = std::make_unique<Derived>(2);
        std::unique_ptr<Base> base;
        if (auto result = static_cast<Base*>(derived.get())) {
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