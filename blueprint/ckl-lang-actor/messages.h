#ifndef AWESOMEPROJECTCPP_MESSAGES_H
#define AWESOMEPROJECTCPP_MESSAGES_H

#include <string>
#include <variant>


struct PrintMessage {
  std::string text;
};

struct StopMessage {};

struct TickMessage {
  std::chrono::milliseconds elapsed_time;
};

struct TickResponse {};

using Message = std::variant<
  TickMessage,
  TickResponse,
  PrintMessage,
  StopMessage
>;

#endif //AWESOMEPROJECTCPP_MESSAGES_H
