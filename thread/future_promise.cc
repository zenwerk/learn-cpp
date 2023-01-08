#include <future>
#include <iostream>
#include <thread>
#include <vector>

int computeSomething() { return 42; }

void worker(std::promise<int> p) {
  try {
    // 処理結果をPromiseに設定する
    // set_value できるのは1回のみ. 複数回呼び出すと例外
    p.set_value(computeSomething());
  } catch (...) {
    // 例外をPromiseに設定
    p.set_exception(std::current_exception());
  }
}

int asyncWorker(const std::vector<int> &data) {
  int sum = 0;
  for (int i: data) {
    // 時間のかかる処理
    std::this_thread::sleep_for(std::chrono::milliseconds{100});
    sum += i;
  }

  return sum;
}

int main() {
  std::promise<int> p;
  std::future<int> f = p.get_future();

  std::thread th{worker, std::move(p)};

  try {
    std::cout << "value: " << f.get() << std::endl;
  } catch (std::exception &e) {
    std::cout << "error: " << e.what() << std::endl;
  }

  th.join();

  //---- async
  std::vector<int> data = {1, 2, 3, 4, 5};
  // std::launch::async は別スレッドで実行
  std::future<int> f2 = std::async(std::launch::async, asyncWorker, std::ref(data));

  try {
    std::cout << "value: " << f2.get() << std::endl;
  } catch (std::exception &e) {
    std::cout << "error: " << e.what() << std::endl;
  }

  // std::launch::deferred は実行遅延. get() されたときに同じスレッドで実行される
  std::future<int> f3 = std::async(std::launch::deferred, asyncWorker, std::ref(data));
  try {
    std::cout << "value: " << f3.get() /* ここで asyncWorker が動く */ << std::endl;
  } catch (std::exception &e) {
    std::cout << "error: " << e.what() << std::endl;
  }

  return 0;
}