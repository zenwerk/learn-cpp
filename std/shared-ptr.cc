#include <iostream>
#include <memory>


class Base {
public:
    virtual ~Base() = default;

    virtual void call() = 0;
};

class Derived : public Base {
public:
    explicit Derived(int i) : i_(i) {
    }

    void call() override {
        std::cout << __FUNCTION__ << ": " << i_ << std::endl;
    }

private:
    int i_ = 0;
};


// 参照で受け取ることを忘れず
void pass_shared_ptr(std::shared_ptr<Base> &v) {
    v = std::make_shared<Derived>(99);
}

void pass_shared_ptr_2(std::shared_ptr<Base> &v) {
    v.reset(new Derived(88));
}

void pass_shared_ptr_get(Base* v) {
    v = new Derived(999);
}

// std::shared_ptr を関数の引数に渡して操作したい
void passing_shared_ptr() {
    {
        std::shared_ptr<Base> v;
        pass_shared_ptr(v);
        if (v)
            v->call();
        else
            std::cout << "nullptr1" << std::endl;
    }
    {
        // This is not work !
        std::shared_ptr<Base> v;
        pass_shared_ptr_get(v.get());
        if (v)
            v->call();
        else
            std::cout << "nullptr2" << std::endl;
    }

    {
        std::shared_ptr<Base> v;
        pass_shared_ptr_2(v);
        if (v)
            v->call();
        else
            std::cout << "nullptr2" << std::endl;
    }
}


int main() {
    passing_shared_ptr();

    return 0;
}