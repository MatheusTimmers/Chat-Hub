#include "ChatUser.hpp"

ChatUser::ChatUser(std::string username, std::string Ip, int port, bool is_udp) {
  this->username = username;
  this->socket = new ClientSocket(Ip, port, is_udp);
  this->stop_msg_thread = false;
};

ChatUser::ChatUser(std::string username, int client_fd,
                   struct sockaddr_in client_addr) {
  this->username = username;
  this->socket = new ClientSocket(client_fd, client_addr);
  this->stop_msg_thread = false;
};

ChatUser::~ChatUser() { delete socket; };

// Rodado dentro de uma thread
void ChatUser::PrintMessages() {
  char buffer[BUFFER_SIZE];
  while (!this->stop_msg_thread) {
    int bytes_received = this->socket->RecvMessage(buffer);
    if (bytes_received > 0) {
      std::istringstream iss(buffer);
      std::string _, username_dst, msg_content;

      if (!(iss >> _ >> username_dst)) {
        std::cerr << "Erro no formato da mensagem." << std::endl;
        return;
      }

      std::getline(iss, msg_content);
      if (!msg_content.empty() && msg_content[0] == ' ') {
        msg_content.erase(0, 1);
      }

      if ((username_dst != "server")) {
        // Mensagem do Server
        if ((msg_content == "login_ok")) {
          cout << "Iniciando chat" << endl;
        } else {
          cout << "Erro: " << msg_content;

          // Não vai ter retentativa,
          this->stop_msg_thread = true;
        }
      } else {
        // Mensagem do usuario
        std::cout << username_dst << ": " << msg_content << std::endl;
      }
    } else if (bytes_received == 0) {
      std::cout << "Conexão fechada pelo servidor." << std::endl;
      break;
    }
  }
}

void ChatUser::UserInteract() {
  std::string input;
  UserCommands cmd = UserCommands::notfound;
  std::string target_name;

  // Comando para iniciar uma conversa
  // chat username
  // Comando para entrar em um grupo
  // group namegroup
  // while o usuario nao dar um comando valido
  while (cmd == UserCommands::notfound) {
    std::cout << "Digite uma ação: chat [username] | group [group_name] "
              << std::endl;
    std::getline(std::cin, input);
    cmd = UserCommands::notfound;

    if (input.substr(0, 4) == "chat") {
      target_name = input.substr(5);

      if (this->UserCanChat(target_name)) {
        cmd = UserCommands::chat;
        std::cout << "Solicitando chat com " << target_name << std::endl;
        this->current_chat = target_name;

        this->SendCommand("chat", target_name);
      }
    } else if (input.substr(0, 5) == "group") {
      target_name = input.substr(6);

      if (this->UserCanChat(target_name)) {
        cmd = UserCommands::group;
        std::cout << "Entrando no grupo " << target_name << std::endl;
        this->current_chat = target_name;

        this->SendCommand("group", target_name);
      }
    } else if (input.substr(0, 4) == "exit") {
      cmd = UserCommands::exitc;
      std::cout << "Saindo da conversa" << std::endl;

      // Verifica se existe chat para sair
      if (this->current_chat != "") {
        this->current_chat = "";

        this->SendCommand("exit", target_name);
      }
    }
  }

  this->WaitingConnection();

  if ((cmd != UserCommands::exitc) or (cmd != UserCommands::notfound)) {
    cout << "Esperando mensagens" << endl;
    this->stop_msg_thread = false;
    std::thread receiver_thread(&ChatUser::PrintMessages, this);

    while (1) {
      std::string msg;
      std::getline(std::cin, msg);
      if (msg == "exit") {
        this->SendCommand("exit", target_name);
        this->stop_msg_thread = true;
        break;
      } else {
        this->SendMessage(msg.c_str(), target_name);
      }
    }
    if (receiver_thread.joinable())
      receiver_thread.join();
  }
}

int ChatUser::RecvMessage(char *buffer) {
  return this->socket->RecvMessage(buffer);
}

void ChatUser::WaitingConnection() {
  bool confirmation_received = false;
  int bytes_received;

  char buffer[BUFFER_SIZE];
  std::string server_message, msg_content;

  while (!confirmation_received) {
    bytes_received = this->RecvMessage(buffer);
    
    if (bytes_received > 0) {
      std::istringstream iss(buffer);
      iss >> server_message;

      if (server_message == "server") {
        iss >> msg_content;

        if (msg_content == "OK") {
          confirmation_received = true;
        } else if (msg_content == "erro") {
          std::cerr << "Servidor: Ocorreu um erro, tente novamente." << std::endl;
          return;
        }
      }
    }
  }
}

void ChatUser::SendMessage(std::string message, std::string username_dst) {
  std::string final_message =
      this->username + " " + username_dst + " " + message;
  this->socket->SendMessage(final_message.c_str());
}

void ChatUser::SendCommand(std::string cmd, std::string param) {
  std::string result = cmd + " " + param;
  this->socket->SendMessage(result.c_str());
}

void ChatUser::SendLogin() {
  std::string login_msg = "login " + this->username;
  this->socket->SendMessage(login_msg.c_str());
}

void ChatUser::ConnectServer() {
  this->socket->ConnectServer();
  this->SendLogin();
}

std::string ChatUser::GetUsername() { return this->username; }

// Verifica se o nome do usuario é diferente de vazio e se já não esta em outro
// chat
bool ChatUser::UserCanChat(const string target_name) {
  return !target_name.empty() and this->current_chat.empty();
}

void ChatUser::SetUsername(std::string username) { this->username = username; }

int ChatUser::GetSocketFd() { return this->socket->GetSocketFd(); };
