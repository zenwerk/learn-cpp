#include <iostream>
#include <cmath>

// T=int のようにするとデフォルト型を指定できる
template<class T=int>
struct Point {
  T x, y;

  Point(T x_, T y_) : x{x_}, y{y_} {}

  T distance(const Point &point) const {
    T dx = x - point.x;  // X座標の差を求める
    T dy = y - point.y;  // Y座標の差を求める
    return sqrt(dx * dx + dy * dy); // 三平方の定理
  }

  void print() {
    std::cout << "(" << x << ", " << y << ")" << std::endl;
  }
};

// テンプレートの特殊化
// 特定の型のときだけ、特別な処理をしたいときに使う記法
template<>
struct Point<long> {
  long x, y;

  Point(long x_, long y_) : x{x_}, y{y_} {}

  long distance(const Point &point) const {
    long dx = x - point.x;  // X座標の差を求める
    long dy = y - point.y;  // Y座標の差を求める
    return static_cast<long>(sqrt(dx * dx + dy * dy)); // 三平方の定理
  }

  void print() {
    std::cout << "This is long! (" << x << ", " << y << ")" << std::endl;
  }
};


int main() {
  Point<int> pi{2, 3};     // 整数型の座標
  Point<double> pd{2.3, 3.9}; // 浮動小数点数型の座標

  Point<> p_default{10, 20}; // デフォルトテンプレート型が適用される

  Point<long> pl{10, 20}; // テンプレートの特殊化

  pi.print();
  pd.print();
  p_default.print();
  pl.print();

  return 0;
}