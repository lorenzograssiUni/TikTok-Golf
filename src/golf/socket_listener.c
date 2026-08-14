#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#endif

#define SOCKET_LISTENER_PORT 55555
#define SOCKET_BUFFER_SIZE 256

#ifdef _WIN32
typedef SOCKET socket_t;
#define INVALID_SOCKET_VALUE INVALID_SOCKET
#define SOCKET_ERROR_VALUE SOCKET_ERROR
#define CLOSE_SOCKET(s) closesocket(s)
#define LAST_ERROR WSAGetLastError()
#else
typedef int socket_t;
#define INVALID_SOCKET_VALUE (-1)
#define SOCKET_ERROR_VALUE (-1)
#define CLOSE_SOCKET(s) close(s)
#define LAST_ERROR errno
#endif

static socket_t g_socket = INVALID_SOCKET_VALUE;
static int g_initialized = 0;

int
socket_listener_init(void)
{
#ifdef _WIN32
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        fprintf(stderr, "WSAStartup failed: %d\n", result);
        return -1;
    }
#endif

    g_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (g_socket == INVALID_SOCKET_VALUE) {
        fprintf(stderr, "socket() failed\n");
#ifdef _WIN32
        WSACleanup();
#endif
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(SOCKET_LISTENER_PORT);

    if (bind(g_socket, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR_VALUE) {
        fprintf(stderr, "bind() failed\n");
        CLOSE_SOCKET(g_socket);
        g_socket = INVALID_SOCKET_VALUE;
#ifdef _WIN32
        WSACleanup();
#endif
        return -1;
    }

    g_initialized = 1;
    return 0;
}

int
socket_listener_poll(char *out_buf, int out_buf_size)
{
    if (!g_initialized || g_socket == INVALID_SOCKET_VALUE) {
        return 0;
    }

    struct sockaddr_in client_addr;
    int client_len = sizeof(client_addr);
    char buffer[SOCKET_BUFFER_SIZE];

    int n = recvfrom(
        g_socket,
        buffer,
        (int)sizeof(buffer),
        0,
        (struct sockaddr *)&client_addr,
        &client_len
    );

    if (n == SOCKET_ERROR_VALUE || n <= 0) {
        return 0;
    }

    if (n >= out_buf_size) {
        n = out_buf_size - 1;
    }

    memcpy(out_buf, buffer, n);
    out_buf[n] = '\0';

    return n;
}

void
socket_listener_cleanup(void)
{
    if (g_socket != INVALID_SOCKET_VALUE) {
        CLOSE_SOCKET(g_socket);
        g_socket = INVALID_SOCKET_VALUE;
    }
#ifdef _WIN32
    if (g_initialized) {
        WSACleanup();
    }
#endif
    g_initialized = 0;
}