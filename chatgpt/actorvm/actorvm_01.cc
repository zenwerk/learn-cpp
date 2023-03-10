#include <iostream>
#include <thread>
#include <vector>
#include <map>
#include <mutex>
#include <condition_variable>
#include <queue>

// NOTE: ChatGPT が示したところどころ不完全なコードであることに注意

class Actor {
public:
  virtual void receive(const std::string &message) = 0;
};

class Message {
public:
  //Actor *sender;
  std::string message;
};

class Mailbox {
public:
  void enqueue(Message message) {
    std::lock_guard<std::mutex> lock(mutex_);
    messages_.push(std::move(message));
    cv_.notify_one();
  }

  Message dequeue() {
    std::unique_lock<std::mutex> lock(mutex_);
    while (messages_.empty()) {
      cv_.wait(lock);
    }

    Message message = std::move(messages_.front());
    messages_.pop();
    return message;
  }

private:
  std::queue<Message> messages_;
  std::mutex mutex_;
  std::condition_variable cv_;
};

class ActorSystem {
public:
  // TODO: コンストラクタで起動したスレッドを join していないのでメモリーリークが起きる
  ActorSystem() {
    for (int i = 0; i < 4; ++i) {
      threads_.push_back(std::thread(&ActorSystem::worker_thread, this));
    }
  }

  void send(Actor *receiver, Message message) {
    mailbox_map_[receiver].enqueue(std::move(message));
  }

private:
  std::vector<std::thread> threads_;
  std::map<Actor *, Mailbox> mailbox_map_;

  void worker_thread() {
    // TODO: std::atomic で while(true) を止める処理が必要
    while (true) {
      for (auto &[actor, mailbox]: mailbox_map_) {
        Message message = mailbox.dequeue();
        actor->receive(message.message);
      }
    }
  }

};

class MyActor : public Actor {
public:
  void receive(const std::string &message) override {
    std::cout << "Received message: " << message << std::endl;
  }
};

int main() {
  ActorSystem actor_system;
  MyActor actor1, actor2;

  Message message1 = {/*&actor1,*/ "Hello from actor 1!"};
  Message message2 = {/*&actor2,*/"Hello from actor 2!"};

  actor_system.send(&actor1, message2);
  actor_system.send(&actor2, message1);

  // 実行結果を見るため sleep する
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  return 0;
}