#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h> /* See NOTES */
#include "msg.h"
#include "log.h"

int main(int argc, char **argv)
{
    struct sockaddr_in serveraddr;
    int listenfd;
    int sock;
    int ret;

    log_create("server.txt");

    // socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        log_write("socket failed, ret %d\n", listenfd);
        return -1;
    }

    // bind端口，SERVER_PORT
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = SERVER_PORT;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    ret = bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (ret < 0) {
        log_write("bind failed ,ret  %d\n", ret);
        return -1;
    }
    // listen，启用监听模式
    ret = listen(listenfd, 0);
    if (ret < 0) {
        log_write("listen failed ,ret  %d\n", ret);
        return -1;
    }
    // accept，返回已经完成3次握手的socket
    sock = accept(listenfd, NULL, 0);
    if (ret < 0) {
        log_write("accept failed ,ret  %d\n", ret);
        return -1;
    }

    log_write("client connect.\n");

    // 读取hello

    char buf[32];

    read(sock, buf, 32, 0);

    log_write("%s", buf);

    log_destroy();
    return 0;
}
