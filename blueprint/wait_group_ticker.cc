#include <thread>
#include <list>

#include "ticker.h"
#include "instrument.h"


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