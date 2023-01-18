#include <thread>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <utility>
#include <list>

std::mutex pmtx;

void print(const std::string &s) {
  std::lock_guard<std::mutex> lk{pmtx};
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


class Sub {
  WaitGroup *wg;
  std::string name;

public:
  explicit Sub(std::string n) : name(std::move(n)) {}

  void set_waitgroup(WaitGroup *waitGroup) {
    wg = waitGroup;
  }

  void tick() {
    std::cout << name << ": tick!" << std::endl;
    wg->Done();
  }
};


// TODO: まずは固定メンバーで実装する
// TODO: その後にいつでもメンバーを追加できるようにする
class Ticker {
  WaitGroup wg;
  std::list<Sub> pre_subscribers;
  std::list<Sub> subscribers;
  std::atomic<bool> done{false};

  void register_subscribers() {
    if (pre_subscribers.empty()) return;
    for (auto sub: pre_subscribers) {
      sub.set_waitgroup(&wg);
      subscribers.push_back(sub);
    }

    pre_subscribers.clear();
  }

public:
  // サブスクライバー登録関数
  void add_subscriber(Sub &sub) {
    pre_subscribers.push_back(sub);
  }

  // TODO: Queueを追加しジョブを処理する
  void send_tick() {
    for (auto sub: subscribers) {
      sub.tick();
    }
  }

  void run() {
    while (!done) {
      register_subscribers();
      wg.Add(subscribers.size());
      std::this_thread::sleep_for(std::chrono::seconds{1});
      send_tick();
      wg.Wait();
    }
  }

  void stop() {
    done.store(true);
  }

};


int main() {
  Sub sub1{"sub1"};
  Sub sub2{"sub2"};
  Sub sub3{"sub3"};
  Sub sub4{"sub4"};

  Ticker ticker;

  ticker.add_subscriber(sub1);
  ticker.add_subscriber(sub2);
  ticker.add_subscriber(sub3);

  std::thread tick(&Ticker::run, &ticker);

  std::this_thread::sleep_for(std::chrono::seconds{2});
  ticker.add_subscriber(sub4);

  std::this_thread::sleep_for(std::chrono::seconds{2});
  ticker.stop();

  tick.join();

  return 0;
}