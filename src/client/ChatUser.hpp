#ifndef _CHAT_USER_HPP
#define _CHAT_USER_HPP

#include "../server/client_socket/ClientSocket.hpp"
#include <sstream>
#include <string>

class ChatUser {
private:
  ClientSocket *socket;
  std::string username;

public:
  // User criado pelo Cliente
  ChatUser(std::string username, std::string Ip, int port);

  // User criado pelo Server
  ChatUser(std::string username, int client_fd, struct sockaddr_in client_addr);
  ~ChatUser();

  void user_interact();

  // Envia o login para o server
  void send_login();

  // Printa as mesnagens que o User recebe
  void print_messages();

  // Recebe as mensagens do User
  int recv_message(char *buffer);

  // Envia as mensagens para o User
  void send_message(std::string message, std::string username_dst);

  // Envia um comando para o servidor
  void send_command(std::string cmd, std::string param);

  // Se conecta no server
  void connect_server();

  // Get e Set
  std::string get_username();
  int get_socket_fd();
  void set_username(std::string username);
};

#endif
