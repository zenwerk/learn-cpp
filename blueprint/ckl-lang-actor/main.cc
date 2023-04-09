#include "actor_system.h"
#include "messages.h"
#include "tick_actor.h"
#include "scheduled_task_actor.h"

class MyActor : public Actor {
public:
  using Actor::Actor;

  void receive(const std::any &msg) override {
    if (const PrintMessage *print_msg = std::any_cast<PrintMessage>(&msg)) {
      std::cout << "Received message: " << print_msg->text << std::endl;
    } else if (const TickMessage *tick_msg = std::any_cast<TickMessage>(&msg)) {
      // std::cout << "Received Tick: duration = " << tick_msg->elapsed_time.count() << "μs" << " | accumulated_time " << tick_msg->accumulated_time.count() << "μs" << std::endl;
      send(tick_msg->sender, TickResponse{shared_from_this()});
    } else if (std::any_cast<StopMessage>(&msg)) {
      stop();
    }
  }
};


int main() {
  ActorSystem actor_system;
  auto actor1 = actor_system.spawn([](ActorSystem &actor_system) { return std::make_shared<MyActor>(actor_system); });
  auto actor2 = actor_system.spawn<MyActor>();
  auto ticker = actor_system.spawn<TickerActor>();
  auto executer = actor_system.spawn<ScheduledTaskActor>();

  actor1->send(actor2, PrintMessage{"Hello from actor 1!"});
  actor2->send(actor1, PrintMessage{"Hello from actor 2!"});

  ticker->add_actor(actor1);
  ticker->add_actor(actor2);
  ticker->add_actor(executer);

  executer->enqueue_task(std::chrono::microseconds{20000}, [] { std::cout << "TASK at 20,000μs" << std::endl; });

  ticker->start_ticking();

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  auto scheduled_time = ticker->get_accumulated_time().count() + (2 * 1000000);
  executer->enqueue_task(std::chrono::microseconds{scheduled_time}, [] { std::cout << "TASK at 2,000,000μs" << std::endl; });

  ticker->remove_actor(actor1);
  std::this_thread::sleep_for(std::chrono::seconds(3));

  actor1->stop();
  actor2->send(actor2, StopMessage{});
  ticker->stop();
  executer->stop();

  return 0;
}