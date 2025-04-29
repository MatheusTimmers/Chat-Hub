#include "chat/ChatHub.hpp"
#include <cstring>
#include <iostream>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif

int main(int argc, char *argv[]) {
  if (argc < 5) {
    std::cerr << "Para o modo Server: [server] [ip] [port] [TCP|UDP]"
              << std::endl;
    std::cerr << "Para o modo Client: [username] [ip] [port] [TCP|UDP]"
              << std::endl;
    return 1;
  }

#ifdef _WIN32
  WSADATA wsaData;
  int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (result != 0) {
    std::cerr << "Erro ao iniciar o Winsock: " << result << std::endl;
    return 1;
  }
#endif

  // Pega as informacoes de ip e porta
  std::string Ip = argv[2];
  int port = std::stoi(argv[3]);
  bool use_udp = (strcmp(argv[4], "udp") == 0) or (strcmp(argv[4], "UDP") == 0);

  if ((strcmp(argv[1], "server") == 0)) {
    std::cout << "Iniciando o programa no modo server." << std::endl;
    ChatHub *chat = new ChatHub(Ip, port, use_udp);

    if (!chat->StartHub()) {
      delete chat;
      cerr << "error ao criar o ChatHub" << std::endl;
      return 0;
    };

    while (1) {
      std::cout << "Servidor pronto e aguardando conexões..." << std::endl;
      chat->AcceptConnections();
    }
  } else {
    std::cout << "Iniciando o programa no modo cliente." << std::endl;
    std::cout << "Nome de Usuario: " << argv[1] << std::endl;
    ChatUser *user = new ChatUser(argv[1], Ip, port, use_udp);

    user->ConnectServer();
    while (1) {
      user->UserInteract();
    }
  }

#ifdef _WIN32
  WSACleanup();
#endif

  return 0;
}
