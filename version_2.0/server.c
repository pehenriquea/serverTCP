// server.c
// Funcao principal onde a gerencia dos clientes e feita e gerenciada para ser distribuida para os
// clientes, seja o display ou o send (de envio)

#include <stdio.h>
#include <winsock2.h>
#include <time.h>

#define PORT 12345
#define MAX_CLIENTS 20
#define BUFFER_SIZE 256

// Estrutura auxiliar para o temporizador
typedef struct {
    int type;
    int user_id;
    int dest_id;
    int text_length;
    char text[141];
} message_t;

// Declarações de funções
void handle_client(SOCKET client_socket, fd_set *master_set, int *client_sockets, int aux_sock);
void send_periodic_message(SOCKET server_socket, fd_set *master_set, int *client_sockets);

// Função Main do servidor
int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    fd_set master_set, read_fds;
    int client_sockets[MAX_CLIENTS] = {0};
    int max_sd, activity, addrlen, new_socket, aux_sock;
    char buffer[BUFFER_SIZE];
    time_t start_time, current_time;

    // Inicializa o Socket Windows
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Falha na inicialização do socket Windows. Codigo de erro: %d\n", WSAGetLastError());
        return 1;
    }

    // Cria o socket do servidor
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Falha na criacao do socket. Codigo de erro: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Configura a estrutura de endereço do servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Associa o socket à porta e ao endereço local
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Falha na associacao. Codigo de erro: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // Define o socket para escuta
    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Falha na escuta. Codigo de erro: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // Inicializa o conjunto de clientes: primeiro o que envia, depois o que escuta
    FD_ZERO(&master_set);
    FD_SET(server_socket, &master_set);
    max_sd = server_socket;

    printf("Server inicializado. Conecte os clientes\n");

    // Obtém o tempo de início do servidor
    time(&start_time);
    while (1) {
        read_fds = master_set;

        // Espera por atividade em algum dos sockets
        activity = select(0, &read_fds, NULL, NULL, NULL);

        if (activity == SOCKET_ERROR) {
            printf("Erro na escolha do socket. Codigo de erro: %d\n", WSAGetLastError());
            break;
        }

        // Verifica se há uma nova conexão pendente no socket do servidor
        if (FD_ISSET(server_socket, &read_fds)) {
            addrlen = sizeof(client_addr);
            if ((new_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addrlen)) == INVALID_SOCKET) {
                printf("Falha na conexao. Codigo de erro: %d\n", WSAGetLastError());
                continue;
            }

            // Adiciona o novo socket à lista de descritores mestre
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = new_socket;
                    printf("OI\n",
                           new_socket, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                    break;
                }
            }

            aux_sock = new_socket;

            FD_SET(new_socket, &master_set);

            // Atualiza o maior socket, se necessario
            if (new_socket > max_sd) {
                max_sd = new_socket;
            }
        }

        // Trata os clientes existentes
        for (int i = 0; i < MAX_CLIENTS; i++) {
            client_socket = client_sockets[i];

            if (FD_ISSET(client_socket, &read_fds)) {
                handle_client(client_socket, &master_set, client_sockets, aux_sock);
            }
        }

        // Envia mensagem periódica a cada minuto
        time(&current_time);
        if (difftime(current_time, start_time) >= 60) {
            send_periodic_message(server_socket, &master_set, client_sockets);
            time(&start_time); // Reinicia o contador de tempo
        }
    }

    // Limpa e fecha os sockets e o Winsock
    closesocket(server_socket);
    WSACleanup();
    return 0;
}

// Funcao que e responsavel por verificar a mensagem que chegou e transmiti-la para o display
void handle_client(SOCKET client_socket, fd_set *master_set, int *client_sockets, int aux_sock) {
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    int bytes_received;
    int or_id, dst_id, len;

    // Recebe a mensagem do cliente
    bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
    printf("MSG\n");

    if (bytes_received > 0) {
        // Envia a mensagem para o display
        send(aux_sock, buffer, BUFFER_SIZE, 0);

    } else if (bytes_received == 0) {
        // Cliente desconectado
        printf("TCHAU\n");
        closesocket(client_socket);
        FD_CLR(client_socket, master_set);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] == client_socket) {
                client_sockets[i] = 0;
                break;
            }
        }
    } else {
        // Erro ao receber dados
        printf("Falhou no recebimento. Codigo do Erro: %d\n", WSAGetLastError());
        closesocket(client_socket);
        FD_CLR(client_socket, master_set);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] == client_socket) {
                client_sockets[i] = 0;
                break;
            }
        }
    }
}

// Funcao que e responsavel por disparar mensagem periodica de verificacao de conexao
void send_periodic_message(SOCKET server_socket, fd_set *master_set, int *client_sockets) {
    message_t message;
    time_t current_time;

    // Prepara a mensagem periódica
    message.type = 3; // Tipo da mensagem periodica
    message.user_id = 0; // ID do server
    message.dest_id = 0; // dest_id 0 significa server para envio de mensagem
    message.text_length = snprintf(message.text, sizeof(message.text), "Server periodic message");

    // Envia a mensagem periódica para todos os clientes de exibição
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (client_sockets[i] == 0) { // Display client
            send(client_sockets[i], (char *)&message, sizeof(message), 0);
        }
    }

    // Atualiza o tempo de início para a próxima mensagem periódica
    time(&current_time);
}
