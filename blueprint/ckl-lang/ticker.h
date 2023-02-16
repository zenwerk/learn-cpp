#ifndef AWESOMEPROJECTCPP_TICKER_H
#define AWESOMEPROJECTCPP_TICKER_H

#include <list>

#include "instrument.h"
#include "timer.h"
#include "utils.h"
#include "wait_group.h"

class Ticker {
  WaitGroup wg;
  Timer timer;
  std::list<std::shared_ptr<Instrument>> waiting_list;
  std::list<std::shared_ptr<Instrument>> instruments;
  std::atomic<bool> done{false};

  void register_instruments() {
    if (waiting_list.empty()) return;
    for (const auto& inst: waiting_list) {
      inst->set_waitgroup(&wg);
      instruments.push_back(inst);
    }

    waiting_list.clear();
  }

public:
  // サブスクライバー登録関数
  void add_instrument(std::shared_ptr<Instrument> &inst) {
    waiting_list.push_back(inst);
  }

  void send_tick(unsigned long long elapsed) {
    for (const auto& sub: instruments) {
      sub->tick(elapsed);
    }
  }

  void print_current_time() {
    auto elapsed = timer.get_elapsed<std::chrono::microseconds>();
    print("current elapsed time: " + std::to_string(elapsed));
  }

  unsigned long long get_current_time() {
    return timer.get_elapsed<std::chrono::microseconds>();
  }

  void run() {
    timer.start();
    while (!done) {
      std::this_thread::sleep_for(std::chrono::microseconds(2));
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

#endif //AWESOMEPROJECTCPP_TICKER_H
