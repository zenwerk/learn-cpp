#include <iostream>


struct Point {
    int *x;
    int *y;

    Point() = default;

    Point(int *x_, int *y_) : x(x_), y(y_) {}

    Point(Point &&r) noexcept: x(r.x), y(r.y) {
        std::cout << "call move constructor" << std::endl;
        r.x = nullptr;
        r.y = nullptr;
    }

    Point &operator=(Point &&r) noexcept {
        std::cout << "call move assign operator" << std::endl;
        this->x = r.x;
        this->y = r.y;
        r.x = nullptr;
        r.y = nullptr;
        return *this;
    }

    void print() const {
        std::cout << "x:" << *x << " y:" << *y << std::endl;
        std::cout << "x:" << std::addressof(x) << " y:" << std::addressof(y) << std::endl;
        std::cout << "-----------------------" << std::endl;
    }

    Point(const Point &) = delete;

    Point operator=(const Point &) = delete;
};

int main() {
    int x = 1;
    int y = 2;

    Point p1{&x, &y};
    p1.print();

    Point p2 = std::move(p1);
    if (p1.x == nullptr && p1.y == nullptr)
        std::cout << "Point p1 was moved" << std::endl;
    p2.print();

    Point p3(std::move(p2));
    if (p2.x == nullptr && p2.y == nullptr)
        std::cout << "Point p2 was moved" << std::endl;
    p3.print();

    // does not call move constructor
    Point &&p4 = std::move(p3);
    p4.print();

    // call move assign operator
    p4 = Point(&x, &y);
    p4.print();
}