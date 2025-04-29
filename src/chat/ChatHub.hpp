#ifndef _CHAT_CPP_
#define _CHAT_CPP_

#define MAX_CLIENTS 10

#include "../client/ChatUser.hpp"
#include "../server/server_socket/ServerSocket.hpp"

#include <map>
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>

class ChatHub {
private:
  std::map<int, ChatUser *> *connected_clients;
  std::map<std::string, std::vector<ChatUser *>> groups;
  std::mutex client_mutex, group_mutex;

  ChatUser *LoginUser(int sockfd);

public:
  ServerSocket *server;

  ChatHub(std::string, int port, bool is_udp);
  ~ChatHub();

  bool StartHub();

  // TCP
  void HandleClient(ChatUser *user);
  void AcceptConnections();

  // UDP
  void HandleClientUdp();

  MessageData ParserReceivedMessage(char *buffer);
  void ProcessMessage(ChatUser *user, char *buffer);
  UserCommands ProcessCommand(ChatUser *user, const std::string &message);

  void AddUserToGroup(ChatUser *user, const std::string &group_name);
  ChatUser *GetUser(const std::string &username);
};

#endif
