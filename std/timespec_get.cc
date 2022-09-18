#include <chrono>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <thread>

unsigned long wallclock = 0;

void loop() {
  long long delta_nsec;

  timespec ts{}, ts_last{};
  timespec_get(&ts_last, TIME_UTC);

  while (true) {
    timespec_get(&ts, TIME_UTC);

    delta_nsec = 1000000000LL * (ts.tv_sec - ts_last.tv_sec); /* 10億 * (秒差分) */
    delta_nsec += ts.tv_nsec - ts_last.tv_nsec;               /* nano秒差分 */

    if (delta_nsec > 0) {
      ts_last = ts;

      if (delta_nsec < 1000000000LL) {
        /* 経過時間が1秒未満 */
        wallclock += delta_nsec;
        std::cout << "wallclock: " << wallclock << std::endl;
      } else {
        std::cout << "large delta: " << delta_nsec << std::endl;
        break;
      }
    }

    if (wallclock > 1000000000LL)
      break ;

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

}

int main() {
  timespec ts{};

  timespec_get(&ts, TIME_UTC);
  printf("sec=%ld  nanosecond=%09ld\n", ts.tv_sec, ts.tv_nsec);
  timespec_get(&ts, TIME_UTC);
  printf("sec=%ld  nanosecond=%09ld\n", ts.tv_sec, ts.tv_nsec);

  // 秒未満の値をミリ秒で取得
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::nanoseconds{ts.tv_nsec});
  std::cout << "ms:" << ms.count() << std::endl;

  // 秒以上の値だけを日時フォーマットで出力
  std::cout << std::ctime(&ts.tv_sec) << std::endl;

  loop();

  return 0;
}