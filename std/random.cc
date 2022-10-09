#include <iostream>
#include <random>

std::random_device seed_gen; // ハードウェアによる非決定性の乱数生成器 (ただし低速)
std::mt19937 engine{seed_gen()}; // メルセンヌ・ツイスタに種を設定
std::uniform_int_distribution<> dist{0, 10}; // 0〜10 の範囲で一様整数分布させる
std::normal_distribution<float> ndist{0.0, 1.0}; // 平均値0, 標準偏差1.0 で正規分布させる

int main() {
  // 一様分布乱数
  for (int i = 0; i < 10; ++i)
    // 分布クラスに乱数生成器を渡すことによって、乱数を生成する
    std::cout << dist(engine) << ", ";
  std::cout << std::endl;

  // 正規乱数
  for (int i = 0; i < 10; ++i)
    std::cout << ndist(engine) << ", ";
  std::cout << std::endl;

  return 0;
}