#include "ClientSocket.hpp"

ClientSocket::ClientSocket(string Ip, int port, bool is_udp) {
  this->addr.sin_family = AF_INET;
  this->addr.sin_port = htons(port);
  this->addr.sin_addr.s_addr = inet_addr(Ip.c_str());

  this->OpenSocket(is_udp);
  this->SetTimeout(2);
}

ClientSocket::ClientSocket(int client_fd, struct sockaddr_in client_addr) {
  this->sockfd = client_fd;
  this->addr = client_addr;
}

ClientSocket::~ClientSocket() { close(this->sockfd); }

bool ClientSocket::ConnectServer() {
  // Cliente UDP não se conecta
  if (!this->is_udp) {
    if (connect(this->sockfd, (struct sockaddr *)&this->addr,
                sizeof(this->addr)) < 0) {
      std::cerr << "Erro ao conectar ao servidor." << std::endl;
      return false;
    }
    return true;
  }
  return false;
}

void ClientSocket::SetTimeout(int timeout) {
  struct timeval tv;
  tv.tv_sec = timeout;
  tv.tv_usec = 0;

  // Define timeout para operações de recv
  if (setsockopt(this->sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv,
                 sizeof(tv)) < 0) {
    std::cerr << "Erro ao definir timeout no socket." << std::endl;
  }
}

void ClientSocket::SetSocketFd(int fd) { this->sockfd = fd; }
