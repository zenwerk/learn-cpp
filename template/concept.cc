#include <iostream>

// 制約: 型T が draw() というメンバ関数を持っていること
template<class T>
concept Drawable = requires (T& x) {
  x.draw();
};

// テンプレート仮引数TをDrawableコンセプトを満たす必要がある
template <class T> requires Drawable<T>
void f(T& x) {
  x.draw();
}

// こういう書き方も可能
template<Drawable T>
void g(T& x) {
  x.draw();
}

class Box {
public:
  void draw() {
    std::cout << "Boxオブジェクトを描画" << std::endl;
  }
};



int main() {
  Box b;
  f(b);
  g(b);

  // int型は Drawable を満たさないのでコンパイルできない
  // int x = 0;
  // f(x);

  return 0;
}