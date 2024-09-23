#ifndef _SOCKET_CPP_
#define _SOCKET_CPP_

#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

using namespace std;

class Socket {
protected:
  int sockfd;

public:
  struct sockaddr_in addr;
  
  ~Socket();
  
  void open_socket();
  void close_socket();

  int get_socket_fd();
};

#endif
