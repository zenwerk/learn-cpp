#include <thread>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <utility>
#include <vector>

std::mutex pmtx;

void print(const std::string &s) {
  std::lock_guard<std::mutex> lk{pmtx};
  std::cout << s << std::endl;
}

struct Worker {
  bool done = false;
  int count = 1;
  std::mutex& mtx;
  std::string s;
  std::condition_variable& cv_tick;

  Worker(std::string s_, std::mutex& mtx_, std::condition_variable& cv) : mtx(mtx_), s(std::move(s_)), cv_tick(cv) {}

  void operator()() {
    print("Start: " + s);
    while (count < 4) {
      print(s + ": lock_shared");
      std::unique_lock<std::mutex> lock{mtx};
      cv_tick.wait(lock, [this]{return !done; });
      print(s + ": loop-" + std::to_string(count));
      done = true;
      ++count;
      print(s + ": unlock_shared");
    }
  }

  void Tick() {
    print(s + ": ticked!");
    done = false;
  }

};

void ticker(std::vector<Worker>& workers, std::mutex& mtx, std::condition_variable& cv_tick) {
  int count = 1;
  while (count < 4) {
    print("ticker: lock_guard-" + std::to_string(count));
//    std::lock_guard<std::mutex> lk{mtx};
    mtx.lock();
    std::this_thread::sleep_for(std::chrono::seconds{1});
    for (Worker& w : workers)
      w.Tick();
    ++count;
    mtx.unlock();
    cv_tick.notify_all();
    print("ticker: unlock_guard-" + std::to_string(count));
  }
}

void worker_(const std::string& s, std::shared_mutex& mtx) {
  //    std::this_thread::sleep_for(std::chrono::seconds{2});
  int count = 1;
  while (count < 6) {
    std::shared_lock<std::shared_mutex> lk{mtx};
    print(s + ": " + std::to_string(count));
    ++count;
  }
}


#if 0
ticker には 各 worker が持つ何かしらのフラグへの参照を渡す必要がある？
シガニーでは、Engine のなかに Ticker がリストで登録されている
Engine が定期的に Ticker を for で回し、 Ticker.Tock でフラグをオフする
フラグがオフになったら、各Tickerが処理を実行し、フラグをオフにする
#endif

int main() {
  std::mutex mtx;
  std::condition_variable cv_tick;

  Worker w1{"Worker1", mtx, cv_tick};
  Worker w2{"Worker2", mtx, cv_tick};
  std::vector<Worker> workers = {w1, w2};

  std::thread th1{ticker, std::ref(workers), std::ref(mtx), std::ref(cv_tick)};

  std::this_thread::sleep_for(std::chrono::milliseconds{10});

  std::thread th2{w1};
  std::thread th3{w2};

//  std::thread th2{worker_, "worker1", std::ref(mtx)};
//  std::thread th3{worker_, "worker2", std::ref(mtx)};

  th1.join();
  th2.join();
  th3.join();

  return 0;
}