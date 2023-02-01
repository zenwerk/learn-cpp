#include <iostream>

/**
 * テンプレートの推論補助の宣言について
 */

template<class T>
struct Point{
  T x, y;

  // コンストラクタは配列を受け取る。
  // 配列の型から、その要素型Tを直接推論はできない
  template<class Array>
  explicit Point(Array ar)
    : x{ar[0]}, y{ar[1]} {}

  void print() {
    std::cout << "(" << x << ", " << y << ")" << std::endl;
  }
};

// 推論補助の宣言
// 配列の要素型をPointWithArrayクラスの実引数とみなす宣言。
template<class Array>
Point(Array ar) -> Point<decltype(ar[0])>;

int main() {
  int a[] = {1, 2, 3};
  Point<int> p1(a);
  p1.print();

  float b[] = {1.1, 2.2};
  Point<float> p2(b);
  p2.print();

  return 0;
}