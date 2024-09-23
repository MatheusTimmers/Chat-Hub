#include "ChatHub.hpp"
#include <iostream>
#include <mutex>
#include <ostream>
#include <string>

ChatHub::ChatHub(std::string ip, int port) {
  this->server = new ServerSocket(ip, port);
  this->connected_clients = new std::map<int, ChatUser *>();
};

ChatHub::~ChatHub() {
  delete server;
  delete connected_clients;
};

bool ChatHub::start_hub() {
  // TODO: deixar responsivo
  if (not(this->server->bind_socket() and this->server->listen_socket(10))) {
    std::cerr << "Erro ao criar o HUB" << std::endl;
    return false;
  }
  return true;
};

ChatUser *ChatHub::login_user(int sockfd) {
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

void ChatHub::accept_connections() {
  while (true) {
    // Aceita conexões do cliente
    int client_sockfd = this->server->accept_client();

    if (client_sockfd > 0) {
      ChatUser *new_user = login_user(client_sockfd);
      std::cout << "Cliente conectado!" << std::endl;

      std::thread(&ChatHub::handle_client, this, new_user).detach();
    } else {
      std::cerr << "Erro ao aceitar conexão do cliente" << std::endl;
    }
  }
}

// Roda em uma thread diferente para cada cliente
void ChatHub::handle_client(ChatUser *user) {
  int client_sockfd = user->get_socket_fd();

  // TODO: Mover para accept_connections
  {
    std::unique_lock<std::mutex> lock(client_mutex);
    (*connected_clients)[client_sockfd] = user;
    std::cout << "Cliente adicionado ao HUB." << std::endl;
  }

  char buffer[BUFFER_SIZE];
  while (true) {
    // Recebe mensagem do cliente
    int bytes_received = user->recv_message(buffer);
    if (bytes_received <= 0) {
      std::cerr << "Cliente desconectado ou erro ao receber dados."
                << std::endl;
      break;
    }

    // TODO: implementar a lógica para enviar a mensagem para outros clientes
    std::string message(buffer);
    process_message(user, message);
  }

  {
    std::unique_lock<std::mutex> lock(client_mutex);
    (*connected_clients).erase(client_sockfd);
    std::cout << "Cliente removido do HUB." << std::endl;
  }

  delete user;
}

void ChatHub::process_message(ChatUser *user, const std::string &message) {
  // Mensagem chat username - linka o usuario atual com
  // Mensagem group group_name - linka o usuario atual com um grupo
  // Mensagem UsernameSrc UsernameDest msg - depois linkado envia a mensagem
  UserCommands cmd = process_command(user, message);

  switch (cmd) {
  case chat:
  case group: {
    // Inicializando as variáveis para os componentes da mensagem
    std::string username_src, username_dest, msg_content;
    std::istringstream iss(message);

    // Extraindo username_src, username_dest, e o corpo da mensagem
    if (!(iss >> username_src >> username_dest)) {
      std::cerr << "Erro no formato da mensagem." << std::endl;
      return;
    }

    // Pega o conteúdo da mensagem
    std::getline(iss, msg_content);

    // Remove o espaço extra
    if (!msg_content.empty() && msg_content[0] == ' ') {
      msg_content.erase(0, 1);
    }

    // Verificar se o destinatário está conectado
    ChatUser *dest_user = get_user(username_dest);
    if (!dest_user) {
      std::cerr << "Usuário " << username_dest
                << " não encontrado ou não está conectado." << std::endl;
      return;
    }

    // Enviar a mensagem para o destinatário
    dest_user->send_message(msg_content.c_str(), username_src);
    std::cout << "Mensagem enviada de " << username_src << " para "
              << username_dest << ": " << msg_content << std::endl;
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

UserCommands ChatHub::process_command(ChatUser *user,
                                      const std::string &message) {
  if (message.substr(0, 4) == "chat") {
    std::string target_user = message.substr(5);
    ChatUser *dest_user = get_user(target_user);
    if (dest_user) {
      std::string welcome_msg = "Chat iniciado com: " + target_user;
      user->send_message(welcome_msg.c_str(), user->get_username());
      return UserCommands::chat;
    } else {
      std::string error_msg = "Usuário " + target_user + " não encontrado.";
      user->send_message(error_msg.c_str(), user->get_username());
      return UserCommands::notfound;
    }
  } else if (message.substr(0, 5) == "group") {
    std::string group_name = message.substr(6);
    if (!group_name.empty()) {
      std::string welcome_msg = "Você entrou no grupo " + group_name;
      user->send_message(welcome_msg.c_str(), user->get_username());
      return UserCommands::group;
    }
  }
  return UserCommands::notfound;
}

ChatUser *ChatHub::get_user(const std::string &username) {
  std::unique_lock<std::mutex> lock(client_mutex);

  for (const auto &pair : *connected_clients) {
    ChatUser *user = pair.second;
    cout << user->get_username() << " " << username << endl;
    if (user->get_username() == username) {
      return user;
    }
  }

  return nullptr;
}
