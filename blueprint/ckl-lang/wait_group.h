#ifndef AWESOMEPROJECTCPP_WAIT_GROUP_H
#define AWESOMEPROJECTCPP_WAIT_GROUP_H

#include <condition_variable>
#include <mutex>

// from: https://blog.csdn.net/sky527759/article/details/106845160
class WaitGroup {
public:
  void Add(int incr = 1) {
    counter += incr;
  }

  void Done() {
    if (--counter <= 0)
      cond.notify_all();
  }

  void Wait() {
    std::unique_lock<std::mutex> lock{mutex};
    cond.wait(lock, [&] { return counter <= 0; });
  }

private:
  std::mutex mutex;
  std::atomic<int> counter{0};
  std::condition_variable cond;
};

#endif //AWESOMEPROJECTCPP_WAIT_GROUP_H
