// display_client.c
// Arquivo que descreve o funcionamento do display, onde as mensagens serao exibidas

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>

#define BUFFER_SIZE 256

// Definicao da funcao principal do servidor display
int main(int argc, char* argv[]) {

    // Em caso de erro na especificacao do servidor
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <ID> <IP do Servidor> <Porta do Servidor>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Le os dados digitados na inicializacao do servidor
    int id = atoi(argv[1]);
    char* server_ip = argv[2];
    int server_port = atoi(argv[3]);

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Dispara erro em caso de falha na criacao do socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Erro na criacao do socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    // Dispara erro em caso de falha na conexao do socket
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Falha na conexao");
        exit(EXIT_FAILURE);
    }
    
    // Loop de escuta do servidor, onde a verificacao de chegada de mensagem sera feita
    while (1) {        
        char buffer[BUFFER_SIZE];
        int or_id, dst_id, len;
        char message[BUFFER_SIZE];

        // Recebe a mensagem do cliente
        recv(sock, buffer, BUFFER_SIZE, 0);

        sscanf(buffer, "%d %d %d %[^\n]", &or_id, &dst_id, &len, message);

        // Exibe a mensagem do cliente
        if (dst_id == id)
            printf("Mensagem do cliente com ID %d: %s\n", dst_id, message);

    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
