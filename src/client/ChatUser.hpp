#ifndef _CHAT_USER_HPP
#define _CHAT_USER_HPP

#include "../../enums/utils.hpp"
#include "../server/client_socket/ClientSocket.hpp"
#include <atomic>
#include <sstream>
#include <string>
#include <thread>

class ChatUser {
private:
  std::atomic<bool> stop_msg_thread;
  ClientSocket *socket;
  std::string username;
  std::string current_chat;

public:
  // User criado pelo Cliente
  ChatUser(std::string username, std::string Ip, int port, bool is_udp);

  // User criado pelo Server
  ChatUser(std::string username, int client_fd, struct sockaddr_in client_addr);
  ~ChatUser();

  void UserInteract();

  // Envia o login para o server
  void SendLogin();

  // Printa as mesnagens que o User recebe
  void PrintMessages();

  // Recebe as mensagens do User
  int RecvMessage(char *buffer);

  // Envia as mensagens para o User
  void SendMessage(std::string message, std::string username_dst);

  // Envia um comando para o servidor
  void SendCommand(std::string cmd, std::string param);

  // Se conecta no server
  void ConnectServer();

  // Varica se o user pode se conectar
  bool UserCanChat(std::string target_name);

  // Espera a resposta do servidor
  void WaitingConnection();

  // Get e Set
  std::string GetUsername();
  int GetSocketFd();
  void SetUsername(std::string username);
};

#endif
