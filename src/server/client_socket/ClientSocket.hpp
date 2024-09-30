#ifndef _CLIENT_SOCKET_HPP
#define _CLIENT_SOCKET_HPP

#include "../socket/socket.hpp"

class ClientSocket : public Socket {
public:
  // Constructor usado pelo cliente
  ClientSocket(string Ip, int port);
  // Constructor usado pelo server
  ClientSocket(int client_fd, struct sockaddr_in client_addr);
  ~ClientSocket();

  bool connect_server();
  int send_message(const char *buffer);
  int recv_message(char *buffer);

  void set_timeout(int timeout);
  void set_socket_fd(int fd);
};

#endif
