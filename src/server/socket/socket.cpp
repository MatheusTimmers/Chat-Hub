#include "socket.hpp"
#include <cstdlib>
#include <sys/socket.h>

Socket::~Socket() { this->close_socket(); }

void Socket::open_socket() {
  if ((this->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    cerr << "Erro ao criar o servidor" << endl;
    exit(EXIT_FAILURE);
  }
}

void Socket::close_socket() {
  if (this->sockfd >= 0) {
    close(this->sockfd);
    this->sockfd = -1;
  }
}

int Socket::get_socket_fd() { return this->sockfd; };