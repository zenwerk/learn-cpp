/*
 * 無名関数で再帰するためのYコンビネータの実装
 * from: http://404-notfound.jp/archives/669
 */
#include <iostream>

/*
 * Yコンビネータの実装
 */
auto Y = [](auto func) {                                    // 関数`func`を受け取り
  return [=](auto &&... xs) {                             //   - 可変数引数xs を受け取り
    return func(func, std::forward<decltype(xs)>(xs)...); //     - 関数`func`に`func`自身と引数xsを適用した結果
  };                                                      //   - を返す関数
};                                                        // を返す関数が `Y`

/*
 * Zコンビネータのために必要な道具であるカリー化関数
 * Yコンビネータの `func(func, ..)` の第１引数`func`を明示的に引数`xs`化した感じ。
 */
auto curry = [](auto f) {                                                                 // 再帰したい`f`を受け取り
  return [=](auto &&... xs) {                                                         // 引数xs
    return [&](auto &&... ys) {                                                       // 引数ys
      return f(std::forward<decltype(xs)>(xs)..., std::forward<decltype(ys)>(ys)...); // f(xs..., ys...)を実行
    };                                                                                // の結果を返す関数
  };                                                                                  // ..
};                                                                                    // .

/*
 * Zコンビネータの実装
 */
auto Z = [](auto func) {
  // return curry(func)(func);
  auto curry1 = curry(func);  // `auto f` に `func` を適用したまでの無名関数が返る
  auto curry2 = curry1(func); // 内側xsに `func` を適用したまでの無名関数が返る

  return curry2; // 結果的に得られるのは`Yコンビネータ`と同じ結果
};


int main() {
  auto yAns = Y([](auto f, auto n) -> decltype(n) { return n < 2 ? n : n * f(f, n - 1); })(10);
  auto zAns = Z([](auto f, auto n) -> decltype(n) { return n < 2 ? n : n * f(f, n - 1); })(10);

  std::cout << yAns << std::endl;
  std::cout << zAns << std::endl;
}