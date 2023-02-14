#ifndef TASK_H
#define TASK_H

#include <string>
#include <utility>
#include "utils.h"


class Task {
  std::string name;

public:
  Task(std::string name, unsigned long long wait) : name(std::move(name)), wait(wait) {}
  unsigned long long wait; // time to wait before the callback

  void run() {
    print("Task: " + name + " done!");
  }
};



#endif //TASK_H
