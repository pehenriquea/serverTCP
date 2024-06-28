// send_client.c
// Funcao que realiza o envio da informacao para o servdor, onde sera tratado e enviado para o
// servidor de display.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>

#define BUFFER_SIZE 256

// Inicio da funcao principal do servidor
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

    // Loop responsavel por sempre requisitar uma mensagem para o usuario
    while (1) {
        printf("Digite a mensagem (formato: <ID de Destino> <Mensagem>): ");
        fgets(buffer, BUFFER_SIZE, stdin);
        int dst_id;
        char message[BUFFER_SIZE];

        // Responsavel por ler o que foi digitado e separar por variaveis
        sscanf(buffer, "%d %[^\n]", &dst_id, message);
        sprintf(buffer, "%d %d %d %s", id, dst_id, strlen(message), message);
        send(sock, buffer, strlen(buffer), 0);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
