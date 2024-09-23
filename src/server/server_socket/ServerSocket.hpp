#ifndef _SERVER_SOCKET_HPP
#define _SERVER_SOCKET_HPP

#include "../socket/socket.hpp"

class ServerSocket : public Socket
{
public:
  ServerSocket(string ip, int port);
  ~ServerSocket();

  int accept_client();
  bool listen_socket(int backlog);
  bool bind_socket();
};

#endif