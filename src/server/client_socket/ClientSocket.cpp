#include "ClientSocket.hpp"

ClientSocket::ClientSocket(string Ip, int port) {
  this->addr.sin_family = AF_INET;
  this->addr.sin_port = htons(port);
  this->addr.sin_addr.s_addr = inet_addr(Ip.c_str());

  this->open_socket();
  this->set_timeout(2);
}

ClientSocket::ClientSocket(int client_fd, struct sockaddr_in client_addr) {
  this->sockfd = client_fd;
  this->addr = client_addr;
}

ClientSocket::~ClientSocket() { close(this->sockfd); }

bool ClientSocket::connect_server() {
  if (connect(this->sockfd, (struct sockaddr *)&this->addr,
              sizeof(this->addr)) < 0) {
    std::cerr << "Erro ao conectar ao servidor." << std::endl;
    return false;
  }

  return true;
}

void ClientSocket::set_timeout(int timeout) {
  struct timeval tv;
  tv.tv_sec = timeout;
  tv.tv_usec = 0;

  // Define timeout para operações de recv
  if (setsockopt(this->sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv,
                 sizeof(tv)) < 0) {
    std::cerr << "Erro ao definir timeout no socket." << std::endl;
  }
}

int ClientSocket::send_message(const char *buffer) {
  int n = send(this->sockfd, buffer, strlen(buffer), 0);
  if (n < 0) {
    std::cerr << "Erro ao enviar mensagem" << std::endl;
    return -1;
  }
  return n;
}

int ClientSocket::recv_message(char *buffer) {
  int n = read(this->sockfd, buffer, BUFFER_SIZE - 1);
  buffer[n] = '\0';

  return n;
}

void ClientSocket::set_socket_fd(int fd) { this->sockfd = fd; }
