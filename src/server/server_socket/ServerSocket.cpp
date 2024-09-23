#include "ServerSocket.hpp"

ServerSocket::ServerSocket(string ip, int port) {
  this->addr.sin_family = AF_INET;
  this->addr.sin_port = htons(port);
  this->addr.sin_addr.s_addr = inet_addr(ip.c_str());

  this->open_socket();
}

ServerSocket::~ServerSocket() {
  close(this->sockfd);
}

bool ServerSocket::listen_socket(int backlog) {
  if (listen(this->sockfd, backlog) < 0) {
    std::cerr << "Erro ao escutar no servidor." << std::endl;
    return false;
  }
  return true;
}

bool ServerSocket::bind_socket() {
  if (bind(this->sockfd, (struct sockaddr *)&this->addr,
           sizeof(sockaddr_in)) < 0) {
    std::cerr << "Erro ao fazer bind no servidor." << std::endl;
    return false;
  }
  return true;
}

int ServerSocket::accept_client() {
  struct sockaddr_in client_addr;
  socklen_t addr_len = sizeof(client_addr);
  
  int sockfd = accept(this->sockfd, (struct sockaddr*)&this->addr, &addr_len);

  if (sockfd < 0) {
    std::cerr << "Erro ao aceitar conexão." << std::endl;
    return false;
  }

  return sockfd;
}