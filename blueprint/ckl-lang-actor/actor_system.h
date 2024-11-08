#ifndef AWESOMEPROJECTCPP_ACTOR_H
#define AWESOMEPROJECTCPP_ACTOR_H

#include <iostream>
#include <thread>
#include <utility>
#include <vector>
#include <map>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>
#include <functional>
#include <atomic>
#include <any>


class ActorSystem;

class Actor : public std::enable_shared_from_this<Actor> {
public:
  explicit Actor(ActorSystem &actor_system, std::string name)
    : actor_system_(actor_system), name_(std::move(name)), is_running_(true), actor_thread_(&Actor::run, this) {}

  virtual void receive(const std::any &msg) = 0;

  template<typename Msg>
  void send(const std::shared_ptr<Actor> &receiver, const Msg &msg) {
    if (receiver.get() != this) {
      receiver->enqueue_message(msg);
    } else {
      receiver->receive(msg);
    }
  }

  void stop() {
    is_running_ = false;
    cv_.notify_one();
    actor_thread_.join();
  }

  std::string get_name() const {
    return name_;
  }

protected:
  void run() {
    while (is_running_) {
      std::unique_lock<std::mutex> lock(mutex_);
      cv_.wait(lock, [this] { return !messages_.empty() || !is_running_; });

      if (!messages_.empty()) {
        std::any msg = std::move(messages_.front());
        messages_.pop();
        lock.unlock();
        receive(msg);
      }
    }
  }

  template<typename Msg>
  void enqueue_message(const Msg &msg) {
    std::lock_guard<std::mutex> lock(mutex_);
    messages_.push(std::any(msg));
    cv_.notify_one();
  }

  ActorSystem &actor_system_;
  std::string name_;
  std::atomic<bool> is_running_;
  std::thread actor_thread_;
  std::queue<std::any> messages_;
  std::mutex mutex_;
  std::condition_variable cv_;
};

class ActorSystem {
public:
  std::shared_ptr<Actor> spawn(const std::function<std::shared_ptr<Actor>(ActorSystem &, const std::string &)> &actor_constructor, const std::string &name) {
    std::shared_ptr<Actor> actor = actor_constructor(*this, name);
    std::lock_guard<std::mutex> lock(actors_mutex_);
    if (actors_.find(name) != actors_.end()) {
      return nullptr;
    }
    actors_[name] = actor;
    return actor;
  }

  template<typename T, typename... Args>
  std::shared_ptr<T> spawn(const std::string &name, Args &&... args) {
    static_assert(std::is_base_of<Actor, T>::value, "T must be derived from Actor");
    std::shared_ptr<T> actor = std::make_shared<T>(*this, name, std::forward<Args>(args)...);
    if (actors_.find(name) != actors_.end()) {
      return nullptr;
    }
    actors_[name] = actor;
    return actor;
  }

  std::shared_ptr<Actor> get_actor(const std::string &name) {
    std::lock_guard<std::mutex> lock(actors_mutex_);
    auto it = actors_.find(name);
    if (it != actors_.end()) {
      return it->second;
    }
    return nullptr;
  }

  void remove_actor(const std::string &name) {
    std::lock_guard<std::mutex> lock(actors_mutex_);
    actors_.erase(name);
  }

private:
  std::map<std::string, std::shared_ptr<Actor>> actors_;;
  std::mutex actors_mutex_;
};


#endif //AWESOMEPROJECTCPP_ACTOR_H
