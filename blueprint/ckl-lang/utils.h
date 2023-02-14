#ifndef AWESOMEPROJECTCPP_UTILS_H
#define AWESOMEPROJECTCPP_UTILS_H

#include <mutex>
#include <iostream>

std::mutex pmtx;

void print(const std::string &s) {
  std::lock_guard<std::mutex> lk{pmtx};
  std::cout << s << std::endl;
}

#endif //AWESOMEPROJECTCPP_UTILS_H
