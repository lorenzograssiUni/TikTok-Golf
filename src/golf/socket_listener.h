#ifndef SOCKET_LISTENER_H
#define SOCKET_LISTENER_H

#ifdef __cplusplus
extern "C" {
#endif

int socket_listener_init(void);
int socket_listener_poll(char *out_buf, int out_buf_size);
void socket_listener_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif /* SOCKET_LISTENER_H */