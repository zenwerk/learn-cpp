#ifndef AWESOMEPROJECTCPP_SCHEDULED_TASK_ACTOR_H
#define AWESOMEPROJECTCPP_SCHEDULED_TASK_ACTOR_H


#include <queue>
#include <functional>
#include <chrono>
#include <mutex>
#include <any>


struct ScheduledTask {
  std::chrono::microseconds scheduled_time;
  std::function<void()> task;

  bool operator<(const ScheduledTask &other) const {
    return scheduled_time > other.scheduled_time;
  }
};

class ScheduledTaskActor : public Actor {
public:
  using Actor::Actor;

  void enqueue_task(std::chrono::microseconds scheduled_time, const std::function<void()> &task) {
    std::lock_guard<std::mutex> lock(scheduled_tasks_mutex_);
    scheduled_tasks_.emplace(ScheduledTask{scheduled_time, task});
  }

  void receive(const std::any &message) override {
    if (const TickMessage *tick_msg = std::any_cast<TickMessage>(&message)) {
      {
        std::lock_guard<std::mutex> lock(scheduled_tasks_mutex_);
        process_scheduled_tasks(tick_msg->accumulated_time);
      }
      send(tick_msg->sender, TickResponse{shared_from_this()});
    } else if (std::any_cast<StopMessage>(&message)) {
      stop();
    }
  }

private:
  void process_scheduled_tasks(const std::chrono::microseconds &accumulated_time) {
    while (!scheduled_tasks_.empty() && scheduled_tasks_.top().scheduled_time <= accumulated_time) {
      scheduled_tasks_.top().task();
      scheduled_tasks_.pop();
    }
  }

  std::priority_queue<ScheduledTask> scheduled_tasks_;
  std::mutex scheduled_tasks_mutex_;
};


#endif //AWESOMEPROJECTCPP_SCHEDULED_TASK_ACTOR_H
