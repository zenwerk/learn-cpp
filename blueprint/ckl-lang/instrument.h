#ifndef AWESOMEPROJECTCPP_INSTRUMENT_H
#define AWESOMEPROJECTCPP_INSTRUMENT_H

#include <string>
#include <queue>

#include "task.h"
#include "utils.h"
#include "wait_group.h"


// TODO: 楽器ごとに MIDI 接続先があり、楽器ごとにバッファを持つ

auto compare = [](const std::shared_ptr<Task>& a, const std::shared_ptr<Task>& b) { return a->wait > b->wait; };

class Instrument {
  WaitGroup *wg{nullptr};
  std::string name;
  std::priority_queue<
    std::shared_ptr<Task>,
    std::vector<std::shared_ptr<Task>>,
    decltype(compare)
  > q{compare};

public:
  explicit Instrument(std::string n) : name(std::move(n)) {}

  void set_waitgroup(WaitGroup *wait_group) {
    wg = wait_group;
  }

  void add_task(const std::shared_ptr<Task>& task) {
    q.push(task);
    print(name + ": 残りタスク数 = " + std::to_string(q.size()));
  }

  void tick(unsigned long long elapsed) {
    for(;;) {
      if (q.empty()) break;
      auto t = q.top();
      if (t->wait <= elapsed) {
        t->run();
        q.pop();
      } else {
        break;
      }
    }

    wg->Done();
  }

};

#endif //AWESOMEPROJECTCPP_INSTRUMENT_H