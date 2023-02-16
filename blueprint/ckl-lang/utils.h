#ifndef AWESOMEPROJECTCPP_UTILS_H
#define AWESOMEPROJECTCPP_UTILS_H

#include <mutex>
#include <iostream>
#include <sstream>

std::mutex pmtx;

void print(const std::string &s) {
  std::lock_guard<std::mutex> lk{pmtx};
  std::cout << s << std::endl;
}

void tokenize(std::string const &str, const char delim, std::vector<std::string> &out) {
  // construct a stream from the string
  std::stringstream ss(str);

  std::string s;
  while (std::getline(ss, s, delim)) {
    out.push_back(s);
  }
}

#endif //AWESOMEPROJECTCPP_UTILS_H
