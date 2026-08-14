#include "socket_listener.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
typedef SOCKET Socket;
#define INVALID_SOCKET_HANDLE INVALID_SOCKET
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
typedef int Socket;
#define INVALID_SOCKET_HANDLE -1
#define closesocket close
#endif

static Socket server_socket = INVALID_SOCKET_HANDLE;
static Socket client_socket = INVALID_SOCKET_HANDLE;
static struct sockaddr_in server_addr;
static char buffer[BUFFER_SIZE];
static bool initialized = false;

bool socket_listener_init(void) {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return false;
#endif
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET_HANDLE) return false;
#ifdef _WIN32
    u_long mode = 1;
    ioctlsocket(server_socket, FIONBIO, &mode);
#else
    int flags = fcntl(server_socket, F_GETFL, 0);
    fcntl(server_socket, F_SETFL, flags | O_NONBLOCK);
#endif
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_addr.sin_port = htons(SOCKET_PORT);
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) { closesocket(server_socket); return false; }
    if (listen(server_socket, 1) < 0) { closesocket(server_socket); return false; }
    printf("✅ Socket listener on port %d\n", SOCKET_PORT);
    initialized = true;
    return true;
}

bool socket_listener_update(Command* cmd) {
    if (!initialized) return false;
    if (client_socket == INVALID_SOCKET_HANDLE) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket != INVALID_SOCKET_HANDLE) {
            printf("✅ Client connected\n");
#ifdef _WIN32
            u_long mode = 1;
            ioctlsocket(client_socket, FIONBIO, &mode);
#else
            int flags = fcntl(client_socket, F_GETFL, 0);
            fcntl(client_socket, F_SETFL, flags | O_NONBLOCK);
#endif
        }
    }
    if (client_socket != INVALID_SOCKET_HANDLE) {
        int bytes_read = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            char* newline = strchr(buffer, '\n');
            if (newline) *newline = '\0';
            printf("📥 Command: %s\n", buffer);
            char command[64]; int value = 0;
            if (sscanf(buffer, "%63s %d", command, &value) == 2) {
                if (strcmp(command, "SET_DIRECTION") == 0) { cmd->type = CMD_SET_DIRECTION; cmd->value = value; return true; }
                if (strcmp(command, "SET_POWER") == 0) { cmd->type = CMD_SET_POWER; cmd->value = value; return true; }
            } else if (sscanf(buffer, "%63s", command) == 1) {
                if (strcmp(command, "EXECUTE_SHOT") == 0) { cmd->type = CMD_EXECUTE_SHOT; return true; }
                if (strcmp(command, "GET_SCORE") == 0) { cmd->type = CMD_GET_SCORE; return true; }
                if (strcmp(command, "RESET_GAME") == 0) { cmd->type = CMD_RESET_GAME; return true; }
            }
        } else if (bytes_read == 0) {
            printf("⚠️  Client disconnected\n");
            closesocket(client_socket);
            client_socket = INVALID_SOCKET_HANDLE;
        }
    }
    cmd->type = CMD_NONE;
    return false;
}

void socket_listener_send_response(const char* response) {
    if (client_socket != INVALID_SOCKET_HANDLE && response) send(client_socket, response, strlen(response), 0);
}

void socket_listener_cleanup(void) {
    if (client_socket != INVALID_SOCKET_HANDLE) { closesocket(client_socket); client_socket = INVALID_SOCKET_HANDLE; }
    if (server_socket != INVALID_SOCKET_HANDLE) { closesocket(server_socket); server_socket = INVALID_SOCKET_HANDLE; }
#ifdef _WIN32
    WSACleanup();
#endif
    initialized = false;
}

bool socket_listener_is_connected(void) { return client_socket != INVALID_SOCKET_HANDLE; }
