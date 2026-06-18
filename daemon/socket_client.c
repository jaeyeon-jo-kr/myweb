#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "socket_client.h"
#include "resource_monitor.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 4567

bool run_client()
{
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("소켓 생성 실패");
        return 1;
    }
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("서버 연결 실패 (Java 서버가 켜져 있는지 확인하세요)");
        close(sock_fd);
        return false;
    }

    cpu_info_type cpu_info;
    try_get_cpu_info(&cpu_info);
    char *send_buffer;
    size_t send_buffer_len;
    try_convert_to_server_str(&cpu_info, &send_buffer, &send_buffer_len);

    if (write(sock_fd, send_buffer, strlen(send_buffer)) < 0) {
        perror("데이터 전송 실패");
        free(send_buffer);
    } else {
        printf("[C 데몬] 데이터 전송 완료: %s", send_buffer); // \n이 포함되어 있음
    }
    
    close(sock_fd);
    release_cpu_info(&cpu_info);
    return true;
}