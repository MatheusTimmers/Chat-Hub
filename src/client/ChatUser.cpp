#include "ChatUser.hpp"

ChatUser::ChatUser(std::string username, std::string Ip, int port) {
  this->username = username;
  this->socket = new ClientSocket(Ip, port);
};

ChatUser::ChatUser(std::string username, int client_fd,
                   struct sockaddr_in client_addr) {
  this->username = username;
  this->socket = new ClientSocket(client_fd, client_addr);
};

ChatUser::~ChatUser() { delete socket; };

// Rodado dentro de uma thread
void ChatUser::print_messages() {
  char buffer[BUFFER_SIZE];
  while (true) {
    int bytes_received = this->socket->recv_message(buffer);
    // Se houver dados recebidos, printa a mensagem e continua a ler
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

      std::cout << username_dst << ": " << msg_content << std::endl;
    } else if (bytes_received == 0) {
      std::cout << "Conexão fechada pelo servidor." << std::endl;
      break;
    } else {
      std::cerr << "Erro ao receber mensagem." << std::endl;
      break;
    }
  }
}

void ChatUser::user_interact() {
  std::string input;

  while (true) {
    std::cout << "Digite uma ação: chat [username] | group [group_name] "
              << std::endl;
    std::getline(std::cin, input);

    // Comando para iniciar uma conversa
    // chat username
    // Comando para entrar em um grupo
    // group namegroup
    if (input.substr(0, 4) == "chat") {
      std::string target_user = input.substr(5);

      if (!target_user.empty()) {
        std::cout << "Solicitando chat com " << target_user << std::endl;
        this->send_command("chat", target_user);
      }
    } else if (input.substr(0, 5) == "group") {
      std::string group_name = input.substr(6);

      if (!group_name.empty()) {
        std::cout << "Entrando no grupo " << group_name << std::endl;
        this->send_command("group", group_name);
      }
    } else {
      // Usuario não digitou comando, ou já esta em um chat
      this->send_message(input.c_str(), "");
    }
  }
}

int ChatUser::recv_message(char *buffer) {
  return this->socket->recv_message(buffer);
}

void ChatUser::send_message(std::string message, std::string username_dst) {
  std::string final_message =
      this->username + " " + username_dst + " " + message;
  this->socket->send_message(final_message.c_str());
}

void ChatUser::send_command(std::string cmd, std::string param) {
  std::string result = cmd + " " + param;
  this->socket->send_message(result.c_str());
}

void ChatUser::send_login() {
  std::string login_msg = "login " + this->username;
  this->socket->send_message(login_msg.c_str());
}

void ChatUser::connect_server() {
  this->socket->connect_server();
  this->send_login();
}

std::string ChatUser::get_username() { return this->username; }

void ChatUser::set_username(std::string username) { this->username = username; }

int ChatUser::get_socket_fd() { return this->socket->get_socket_fd(); };
