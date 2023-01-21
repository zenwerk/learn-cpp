#ifndef TIMER_H
#define TIMER_H

#include <chrono>

// from: https://codereview.stackexchange.com/a/225927
class Timer {
  using clock = std::chrono::steady_clock;
  clock::time_point start_time_ = {};
  clock::duration elapsed_time_ = {};

public:
  [[nodiscard]] bool is_running() const {
    return start_time_ != clock::time_point{};
  }

  void start() {
    if (!is_running()) {
      start_time_ = clock::now();
    }
  }

  void stop() {
    if (is_running()) {
      elapsed_time_ += clock::now() - start_time_;
      start_time_ = {};
    }
  }

  void reset() {
    start_time_ = {};
    elapsed_time_ = {};
  }

  clock::duration get_elapsed() {
    auto result = elapsed_time_;
    if (is_running()) {
      result += clock::now() - start_time_;
    }
    return result;
  }

  template<class T>
  long long get_elapsed() {
    auto elapsed = get_elapsed();
    return std::chrono::duration_cast<T>(elapsed).count();
  }

};

#endif
