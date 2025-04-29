#include "ChatHub.hpp"
#include <cstring>
#include <iostream>
#include <iterator>
#include <locale>
#include <mutex>
#include <ostream>
#include <string>

ChatHub::ChatHub(std::string ip, int port, bool is_udp) {
  this->server = new ServerSocket(ip, port, is_udp);
  this->connected_clients = new std::map<int, ChatUser *>();
};

ChatHub::~ChatHub() {
  delete server;
  delete connected_clients;
};

bool ChatHub::StartHub() {
  if (not(this->server->BindSocket() and
          this->server->ListenSocket(MAX_CLIENTS))) {
    std::cerr << "Erro ao criar o HUB" << std::endl;
    return false;
  }
  return true;
};

ChatUser *ChatHub::LoginUser(int sockfd) {
  char buffer[BUFFER_SIZE];

  int recv_status = recv(sockfd, buffer, BUFFER_SIZE, 0);
  if (recv_status <= 0) {
    std::cerr << "Erro ao receber nome de usuário do cliente." << std::endl;
    return nullptr;
  }

  buffer[recv_status] = '\0';

  std::string login = buffer;
  if (login.substr(0, 5) != "login") {
    std::cerr << "Primeira mensagem não é de login" << std::endl;
    return nullptr;
  }

  return new ChatUser(login.substr(6), sockfd, this->server->addr);
};

void ChatHub::AcceptConnections(bool is_udp) {
  // Aceita conexões do cliente
  if (is_udp) {
    int client_sockfd = this->server->AcceptClient();

    if (client_sockfd > 0) {
      ChatUser *new_user = LoginUser(client_sockfd);
      std::cout << "Cliente conectado!" << std::endl;

      std::thread(&ChatHub::HandleClient, this, new_user).detach();
    }
  } else {
    char buffer[BUFFER_SIZE];
    this->server->RecvMessage(buffer);

    std::thread(&ChatHub::HandleClient, this).detach();
  }
}

// Roda em uma thread diferente para cada cliente
void ChatHub::HandleClient(ChatUser *user) {
  int client_sockfd = user->GetSocketFd();

  // TODO: Mover para AcceptConnections
  {
    std::unique_lock<std::mutex> lock(client_mutex);
    (*connected_clients)[client_sockfd] = user;
    std::cout << "Cliente adicionado ao HUB." << std::endl;
  }

  char buffer[BUFFER_SIZE];
  int bytes_received = user->RecvMessage(buffer);

  if (bytes_received > 0) {
    ProcessMessage(user, buffer);
  } else {
    std::cerr << "Cliente desconectado ou erro ao receber dados." << std::endl;
  }

  {
    std::unique_lock<std::mutex> lock(client_mutex);
    (*connected_clients).erase(client_sockfd);
    std::cout << "Cliente removido do HUB." << std::endl;
  }

  delete user;
}

void ChatHub::HandleClientUdp() {
  // Espera uma mensagem
  char buffer[BUFFER_SIZE];
  this->server->RecvMessage(buffer);

  MessageData msg = ParserReceivedMessage(buffer);
  ChatUser *user = GetUser(msg.name_src);
  if (!user) {
    user = LoginUser(int sockfd)
  }
}

