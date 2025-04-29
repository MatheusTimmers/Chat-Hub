#ifndef _SERVER_SOCKET_HPP
#define _SERVER_SOCKET_HPP

#include "../socket/socket.hpp"

class ServerSocket : public Socket {
public:
  ServerSocket(string ip, int port, bool is_udp);
  ~ServerSocket();

  int AcceptClient();
  bool ListenSocket(int backlog);
  bool BindSocket();

  void RecvMessage() override;
};

#endif
