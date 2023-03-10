#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <chrono>

class ActorSystem;

class Actor {
public:
  explicit Actor(ActorSystem *system) : m_system(system) {}

  virtual void receive(const std::string &msg) = 0;

  void send(Actor *actor, const std::string &msg);

private:
  ActorSystem *m_system;
};

class ActorSystem {
public:
  explicit ActorSystem(int numCores) {
    for (int i = 0; i < numCores; i++) {
      workers.emplace_back([this]() {
        // TODO: これだと cv.wait しているスレッドは quit = true しても終了できない
        // 終了メッセージをworkerスレッドに対して投げる必要がある。
        while (!quit) {
          std::unique_lock<std::mutex> lock(queueMutex);

          // Wait until there is a message to process
          cv.wait(lock, [this] { return !queue.empty(); });

          // Get the next message from the queue
          auto msg = queue.front();
          queue.pop();

          lock.unlock();

          // Process the message
          msg.to->receive(msg.msg);
        }
      });
    }
  }

  ~ActorSystem() {
    quit.store(true);
    std::cout << "ここまできた2" << std::endl;
    for (auto &worker: workers) {
      worker.join();
    }
  }

  void send(Actor *from, Actor *to, const std::string &msg) {
    std::lock_guard<std::mutex> lock(queueMutex);
    queue.push({to, msg});
    cv.notify_one();
  }

private:
  struct Message {
    Actor *to;
    std::string msg;
  };

  std::atomic<bool> quit = false;
  std::vector<std::thread> workers;
  std::mutex queueMutex;
  std::condition_variable cv;
  std::queue<Message> queue;
};

void Actor::send(Actor *actor, const std::string &msg) {
  m_system->send(this, actor, msg);
}

class EchoActor : public Actor {
public:
  explicit EchoActor(ActorSystem *system) : Actor(system) {}

  void receive(const std::string &msg) override {
    std::cout << "EchoActor received: " << msg << std::endl;
//    send(/*this, */this, msg);  // Echo the message back to myself
  }
};

int main() {
  ActorSystem system(4);  // Use 4 worker threads
  EchoActor actor(&system);

  // Send messages to the actor and wait for a response
  for (int i = 0; i < 2; i++) {
    actor.send(&actor, "Hello " + std::to_string(i));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  std::cout << "ここまできた" << std::endl;

  return 0;
}
