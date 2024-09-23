#ifndef _CHAT_CPP_
#define _CHAT_CPP_

#include "../client/ChatUser.hpp"
#include "../server/server_socket/ServerSocket.hpp"
#include <map>
#include <mutex>
#include <sstream>
#include <thread>

enum UserCommands {
  group,
  chat,
  notfound,
  exitc,
};

class ChatHub {
private:
  std::map<int, ChatUser *> *connected_clients;
  std::mutex client_mutex;

  ChatUser *login_user(int sockfd);

public:
  ServerSocket *server;

  ChatHub(std::string, int port);
  ~ChatHub();

  bool start_hub();
  void handle_client(ChatUser *user);
  void process_message(ChatUser *user, const std::string &message);
  UserCommands process_command(ChatUser *user, const std::string &message);
  void accept_connections();

  ChatUser *get_user(const std::string &username);
};

#endif
