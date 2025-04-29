#include "ServerSocket.hpp"

ServerSocket::ServerSocket(string ip, int port, bool is_udp) {
  this->addr.sin_family = AF_INET;
  this->addr.sin_port = htons(port);
  this->addr.sin_addr.s_addr = inet_addr(ip.c_str());

  this->OpenSocket(is_udp);
}

ServerSocket::~ServerSocket() { close(this->sockfd); }

bool ServerSocket::ListenSocket(int backlog) {
  // Não deve ter listen para UDP
  if (!this->is_udp) {
    if (listen(this->sockfd, backlog) < 0) {
      std::cerr << "Erro ao escutar no servidor." << std::endl;
      return false;
    }
    return true;
  }
  return false;
}

bool ServerSocket::BindSocket() {
  if (bind(this->sockfd, (struct sockaddr *)&this->addr, sizeof(sockaddr_in)) <
      0) {
    std::cerr << "Erro ao fazer bind no servidor." << std::endl;
    return false;
  }
  return true;
}

int ServerSocket::AcceptClient() {
  struct sockaddr_in client_addr;
  socklen_t addr_len = sizeof(client_addr);

  int sockfd = accept(this->sockfd, (struct sockaddr *)&this->addr, &addr_len);

  if (sockfd < 0) {
    std::cerr << "Erro ao aceitar conexão." << std::endl;
    return false;
  }

  return sockfd;
}
