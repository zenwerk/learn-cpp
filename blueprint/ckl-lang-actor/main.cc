#include <iostream>
#include <sstream>
#include <string>
#include <vector>

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
  auto actor1 = actor_system.spawn([](ActorSystem &as, std::string name) { return std::make_shared<MyActor>(as, name); }, "actor1");
  auto actor2 = actor_system.spawn<MyActor>("actor2");
  auto ticker = actor_system.spawn<TickerActor>("ticker");
  auto executor = actor_system.spawn<ScheduledTaskActor>("executor");

  actor1->send(actor2, PrintMessage{"Hello from actor 1!"});
  actor2->send(actor1, PrintMessage{"Hello from actor 2!"});

  ticker->add_actor(actor1);
  ticker->add_actor(actor2);
  ticker->add_actor(executor);

  executor->enqueue_task(std::chrono::microseconds{20000}, [] { std::cout << "TASK at 20,000μs" << std::endl; });

  ticker->start_ticking();

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  auto scheduled_time = ticker->get_accumulated_time().count() + (2 * 1000000);
  executor->enqueue_task(std::chrono::microseconds{scheduled_time}, [] { std::cout << "TASK at 2,000,000μs" << std::endl; });

  ticker->remove_actor(actor1);
  std::this_thread::sleep_for(std::chrono::seconds(3));

  actor1->stop();
  actor2->send(actor2, StopMessage{});
  ticker->stop();
  executor->stop();

  actor_system.remove_actor(actor1->get_name());
  actor_system.remove_actor(actor2->get_name());
  actor_system.remove_actor(ticker->get_name());
  actor_system.remove_actor(executor->get_name());

  std::string input;
  while (true) {
    std::cout << "$> ";
    std::getline(std::cin, input);

    if (input == "quit") {
      // TODO: 残っているアクターを全て終了させる
      break;
    }

    std::istringstream iss(input);
    std::vector<std::string> tokens;
    std::string token;
    while (std::getline(iss, token, ' ')) {
      tokens.push_back(token);
    }

    if (!tokens.empty()) {
      const auto &command = tokens[0];
      if (command == "create" && tokens.size() == 3) {
        std::string actor_name = tokens[1];
        std::string actor_type = tokens[2];
        // TODO: アクターの種類を文字列で指定
        if (actor_type == "MyActor") {
          auto actor = actor_system.spawn<MyActor>(actor_name);
          if (actor) {
            std::cout << "Created MyActor with name: " << actor_name << std::endl;
          } else {
            std::cout << "Failed to create EchoActor with name: " << actor_name << std::endl;
          }
        } else {
          std::cout << "Unknown actor type: " << actor_type << std::endl;
        }
      } else if (command == "send" && tokens.size() == 3) {
        std::string actor_name = tokens[1];
        std::string message = tokens[2];
        // TODO: アクターの種類によって送信するメッセージを分ける
        auto actor = actor_system.get_actor(actor_name);
        if (actor) {
          actor->send(actor, PrintMessage{message});
        } else {
          std::cout << "Actor not found: " << actor_name << std::endl;
        }
      } else if (command == "stop" && tokens.size() == 2) {
        std::string actor_name = tokens[1];
        auto actor = actor_system.get_actor(actor_name);
        if (actor) {
          actor->stop();
          std::cout << "Stopped actor: " << actor_name << std::endl;
        } else {
          std::cout << "Actor not found: " << actor_name << std::endl;
        }
      } else if (command == "remove" && tokens.size() == 2) {
        std::string actor_name = tokens[1];
        actor_system.remove_actor(actor_name);
        std::cout << "Removing actor: " << actor_name << std::endl;
      } else {
        std::cout << "Invalid command or wrong number of arguments.\n";
      }
    }
  }

  return 0;
}