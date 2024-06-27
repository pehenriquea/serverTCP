// server_win.c
#include <stdio.h>
#include <winsock2.h>

#define PORT 12345
#define MAX_CLIENTS 20
#define BUFFER_SIZE 256

typedef struct {
    int type;
    int user_id;
    int dest_id;
    int text_length;
    char text[141];
} message_t;

void handle_client(SOCKET client_socket, fd_set *master_set, int *client_sockets, int aux_sock);

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    fd_set master_set, read_fds;
    int client_sockets[MAX_CLIENTS] = {0};
    int max_sd, activity, addrlen, new_socket, aux_sock;
    char buffer[BUFFER_SIZE];

    // Inicializa o Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // Cria o socket do servidor
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Configura a estrutura de endereço do servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Associa o socket à porta e ao endereço local
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed. Error Code: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // Define o socket para escuta
    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Listen failed. Error Code: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // Inicializa o conjunto de descritores mestre e adiciona o socket do servidor a ele
    FD_ZERO(&master_set);
    FD_SET(server_socket, &master_set);
    max_sd = server_socket;

    printf("Server inicializado. Conecte o cliente de envio primeiro...\n");

    while (1) {
        read_fds = master_set;

        // Espera por atividade em algum dos sockets
        activity = select(0, &read_fds, NULL, NULL, NULL);

        if (activity == SOCKET_ERROR) {
            printf("Select error. Error Code: %d\n", WSAGetLastError());
            break;
        }

        // Verifica se há uma nova conexão pendente no socket do servidor
        if (FD_ISSET(server_socket, &read_fds)) {
            addrlen = sizeof(client_addr);
            if ((new_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addrlen)) == INVALID_SOCKET) {
                printf("Accept failed. Error Code: %d\n", WSAGetLastError());
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

            // Atualiza o maior descritor, se necessário
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
    }

    // Limpa e fecha os sockets e o Winsock
    closesocket(server_socket);
    WSACleanup();
    return 0;
}

void handle_client(SOCKET client_socket, fd_set *master_set, int *client_sockets, int aux_sock) {
    char buffer[BUFFER_SIZE];
    int bytes_received;

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
        printf("Falhou no recebimento. Código do Erro: %d\n", WSAGetLastError());
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
