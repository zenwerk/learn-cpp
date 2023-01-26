#include <thread>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <utility>
#include <list>
#include <vector>
#include <queue>

#include "timer.h"
#include "utils.h"
#include "task.h"


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

auto compare = [](const std::shared_ptr<Task>& a, const std::shared_ptr<Task>& b) { return a->wait > b->wait; };

class Sub {
  WaitGroup *wg{nullptr};
  std::string name;
  std::priority_queue<
    std::shared_ptr<Task>,
    std::vector<std::shared_ptr<Task>>,
    decltype(compare)
  > q{compare};

public:
  explicit Sub(std::string n) : name(std::move(n)) {}

  void set_waitgroup(WaitGroup *waitGroup) {
    wg = waitGroup;
  }

  void add_task(const std::shared_ptr<Task>& task) {
    q.push(task);
    print(name + " queue size:" + std::to_string(q.size()));
  }

  void tick(unsigned long long elapsed) {
    for(;;) {
      if (q.empty()) break;
      auto t = q.top();
      if (t->wait <= elapsed) {
        print("------------");
        print(std::to_string(t->wait) + " <= " + std::to_string(elapsed));
        t->run();
        print(name + " queue size -> " + std::to_string(q.size()));
        q.pop();
        print(name + " popped queue size -> " + std::to_string(q.size()));
      } else {
        break;
      }
    }
    wg->Done();
  }
};


class Ticker {
  WaitGroup wg;
  Timer timer;
  std::list<Sub> pre_subscribers;
  std::list<std::shared_ptr<Sub>> subscribers;
  std::atomic<bool> done{false};

  void register_subscribers() {
    if (pre_subscribers.empty()) return;
    for (auto sub: pre_subscribers) {
      sub.set_waitgroup(&wg);
      subscribers.push_back(std::make_shared<Sub>(sub));
    }

    pre_subscribers.clear();
  }

public:
  // サブスクライバー登録関数
  void add_subscriber(Sub &sub) {
    pre_subscribers.push_back(sub);
  }

  void send_tick(unsigned long long elapsed) {
    for (const auto& sub: subscribers) {
      sub->tick(elapsed);
    }
  }

  void run() {
    timer.start();
    while (!done) {
      register_subscribers();
      wg.Add(subscribers.size());
      unsigned long long elapsed = timer.get_elapsed<std::chrono::microseconds>();
      send_tick(elapsed);
      wg.Wait();
    }
  }

  void stop() {
    timer.stop();
    done.store(true);
  }

};


int main() {
  Sub sub1{"Sub_1"};
  Sub sub2{"Sub_2"};
  Sub sub3{"Sub_3"};
  Sub sub4{"Sub_4"};

  Ticker ticker;

  auto t1 = std::make_shared<Task>(Task{"Task_1", 1000000});
  auto t2 = std::make_shared<Task>(Task{"Task_2", 2000000});
  auto t3 = std::make_shared<Task>(Task{"Task_3", 3000000});
  auto t4 = std::make_shared<Task>(Task{"Task_4", 4000000});

  sub1.add_task(t4);
  sub1.add_task(t1);
  sub1.add_task(t2);
  sub1.add_task(t3);

  sub2.add_task(t1);
  sub2.add_task(t4);
  sub2.add_task(t2);
  sub2.add_task(t3);

  sub3.add_task(t3);
  sub3.add_task(t4);
  sub3.add_task(t1);
  sub3.add_task(t2);

  ticker.add_subscriber(sub1);
  ticker.add_subscriber(sub2);
  ticker.add_subscriber(sub3);

  std::thread tick(&Ticker::run, &ticker);

//  std::this_thread::sleep_for(std::chrono::seconds{1});
//  ticker.add_subscriber(sub4);

  std::this_thread::sleep_for(std::chrono::seconds{5});
  ticker.stop();

  tick.join();

  return 0;
}