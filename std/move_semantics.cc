#include <iostream>
#include <memory>


struct Point {
  int *x;
  int *y;

  Point() = default;

  Point(int *x_, int *y_) : x(x_), y(y_) {}

  Point(Point &&r) noexcept: x(r.x), y(r.y) {
    std::cout << "Call move constructor: " << __FUNCTION__ << std::endl;
    r.x = nullptr;
    r.y = nullptr;
  }

  Point &operator=(Point &&r) noexcept {
    std::cout << "Call move assign operator: " << __FUNCTION__ << std::endl;
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

  // コピー演算を削除する
  Point(const Point &) = delete;

  Point operator=(const Point &) = delete;
};

void pointExample() {
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

  // 右辺値参照で受け取るとムーブコンストラクタは呼ばれない
  Point &&p4 = std::move(p3);
  p4.print();

  // ムーブ代入が呼ばれる
  p4 = Point(&x, &y);
  p4.print();
}

struct U {
  std::unique_ptr<int> val;

  explicit U(std::unique_ptr<int> val_) noexcept: val(std::move(val_)) {}

  explicit U(int &&val_) noexcept: val(std::make_unique<int>(val_)) {}

  U(U &&r) noexcept: val(std::move(r.val)) {
    std::cout << "Call move constructor: " << __FUNCTION__ << std::endl;
  }

  U &operator=(U &&r) noexcept {
    std::cout << "Call move assign operator: " << __FUNCTION__ << std::endl;
    this->val = std::move(r.val);
    return *this;
  }

  void print() const {
    if (val)
      std::cout << "val:" << *val << std::endl;
    else
      std::cout << "val was moved" << std::endl;
  }
};

void uExample() {
  U u1{std::make_unique<int>(1)};
  u1.print();

  U u2 = std::move(u1);
  u2.print();
  u1.print(); // ムーブされているので `val was moved`

  // 右辺値参照で受け取ると u3 と u2 は同じものを指す
  U &&u3 = std::move(u2);
  u3.print();
  u2.print();

  // u3 = u2 なので print() の値も両方とも `2` に変わる
  u3 = U(std::make_unique<int>(2));
  u3.print();
  u2.print();

  // `U`構造体の unique_ptr を作成する
  auto a = std::make_unique<U>(U{std::make_unique<int>(3)});
  a->print();
  std::unique_ptr<U> b;
  b = std::move(a);
  b->print();
  if (a) {
    std::cout << "a はムーブされていない" << std::endl;
    a->print();
  }

  // `U`のunique_ptr の中の val だけ move する、なんてこともできる
  auto bval = std::move(b->val);
  std::cout << *bval << std::endl;

  if (b->val)
    std::cout << b->val << std::endl;
  else
    std::cout << "b->val moved" << std::endl;
}

void foo(int *ptr) {
  *ptr = 999;
}

void bar(std::unique_ptr<int> &up) {
  *up = 100;
}

void smartPointer() {
  auto a = std::make_unique<int>(0);
  std::cout << "------------------" << std::endl;
  std::cout << *a << std::endl;
  foo(a.get());
  std::cout << *a << std::endl;
  bar(a);
  std::cout << *a << std::endl;
}

int main() {
  pointExample();

  uExample();

  smartPointer();

  return 0;
}