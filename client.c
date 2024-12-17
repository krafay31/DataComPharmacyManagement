#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")  // Link Winsock library

void handle_response(SOCKET sock) {
    char response[1024] = {0};
    int bytes_received = recv(sock, response, 1024, 0);
    if (bytes_received > 0) {
        printf("Server: %s\n", response);
    }
}

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;
    char server_ip[] = "127.0.0.1"; // Server IP
    int port = 12345;

    // Initialize Winsock
    printf("Initializing Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock. Error Code : %d\n", WSAGetLastError());
        return 1;
    }
    printf("Winsock initialized.\n");

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code : %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    server.sin_addr.s_addr = inet_addr(server_ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Connection to server failed. Error Code : %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    printf("Connected to server.\n");

    // Menu loop
    while (1) {
        printf("\n1. Add Medicine\n2. Search Medicine\n3. Delete Medicine\n4. Sell Medicine\n5. Exit\nEnter your choice: ");
        int choice;
        scanf("%d", &choice);
        getchar(); // Clear newline character

        if (choice == 1) {
            char name[50];
            int quantity;
            printf("Enter medicine name: ");
            fgets(name, sizeof(name), stdin);
            name[strcspn(name, "\n")] = 0; // Remove newline
            printf("Enter quantity: ");
            scanf("%d", &quantity);
            getchar();

            char request[1024];
            sprintf(request, "ADD:%s,%d", name, quantity);
            send(sock, request, strlen(request), 0);
            handle_response(sock);
        } else if (choice == 2) {
            char name[50];
            printf("Enter medicine name to search: ");
            fgets(name, sizeof(name), stdin);
            name[strcspn(name, "\n")] = 0;

            char request[1024];
            sprintf(request, "SEARCH:%s", name);
            send(sock, request, strlen(request), 0);
            handle_response(sock);
        } else if (choice == 3) {
            char name[50];
            printf("Enter medicine name to delete: ");
            fgets(name, sizeof(name), stdin);
            name[strcspn(name, "\n")] = 0;

            char request[1024];
            sprintf(request, "DELETE:%s", name);
            send(sock, request, strlen(request), 0);
            handle_response(sock);
        } else if (choice == 4) {
            char name[50];
            int quantity;
            printf("Enter medicine name: ");
            fgets(name, sizeof(name), stdin);
            name[strcspn(name, "\n")] = 0;
            printf("Enter quantity to sell: ");
            scanf("%d", &quantity);
            getchar();

            char request[1024];
            sprintf(request, "SELL:%s,%d", name, quantity);
            send(sock, request, strlen(request), 0);
            handle_response(sock);
        } else if (choice == 5) {
            printf("Exiting...\n");
            break;
        } else {
            printf("Invalid choice. Try again.\n");
        }
    }

    // Clean up
    closesocket(sock);
    WSACleanup();
    return 0;
}
