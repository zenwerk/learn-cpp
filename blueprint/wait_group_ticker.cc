#include <thread>
#include <list>

#include "timer.h"
#include "task.h"
#include "wait_group.h"
#include "instrument.h"


//auto compare = [](const std::shared_ptr<Task>& a, const std::shared_ptr<Task>& b) { return a->wait > b->wait; };
//class Instrument {
//  WaitGroup *wg{nullptr};
//  std::string name;
//
//  std::priority_queue<
//    std::shared_ptr<Task>,
//    std::vector<std::shared_ptr<Task>>,
//    decltype(compare)
//  > q{compare};
////  LockedQueue<Task, decltype(q)> lq{1000, q};
//
//public:
//  explicit Instrument(std::string n) : name(std::move(n)) {}
//
//  void set_waitgroup(WaitGroup *wait_group) {
//    wg = wait_group;
//  }
//
//  void add_task(const std::shared_ptr<Task>& task) {
//    q.push(task);
//    print(name + " queue size:" + std::to_string(q.size()));
//  }
//
//  void tick(unsigned long long elapsed) {
//    for(;;) {
//      if (q.empty()) break;
//      auto t = q.top();
//      if (t->wait <= elapsed) {
//        print("------------");
//        print(std::to_string(t->wait) + " <= " + std::to_string(elapsed));
//        t->run();
//        print(name + " queue size -> " + std::to_string(q.size()));
//        q.pop();
//        print(name + " popped queue size -> " + std::to_string(q.size()));
//      } else {
//        break;
//      }
//    }
//    wg->Done();
//  }
//};


class Ticker {
  WaitGroup wg;
  Timer timer;
  std::list<Instrument> waiting_list;
  std::list<std::shared_ptr<Instrument>> instruments;
  std::atomic<bool> done{false};

  void register_instruments() {
    if (waiting_list.empty()) return;
    for (auto inst: waiting_list) {
      inst.set_waitgroup(&wg);
      instruments.push_back(std::make_shared<Instrument>(inst));
    }

    waiting_list.clear();
  }

public:
  // サブスクライバー登録関数
  void add_instrument(Instrument &inst) {
    waiting_list.push_back(inst);
  }

  void send_tick(unsigned long long elapsed) {
    for (const auto& sub: instruments) {
      sub->tick(elapsed);
    }
  }

  void run() {
    timer.start();
    while (!done) {
      register_instruments();
      wg.Add(instruments.size());
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
  Instrument inst1{"Instrument_1"};
  Instrument inst2{"Instrument_2"};
  Instrument inst3{"Instrument_3"};
  Instrument inst4{"Instrument_4"};

  Ticker ticker;

  auto t1 = std::make_shared<Task>(Task{"Task_1", 1000000});
  auto t2 = std::make_shared<Task>(Task{"Task_2", 2000000});
  auto t3 = std::make_shared<Task>(Task{"Task_3", 3000000});
  auto t4 = std::make_shared<Task>(Task{"Task_4", 4000000});

  inst1.add_task(t4);
  inst1.add_task(t1);
  inst1.add_task(t2);
  inst1.add_task(t3);

  inst2.add_task(t1);
  inst2.add_task(t4);
  inst2.add_task(t2);
  inst2.add_task(t3);

  inst3.add_task(t3);
  inst3.add_task(t4);
  inst3.add_task(t1);
  inst3.add_task(t2);

  inst4.add_task(t3);
  inst4.add_task(t4);
  inst4.add_task(t1);
  inst4.add_task(t2);

  ticker.add_instrument(inst1);
  ticker.add_instrument(inst2);
  ticker.add_instrument(inst3);

  std::thread tick(&Ticker::run, &ticker);

  std::this_thread::sleep_for(std::chrono::seconds{1});
  ticker.add_instrument(inst4);

  std::this_thread::sleep_for(std::chrono::seconds{5});
  ticker.stop();

  tick.join();

  return 0;
}