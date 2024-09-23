#include "chat/ChatHub.hpp"
#include <cstring>
#include <iostream>
#include <thread>

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

    user->user_interact();

    // verifica a interação do usuario
    std::thread receiver_thread(&ChatUser::print_messages, user);

    while (1) {
      // TODO: Criar comandos para entrar em chats com pessoas especificas
      std::string msg;
      std::getline(std::cin, msg);
      user->send_message(msg.c_str(), "teste");
    }
  }

  return 0;
}
