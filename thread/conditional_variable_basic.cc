// 引用元:
// https://qiita.com/hiro4669/items/bffbfa93312f2b7f6870#condition_variable

#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

std::mutex mtx;
std::condition_variable cv;

void worker1() {
  std::cout << "Worker1 start  - 2秒待機" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(2));
  // finish preparing
  std::cout << "Worker1        - mtxロック取得を試みる" << std::endl;
  {
    std::lock_guard<std::mutex> lock(mtx);
  }
  std::cout << "Worker1 finish - mtxロックの取得・解放済み - cv.notify_all()" << std::endl;
  cv.notify_all(); // cvで待機しているスレッドに通知
}

void worker2() {
  std::cout << "Worker2 start" << std::endl;
  {
    std::cout << "Worker2        - mtxロック取得を試みる" << std::endl;
    std::unique_lock<std::mutex> uniq_lk(mtx); // ここでロックされる
    std::cout << "Worker2        - mtx取得済み からの cv.wait() する. mtx解放済み" << std::endl;
    cv.wait(uniq_lk); // ここで一旦ロックを開放
    std::cout << "Worker2        - 条件変数の通知を受け取り, cv.waitを抜ける" << std::endl;
  } // ここで開放
  std::cout << "Worker2 finish" << std::endl;
}

int main() {
  std::thread th1(worker1);
  std::thread th2(worker2);
  std::thread th3(worker2);
  th1.join();
  th2.join();
  th3.join();
  return 0;
}
