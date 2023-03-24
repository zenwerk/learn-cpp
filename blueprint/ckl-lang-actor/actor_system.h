#ifndef AWESOMEPROJECTCPP_ACTOR_H
#define AWESOMEPROJECTCPP_ACTOR_H

#include <iostream>
#include <thread>
#include <vector>
#include <map>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>
#include <functional>
#include <atomic>

#include "messages.h"

class ActorSystem;

class Actor {
public:
  explicit Actor(ActorSystem &actor_system)
    : actor_system_(actor_system), is_running_(true), actor_thread_(&Actor::run, this) {}

  virtual void receive(const Message &message) = 0;

  void send(const std::shared_ptr<Actor> &receiver, const Message &message) {
    if (receiver.get() != this) {
      receiver->enqueue_message(message);
    } else {
      receiver->receive(message);
    }
  }

  void stop() {
    is_running_ = false;
    cv_.notify_one();
    actor_thread_.join();
  }

  void run() {
    while (is_running_) {
      std::unique_lock<std::mutex> lock(mutex_);
      cv_.wait(lock, [this] { return !messages_.empty() || !is_running_; });

      if (!messages_.empty()) {
        Message message = std::move(messages_.front());
        messages_.pop();
        lock.unlock();
        receive(message);
      }
    }
  }

  void enqueue_message(const Message &message) {
    std::lock_guard<std::mutex> lock(mutex_);
    messages_.push(message);
    cv_.notify_one();
  }

protected:
  ActorSystem &actor_system_;
  std::atomic<bool> is_running_;
  std::thread actor_thread_;
  std::queue<Message> messages_;
  std::mutex mutex_;
  std::condition_variable cv_;
};

class ActorSystem {
public:
  std::shared_ptr<Actor> spawn(const std::function<std::shared_ptr<Actor>(ActorSystem &)> &actor_constructor) {
    std::shared_ptr<Actor> actor = actor_constructor(*this);
    std::lock_guard<std::mutex> lock(actors_mutex_);
    actors_[actor.get()] = actor;
    return actor;
  }

  template<typename T, typename... Args>
  std::shared_ptr<T> spawn(Args &&... args) {
    static_assert(std::is_base_of<Actor, T>::value, "T must be derived from Actor");
    std::shared_ptr<T> actor = std::make_shared<T>(*this, std::forward<Args>(args)...);
    actors_[actor.get()] = actor;
    return actor;
  }

private:
  std::map<Actor *, std::shared_ptr<Actor>> actors_;
  std::mutex actors_mutex_;
};


#endif //AWESOMEPROJECTCPP_ACTOR_H
