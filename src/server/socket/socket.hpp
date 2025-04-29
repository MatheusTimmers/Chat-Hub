#ifndef _SOCKET_CPP_
#define _SOCKET_CPP_

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#define BUFFER_SIZE 1024

using namespace std;

class Socket {
protected:
  bool is_udp;
  int sockfd;

public:
  struct sockaddr_in addr;

  ~Socket();

  void OpenSocket(bool is_udp);
  void CloseSocket();

  // UDP
  virtual int RecvMessage(char *buffer);
  int SendMessage(const char *buffer);

  int GetSocketFd();
};

#endif
