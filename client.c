#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h> /* See NOTES */
#include "log.h"
#include "msg.h"

enum FTP_CMD get_cmd(char *buf)
{
    return FTP_CMD_ERROR;
}

int main(int argc, char **argv)
{
    int ret;
    int sock;
    struct sockaddr_in serveraddr;
    struct Msg *msg_send = NULL;
    struct Msg *msg_recv = NULL;
    msg_send = (struct Msg *)malloc(sizeof(struct Msg));
    msg_recv = (struct Msg *)malloc(sizeof(struct Msg));

    log_create("client.txt");

    // 1 创建socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    // 2. 初始化服务端地址, ip, port
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = SERVER_PORT;
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");


    // 3. 连接connect
    ret = connect(sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (ret < 0) {
        // 链接服务端失败，退出程序
        log_write("connect failed, ret %d\n", ret);
        return -1;
    }

    
    while(1) {
        char buf[32];
        enum FTP_CMD cmd;
        
        // 等待用户输入
        scanf("%s", buf);
        log_write("%s", buf);

        // buf转为FTP_CMD
        cmd = get_cmd(buf);
        log_write("cmd %d", cmd);
        switch (cmd) {
            case FTP_CMD_LS:
                break;
            default:
                break;
        }

        // 初始化结构体struct Msg
        msg_send->cmd = cmd;


        // 发送
        ret = send(sock, msg_send, sizeof(struct Msg), 0);
        log_write("send ret %d", ret);

        // 接收
        ret = recv(sock, msg_recv, sizeof(struct Msg), 0);
        log_write("recv ret %d", ret);
    }

    log_destroy();
    return 0;
}
