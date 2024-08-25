#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <unistd.h>

#pragma comment(lib, "Ws2_32.lib")

#define BUFFER_SIZE 1024

void handle_incoming_connections(SOCKET client_sock) {
    char buffer[BUFFER_SIZE];
    int bytes_received;

    while ((bytes_received = recv(client_sock, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[bytes_received] = '\0';
        printf("\nReceived: %s\n", buffer);
    }

    closesocket(client_sock);
}

int main(int argc, char *argv[]) {
    WSADATA wsa;
    SOCKET listen_sock, client_sock, peer_sock;
    struct sockaddr_in server, client, peer;
    char buffer[BUFFER_SIZE];
    int addr_len = sizeof(struct sockaddr_in);
    int port;
    fd_set readfds;

    if (argc != 3) {
        printf("Usage: %s <port> <peer_ip>\n", argv[0]);
        return 1;
    }

    port = atoi(argv[1]);

    // Initialize Winsock
    WSAStartup(MAKEWORD(2, 2), &wsa);

    // Create a socket for listening
    listen_sock = socket(AF_INET, SOCK_STREAM, 0);

    // Prepare the sockaddr_in structure for the server
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    // Bind
    bind(listen_sock, (struct sockaddr *)&server, sizeof(server));

    // Listen for incoming connections
    listen(listen_sock, 3);
    printf("Listening on port %d...\n", port);

    //Act as a client and connect to the peer
    peer_sock = socket(AF_INET, SOCK_STREAM, 0);

    peer.sin_family = AF_INET;
    peer.sin_addr.s_addr = inet_addr(argv[2]);
    peer.sin_port = htons(port);

    // Connect to the peer's server
    if (connect(peer_sock, (struct sockaddr *)&peer, sizeof(peer)) < 0) {
        printf("Connection to peer failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Connected to peer %s:%d\n", argv[2], port);

    // Accept incoming connections
    u_long mode = 1; 
    ioctlsocket(listen_sock, FIONBIO, &mode);

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(peer_sock, &readfds);
        FD_SET(listen_sock, &readfds);
        FD_SET(STDIN_FILENO, &readfds);

        int activity = select(0, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(listen_sock, &readfds)) {
            client_sock = accept(listen_sock, (struct sockaddr *)&client, &addr_len);
            if (client_sock != INVALID_SOCKET) {
                handle_incoming_connections(client_sock);
            }
        }

        if (FD_ISSET(peer_sock, &readfds)) {
            int bytes_received = recv(peer_sock, buffer, BUFFER_SIZE, 0);
            if (bytes_received > 0) {
                buffer[bytes_received] = '\0';
                printf("\nReceived from peer: %s\n", buffer);
            }
        }

        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            // Read input from the user
            if (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
                send(peer_sock, buffer, strlen(buffer), 0);
            }
        }
    }

    closesocket(listen_sock);
    closesocket(peer_sock);
    WSACleanup();

    return 0;
}
