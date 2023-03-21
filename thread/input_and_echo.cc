#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

std::mutex mtx;
std::condition_variable cv;
std::queue<std::string> input_queue;
bool done = false;

void input_thread() {
  while (true) {
    std::string input;
    std::getline(std::cin, input);

    {
      std::unique_lock<std::mutex> lock(mtx);
      if (input == "exit") {
        done = true;
        break;
      }
      input_queue.push(input);
    }
    cv.notify_one();
  }
}

void echo_thread() {
  std::unique_lock<std::mutex> lock(mtx);
  while (!done || !input_queue.empty()) {
    cv.wait(lock, [] { return !input_queue.empty() || done; });

    if (!input_queue.empty()) {
      std::string input = input_queue.front();
      input_queue.pop();
      lock.unlock();

      std::cout << "Echo: " << input << std::endl;

      lock.lock();
    }
  }
}

int main() {
  std::thread t_input(input_thread);
  std::thread t_echo(echo_thread);

  t_input.join();
  cv.notify_one(); // echo_thread に input_thread が終了したことを伝える
  t_echo.join();

  return 0;
}