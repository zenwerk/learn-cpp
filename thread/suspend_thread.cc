#include <thread>
#include <atomic>
#include <condition_variable>
#include <iostream>

class LoopController {
public:
  LoopController() : running(false), exit_flag(false), resumed(false) {
    std::cout << "constructed..." << std::endl;
    worker_thread = std::thread([this] { this->loop(); });
  }

  void start() {
    if (!running) {
      running = true;
      resumed = true;
      exit_flag = false;
    }
    cv.notify_all();
    std::cout << "起動" << std::endl;
  }

  void stop() {
    std::cout << "停止" << std::endl;
    running = false;
  }

  void exit() {
    exit_flag = true;
    running = false;
    cv.notify_all();
    if (worker_thread.joinable()) {
      worker_thread.join();
    }
  }

  ~LoopController() {
    if (running)
      exit();
  }

private:
  std::atomic<bool> running;
  std::atomic<bool> exit_flag;
  std::atomic<bool> resumed;
  std::thread worker_thread;
  std::mutex mtx;
  std::condition_variable cv;
  int cnt = 0;

  void loop() {
    while (true) {
      std::unique_lock<std::mutex> lock(mtx);
      cv.wait(lock, [&] {
        return running || exit_flag;
      });
      if (resumed) {
        resumed = false;
        std::cout << "復帰しました..." << std::endl;
      }

      if (exit_flag) {
        std::cout << "終了します..." << std::endl;
        break;
      }

      // 無限ループの処理
      std::cout << "実行中... " << ++cnt << std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }
};

int main() {
  auto loop_controller = LoopController();
  std::this_thread::sleep_for(std::chrono::seconds(1));

  loop_controller.start();
  std::this_thread::sleep_for(std::chrono::seconds(3));

  loop_controller.stop();
  std::this_thread::sleep_for(std::chrono::seconds(3));

  loop_controller.start();
  std::this_thread::sleep_for(std::chrono::seconds(2));

  loop_controller.exit();

  return 0;
}