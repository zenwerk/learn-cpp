#include <thread>
#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>

// 排他的出力処理
std::mutex printMtx_;

void print(const std::string &s) {
  std::lock_guard<std::mutex> lk{printMtx_};
  std::cout << s << std::endl;
}

// from: https://blog.csdn.net/sky527759/article/details/106845160
class WaitGroup {
public:
  void Add(int incr = 1) {
    counter += incr;
    print("incr->" + std::to_string(counter));
  }

  void Done() { if (--counter <= 0) cond.notify_all(); else print("decr->" + std::to_string(counter)); }

  void Wait() {
    std::unique_lock<std::mutex> lock{mutex};
    print("Waiting...");
    cond.wait(lock, [&] { return counter <= 0; });
    print("Done!");
  }

private:
  std::mutex mutex;
  std::atomic<int> counter{0};
  std::condition_variable cond;
};


void stop(int n, WaitGroup &wg) {
  std::this_thread::sleep_for(std::chrono::seconds(n));
  print(std::to_string(n) + "秒待ちました。");
  wg.Done();
}


int main() {
  WaitGroup wg;
  std::vector<std::thread> threads;

  for (int i = 1; i < 4; i++) {
    wg.Add();
    auto th = std::thread(stop, i, std::ref(wg));
    threads.push_back(std::move(th)); // 1秒かかる処理
  }

  wg.Wait();

  for (std::thread &th: threads) {
    th.join();
  }

  return 0;
}
