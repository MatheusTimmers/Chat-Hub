#ifndef _CLIENT_SOCKET_HPP
#define _CLIENT_SOCKET_HPP

#include "../socket/socket.hpp"

class ClientSocket : public Socket {
public:
  // Constructor usado pelo cliente
  ClientSocket(string Ip, int port, bool is_udp);
  // Constructor usado pelo server
  ClientSocket(int client_fd, struct sockaddr_in client_addr);
  ~ClientSocket();

  // TCP
  bool ConnectServer();
  int SendMessage(const char *buffer);
  int RecvMessage(char *buffer);

  void SetTimeout(int timeout);
  void SetSocketFd(int fd);
};

#endif
