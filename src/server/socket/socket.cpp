#include "socket.hpp"
#include <cstdlib>
#include <sys/socket.h>

Socket::~Socket() { this->CloseSocket(); }

void Socket::OpenSocket(bool is_udp) {
  if (is_udp) {
    if ((this->sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      cerr << "Erro ao criar o servidor UDP" << endl;
      exit(EXIT_FAILURE);
    }
  } else {
    if ((this->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      cerr << "Erro ao criar o servidor TCP" << endl;
      exit(EXIT_FAILURE);
    }
  }
}

void Socket::CloseSocket() {
  if (this->sockfd >= 0) {
    close(this->sockfd);
    this->sockfd = -1;
  }
}

int Socket::SendMessage(const char *buffer) {
  int n = -1;
  if (this->is_udp) {
    n = sendto(this->sockfd, buffer, strlen(buffer), 0,
               (struct sockaddr *)&this->addr, sizeof(this->addr));

  } else {
    n = send(this->sockfd, buffer, strlen(buffer), 0);
  }

  if (n < 0) {
    std::cerr << "Erro ao enviar mensagem" << std::endl;
    return -1;
  }
  return n;
}

// Para receber mensagens via UDP
int Socket::RecvMessage(char *buffer) {
  if (this->is_udp) {
    socklen_t addr_len = sizeof(this->addr);
    return recvfrom(this->sockfd, buffer, BUFFER_SIZE, 0,
                    (struct sockaddr *)&this->addr, &addr_len);
  } else {
    return read(this->sockfd, buffer, BUFFER_SIZE);
  }
}

int Socket::GetSocketFd() { return this->sockfd; };
