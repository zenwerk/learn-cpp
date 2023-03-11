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
      // TODO: CPU使用率を減らすためスリープしているが人間が遅延を感じるのは 0.1s = 100ms からなので許容範囲内か？
      std::this_thread::sleep_for(std::chrono::microseconds (500));
      register_instruments();
      wg.Add(instruments.size());
      unsigned long long elapsed = timer.get_elapsed<std::chrono::microseconds>();
      send_tick(elapsed);
      wg.Wait();
    }
  }

  /*
  void run_with_timespec_get() {
    timespec ts{}, ts_last{};
    if (timespec_get(&ts_last, TIME_UTC) < 0) {
      print("ERROR: timespec_get() failed");
      return;
    }

    while (!done) {
      register_instruments();

      timespec_get(&ts, TIME_UTC);
      delta_nsec = 1000000000LL * (ts.tv_sec - ts_last.tv_sec); // 10億 * (秒差分)
      delta_nsec += ts.tv_nsec - ts_last.tv_nsec;               // nano秒差分

      if (delta_nsec > 0) {
        ts_last = ts; // 呼び出し時刻を更新

        if (delta_nsec < 1000000000LL) { // 経過時間が1秒未満
          // nano秒をマイクロ秒に変換
          send_tick(delta_nsec / 1000);
        } else {
          print("WARNING: ignored huge clock delta");
        }
      }
    }

  }
  */

  void stop() {
    timer.stop();
    done.store(true);
  }

};

#endif //AWESOMEPROJECTCPP_TICKER_H
