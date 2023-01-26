#include <string>
#include <iostream>
#include <queue>
#include <utility>


class Task {
  std::string name;

public:
  Task(std::string name, unsigned long long wait) : name(std::move(name)), wait(wait) {}
  unsigned long long wait; // time to wait before the callback

  void run() {
    std::cout << "Task: " + name + " done!" << std::endl;
  }

  void foo() const {
    std::cout << "my name is " << name << "." << std::endl;
  }
};

auto compare = [](std::shared_ptr<Task> a, std::shared_ptr<Task> b) { return a->wait > b->wait; };

class Q {
  std::string name;
  std::priority_queue<
    std::shared_ptr<Task>,
    std::vector<std::shared_ptr<Task>>,
    decltype(compare)
  > que{compare};

public:
  explicit Q(std::string name) : name(std::move(name)) {}

  void push(std::shared_ptr<Task> &task) {
    que.push(task);
    task->foo();
    std::cout << name + " queue size:" + std::to_string(que.size()) << std::endl;
  }

  std::shared_ptr<Task> top() {
    if (que.empty()) {
      std::cout << "que is empty" << std::endl;
      return nullptr;
    }
    auto t = que.top();
    que.pop();
    return t;
  }

};


int main() {
  Q q{"Q"};

  auto t1 = std::make_shared<Task>(Task{"task1", 1000000});
  auto t2 = std::make_shared<Task>(Task{"task2", 2000000});
  auto t3 = std::make_shared<Task>(Task{"task3", 3000000});
  auto t4 = std::make_shared<Task>(Task{"task4", 4000000});

  q.push(t2);
  q.push(t1);
  q.push(t4);
  q.push(t3);

  auto a = q.top();
  a->foo();
  a->run();

  auto b = q.top();
  b->foo();
  b->run();

  auto c = q.top();
  c->foo();
  c->run();

  auto d = q.top();
  d->foo();
  d->run();

//  std::cout << que.empty() << "|" << que.size() << std::endl;
//
//  // size() == 0 でも top を呼ぶと無限に pop できる？
  auto e = q.top();
  if (e == nullptr)
    std::cout << "ぬるぽ" << std::endl;
//  que.pop();
//
//  auto f = que.top();
//  f.run();
//  que.pop();

  return 0;
}