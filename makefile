# Flags do compilador
CXXFLAGS = -std=c++11 -Wall

# Diretórios
SRC_DIR = src
CHAT_DIR = $(SRC_DIR)/chat
CLIENT_DIR = $(SRC_DIR)/client
SERVER_SOCKET_DIR = $(SRC_DIR)/server/server_socket
CLIENT_SOCKET_DIR = $(SRC_DIR)/server/client_socket
SOCKET_DIR = $(SRC_DIR)/server/socket

# Arquivos fontes
SRC_FILES = \
    $(CHAT_DIR)/ChatHub.cpp \
    $(CLIENT_DIR)/ChatUser.cpp \
    $(SERVER_SOCKET_DIR)/ServerSocket.cpp \
    $(CLIENT_SOCKET_DIR)/ClientSocket.cpp \
    $(SOCKET_DIR)/socket.cpp \
    $(SRC_DIR)/main.cpp

# Arquivo de saída
OUTPUT = chat_app

# Regra padrão para compilar o projeto
all: $(OUTPUT)

# Regra para compilar o executável
$(OUTPUT): $(SRC_FILES)
	g++ $(CXXFLAGS) $(SRC_FILES) -o $(OUTPUT)

# Regra para limpar os arquivos gerados
clean:
	rm -f $(OUTPUT)

.PHONY: all clean
