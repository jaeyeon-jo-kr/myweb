#ifndef __SOCKET_CLIENT__
#define __SOCKET_CLIENT__

struct client_send_str {
    char *str;
    size_t size;
};

typedef struct client_send_str client_send_str_type;

bool run_client();

#endif