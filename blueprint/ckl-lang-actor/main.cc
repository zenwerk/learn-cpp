#include "actor_system.h"
#include "messages.h"

class MyActor : public Actor {
public:
  using Actor::Actor;

  void receive(const Message &message) override {
    std::visit([this](const auto &msg) {
      using T = std::decay_t<decltype(msg)>;
      if constexpr (std::is_same_v<T, PrintMessage>) {
        std::cout << "Received message: " << msg.text << std::endl;
      } else if constexpr (std::is_same_v<T, StopMessage>) {
        stop();
      }
    }, message);
  }
};

int main() {
  ActorSystem actor_system;
  auto actor1 = actor_system.spawn([](ActorSystem &actor_system) { return std::make_shared<MyActor>(actor_system); });
  auto actor2 = actor_system.spawn<MyActor>();

  actor1->send(actor2, PrintMessage{"Hello from actor 1!"});
  actor2->send(actor1, PrintMessage{"Hello from actor 2!"});

  actor1->stop();
  actor2->send(actor2, StopMessage{});

  std::this_thread::sleep_for(std::chrono::seconds(1));

  return 0;
}