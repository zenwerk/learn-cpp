#include <iostream>

template<typename... Ts> class overloaded_lambdas : public Ts ... { public: using Ts::operator()...; };
// Deduction Guide
template<typename... Ts> overloaded_lambdas(Ts &&...) -> overloaded_lambdas<Ts...>;


int main() {
    overloaded_lambdas print_type{
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