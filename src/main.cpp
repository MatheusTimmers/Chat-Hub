#include "chat/ChatHub.hpp"
#include <cstring>
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc < 3) {
    std::cerr << "Para o modo Server: [server] [ip] [port]" << std::endl;
    std::cerr << "Para o modo Client: [username] [ip] [port]" << std::endl;
    return 1;
  }

  // Pega as informações do server
  std::string Ip = argv[2];
  int port = std::stoi(argv[3]);

  if ((strcmp(argv[1], "server") == 0)) {
    std::cout << "Iniciando o programa no modo server." << std::endl;
    ChatHub *chat = new ChatHub(Ip, port);

    if (!chat->start_hub()) {
      delete chat;
      cerr << "error ao criar o ChatHub" << std::endl;
      return 0;
    };

    while (1) {
      std::cout << "Servidor pronto e aguardando conexões..." << std::endl;
      chat->accept_connections();
    }
  } else {
    std::cout << "Iniciando o programa no modo cliente." << std::endl;
    std::cout << "Nome de Usuario: " << argv[1] << std::endl;
    ChatUser *user = new ChatUser(argv[1], Ip, port);

    user->connect_server();
    while (1) {
      user->user_interact();
    }
  }

  return 0;
}
