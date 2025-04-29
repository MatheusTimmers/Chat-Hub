#include <iostream>

enum UserCommands {
  group,
  chat,
  notfound,
  exitc,
};

struct MessageData {
  std::string name_src;
  std::string name_dest;
  std::string content;
};
