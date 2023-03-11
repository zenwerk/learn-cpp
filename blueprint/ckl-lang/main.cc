#include <thread>
#include <list>

#include "ticker.h"
#include "instrument.h"


int main() {
  auto inst1 = std::make_shared<Instrument>(Instrument{"Instrument_1"});
  auto inst2 = std::make_shared<Instrument>(Instrument{"Instrument_2"});
  auto inst3 = std::make_shared<Instrument>(Instrument{"Instrument_3"});
  auto inst4 = std::make_shared<Instrument>(Instrument{"Instrument_4"});

  Ticker ticker;

  auto t1 = std::make_shared<Task>(Task{"Task_1", 1000000});
  auto t2 = std::make_shared<Task>(Task{"Task_2", 2000000});
  auto t3 = std::make_shared<Task>(Task{"Task_3", 3000000});
  auto t4 = std::make_shared<Task>(Task{"Task_4", 4000000});

  inst1->add_task(t4);
  inst1->add_task(t1);
  inst1->add_task(t2);
  inst1->add_task(t3);

  inst2->add_task(t1);
  inst2->add_task(t4);
  inst2->add_task(t2);
  inst2->add_task(t3);

  inst3->add_task(t3);
  inst3->add_task(t4);
  inst3->add_task(t1);
  inst3->add_task(t2);

  inst4->add_task(t3);
  inst4->add_task(t4);
  inst4->add_task(t1);
  inst4->add_task(t2);

  ticker.add_instrument(inst1);
  ticker.add_instrument(inst2);
  ticker.add_instrument(inst3);

  std::thread tick(&Ticker::run, &ticker);
  //std::thread tick(&Ticker::run_with_timespec_get, &ticker);

  std::this_thread::sleep_for(std::chrono::seconds{1});
  ticker.add_instrument(inst4);

//  std::this_thread::sleep_for(std::chrono::seconds{5});

  // メインループ
  const char delim = ' ';
  for (std::string buf {}; std::cout << "> ", std::getline(std::cin, buf);) {
    std::string top;
    std::vector<std::string> tokens;
    tokenize(buf, delim, tokens);
    if (!tokens.empty()) {
      top = tokens[0];
      tokens.erase(tokens.begin());
    }

    if (top.starts_with("e")) {
      auto current = ticker.get_current_time();
      auto wait_ = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::seconds{3}).count();

      auto new_task = std::make_shared<Task>(Task{"New Task", wait_ + current});
      inst1->add_task(new_task);
      std::cout << "new task: " << wait_ << " + " << current << std::endl;

    } else if (top == "c") {
      ticker.print_current_time();
    } else if (top == "q") {
      break;
    } else {
      std::cout << "unknown message: " << buf << std::endl;
    }
  }

  // 終了処理
  ticker.stop();
  tick.join();

  return 0;
}