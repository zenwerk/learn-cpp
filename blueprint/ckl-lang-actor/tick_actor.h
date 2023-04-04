#ifndef AWESOMEPROJECTCPP_TICK_ACTOR_H
#define AWESOMEPROJECTCPP_TICK_ACTOR_H


#include <chrono>
#include <vector>
#include <map>
#include <mutex>
#include <any>


struct TickMessage {
  std::chrono::microseconds elapsed_time{};
  std::chrono::microseconds accumulated_time{};
  std::shared_ptr<Actor> sender{};
};

struct TickResponse {
  std::shared_ptr<Actor> sender{};
};

class TickerActor : public Actor {
public:
  using Actor::Actor;

  void add_actor(const std::shared_ptr<Actor> &actor) {
    std::lock_guard<std::mutex> lock(registered_actors_mutex_);
    registered_actors_.push_back(actor);
    pending_responses_[actor.get()] = false;
  }

  void remove_actor(const std::shared_ptr<Actor> &actor) {
    std::lock_guard<std::mutex> lock(registered_actors_mutex_);
    // registered_actors_.erase(std::remove(registered_actors_.begin(), registered_actors_.end(), actor), registered_actors_.end());
    std::erase(registered_actors_, actor);
    pending_responses_.erase(actor.get());
  }

  void receive(const std::any &message) override {
    if (const TickResponse *tick_response = std::any_cast<TickResponse>(&message)) {
      handle_tick_response(*tick_response);
    } else if (std::any_cast<StopMessage>(&message)) {
      stop();
    }
  }

  void start_ticking() {
    last_tick_time_ = std::chrono::steady_clock::now();
    accumulated_time_ = std::chrono::microseconds(0);
    tick();
  }

private:
  void handle_tick_response(const TickResponse &resp) {
    {
      std::lock_guard<std::mutex> lock(registered_actors_mutex_);
      auto it = pending_responses_.find(resp.sender.get());
      if (it != pending_responses_.end()) {
        it->second = false;
      }
    }

    if (std::all_of(pending_responses_.begin(), pending_responses_.end(),
                    [](const auto &pair) { return !pair.second; })) {
      tick();
    }
  }

  void tick() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(now - last_tick_time_);
    last_tick_time_ = now;
    // Update accumulated_time_ with elapsed_time
    accumulated_time_ += elapsed_time;

    std::lock_guard<std::mutex> lock(registered_actors_mutex_);
    for (const auto &actor: registered_actors_) {
      send(actor, TickMessage{elapsed_time, accumulated_time_, shared_from_this()});
      pending_responses_[actor.get()] = true;
    }
  }

  std::vector<std::shared_ptr<Actor>> registered_actors_;
  std::map<Actor *, bool> pending_responses_;
  std::mutex registered_actors_mutex_;
  std::chrono::steady_clock::time_point last_tick_time_;
  std::chrono::microseconds accumulated_time_;
};

#endif //AWESOMEPROJECTCPP_TICK_ACTOR_H
