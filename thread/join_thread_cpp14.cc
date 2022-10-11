#include <atomic>
#include <thread>

// from: https://faithandbrave.hateblo.jp/entry/2022/09/01/034015
class join_thread {
  std::atomic<bool> _stop_request{false};
  std::thread _thread;

public:
  template <class F>
  explicit join_thread(F f) {
    _thread = std::thread{[this, g = std::move(f)] { g(_stop_request); }};
  }

  ~join_thread() { join(); }

  void request_stop() { _stop_request.store(true); }

  void join() {
    request_stop();
    if (_thread.joinable()) {
      _thread.join();
    }
  }
};

#include <chrono>
#include <iostream>
void f(std::atomic<bool> &stop_request) {
  int sum = 0;
  while (!stop_request.load()) {
    ++sum;
  }
  std::cout << sum << std::endl;
}

int main() {
  join_thread t{f};

  std::this_thread::sleep_for(std::chrono::milliseconds(3));

  t.request_stop();
  t.join();
}
