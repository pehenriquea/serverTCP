// send_client.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>

#define BUFFER_SIZE 256

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <ID> <Server IP> <Server Port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int id = atoi(argv[1]);
    char* server_ip = argv[2];
    int server_port = atoi(argv[3]);

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    sprintf(buffer, "1 %d", id);

    while (1) {
        printf("Enter message (format: <destination_id> <message>): ");
        fgets(buffer, BUFFER_SIZE, stdin);
        int dst_id;
        char message[BUFFER_SIZE];
        sscanf(buffer, "%d %[^\n]", &dst_id, message);
        sprintf(buffer, "%d %d %d %s", id, dst_id, strlen(message), message);
        send(sock, buffer, strlen(buffer), 0);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
