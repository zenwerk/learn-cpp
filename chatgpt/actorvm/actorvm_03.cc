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

class ActorSystem;

class Actor {
public:
  Actor(ActorSystem &actor_system)
    : actor_system_(actor_system), is_running_(true), actor_thread_(&Actor::run, this) {}

  virtual void receive(const std::string &message) = 0;

  void send(std::shared_ptr<Actor> receiver, const std::string& message) {
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
        std::string message = std::move(messages_.front());
        messages_.pop();
        lock.unlock();
        receive(message);
      }
    }
  }

  void enqueue_message(const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    messages_.push(message);
    cv_.notify_one();
  }


protected:
  ActorSystem &actor_system_;
  std::atomic<bool> is_running_;
  std::thread actor_thread_;
  std::queue<std::string> messages_;
  std::mutex mutex_;
  std::condition_variable cv_;
};

class ActorSystem {
public:
  std::shared_ptr<Actor> spawn(std::function<std::shared_ptr<Actor>(ActorSystem &)> actor_constructor) {
    std::shared_ptr<Actor> actor = actor_constructor(*this);
    std::lock_guard<std::mutex> lock(actors_mutex_);
    actors_[actor.get()] = actor;
    return actor;
  }

  template<typename T, typename... Args>
  std::shared_ptr<T> spawn(Args&&... args) {
    static_assert(std::is_base_of<Actor, T>::value, "T must be derived from Actor");
    std::shared_ptr<T> actor = std::make_shared<T>(*this, std::forward<Args>(args)...);
    actors_[actor.get()] = actor;
    return actor;
  }


private:
  std::map<Actor *, std::shared_ptr<Actor>> actors_;
  std::mutex actors_mutex_;
};

//void Actor::send(std::shared_ptr<Actor> receiver, const std::string &message) {
//  actor_system_.send(this, receiver.get(), message);
//}

class MyActor : public Actor {
public:
  using Actor::Actor;

  void receive(const std::string &message) override {
    std::cout << "Received message: " << message << std::endl;
    if (message == "stop") {
      stop();
    }
  }
};

int main() {
  ActorSystem actor_system;
  auto actor1 = actor_system.spawn([](ActorSystem &actor_system) { return std::make_shared<MyActor>(actor_system); });
  auto actor2 = actor_system.spawn<MyActor>();

  actor1->send(actor2, "Hello from actor 1!");
  actor2->send(actor1, "Hello from actor 2!");
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  actor1->send(actor1, "stop");
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  actor2->send(actor2, "stop");

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  return 0;
}