void ChatHub::ProcessMessage(ChatUser *user, char *buffer) {
  while (1) {
    // Mensagem chat username - linka o usuario atual com
    // Mensagem group group_name - linka o usuario atual com um grupo
    // Mensagem UsernameSrc UsernameDest msg - depois linkado envia a mensagem
    std::string message(buffer);
    UserCommands cmd = ProcessCommand(user, message);

    // TODO: Criar mensagem para cliente para avisar da
    // conexão bem sucedidada
    switch (cmd) {
    case chat: {
      while (1) {
        user->RecvMessage(buffer);
        MessageData msg = this->ParserReceivedMessage(buffer);

        if (msg.content == "exit") {
          break;
        }

        ChatUser *dest_user = GetUser(msg.name_dest);
        if (!dest_user) {
          std::cerr << "Usuário " << msg.name_dest
                    << " não encontrado ou não está conectado." << std::endl;
          return;
          // TODO:Mensagem avisando do erro
        }
        dest_user->SendMessage(msg.content, msg.name_src);
      }

      break;
    }
    case group: {
      while (1) {
        user->RecvMessage(buffer);
        MessageData msg = this->ParserReceivedMessage(buffer);

        if (msg.content == "exit") {
          break;
        }

        if (groups.find(msg.name_dest) != groups.end()) {
          for (ChatUser *member : groups[msg.name_dest]) {
            if (member != user) {
              member->SendMessage(msg.content, user->GetUsername());
            }
          }
        }
      }
      break;
    }
    case exitc: {
      std::cout << "Saindo da aplicação..." << std::endl;
      // TODO: adicionar a lógica para quando o comando for "exit"
      break;
    }
    case notfound: {
      std::cerr << "Comando não encontrado!" << std::endl;
      // TODO: Adicionar a lógica para comandos não encontrados
      break;
    }
    default: {
      std::cerr << "Comando desconhecido!" << std::endl;
      break;
    }
    }
  }
}

UserCommands ChatHub::ProcessCommand(ChatUser *user,
                                     const std::string &message) {
  if (message.substr(0, 4) == "chat") {
    std::string tarGetUser = message.substr(5);
    ChatUser *dest_user = GetUser(tarGetUser);
    cout << "DEBUG2 " << message << endl;

    if (dest_user) {
      std::string welcome_msg = "Chat iniciado com " + tarGetUser;
      user->SendMessage(welcome_msg.c_str(), user->GetUsername());
      return UserCommands::chat;
    } else {
      std::string error_msg = "Usuário " + tarGetUser + " não encontrado.";
      user->SendMessage(error_msg.c_str(), user->GetUsername());
      return UserCommands::notfound;
    }
  } else if (message.substr(0, 5) == "group") {
    std::string group_name = message.substr(6);
    if (!group_name.empty()) {
      AddUserToGroup(user, group_name);
      return UserCommands::group;
    }
  } else if (message.substr(0, 4) == "exit") {
    return UserCommands::exitc;
  }
  return UserCommands::notfound;
}

MessageData ChatHub::ParserReceivedMessage(char *buffer) {
  MessageData message_data;

  if (strlen(buffer)) {
    // TODO: Criar mensagem para cliente para avisar da conexão bem sucedida
    std::string message(buffer);
    std::istringstream iss(message);

    std::cout << "DEBUG3: " << message << std::endl;

    if (!(iss >> message_data.name_src >> message_data.name_dest)) {
      std::cerr << "Erro no formato da mensagem." << std::endl;
      return message_data;
    }

    if (message == "exit " + message_data.name_dest) {
      message_data.content = "exit";
      return message_data;
    }

    std::getline(iss, message_data.content);

    if (!message_data.content.empty() && message_data.content[0] == ' ') {
      message_data.content.erase(0, 1);
    }
  } else {
    cout << "DEBUG4: buffer vazio" << endl;
  }

  return message_data;
}

ChatUser *ChatHub::GetUser(const std::string &username) {
  std::unique_lock<std::mutex> lock(client_mutex);

  for (const auto &pair : *connected_clients) {
    ChatUser *user = pair.second;
    if (user->GetUsername() == username) {
      return user;
    }
  }

  return nullptr;
}

void ChatHub::AddUserToGroup(ChatUser *user, const std::string &group_name) {
  std::unique_lock<std::mutex> lock(group_mutex);

  if (this->groups.find(group_name) == this->groups.end()) {
    groups[group_name] = std::vector<ChatUser *>();
  }

  groups[group_name].push_back(user);

  std::string welcome_msg = "Você entrou no grupo " + group_name;
  user->SendMessage(welcome_msg.c_str(), "server");

  for (ChatUser *member : groups[group_name]) {
    if (member != user) {
      member->SendMessage(user->GetUsername() + " entrou no grupo.", "server");
    }
  }
}
