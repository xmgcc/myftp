#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h> /* See NOTES */
#include "log.h"
#include "msg.h"

/**
 * @brief 把用户输入的字符串转为FTP_CMD
 *
 * @param buf
 *
 * @return 
 */
enum FTP_CMD get_cmd(char *buf)
{
    return FTP_CMD_ERROR;
}

/**
 * @brief 等待用户输入，并处理
 */
void handle_user_input(struct Msg *msg_send)
{
    char buf[32];
    enum FTP_CMD cmd;

    // 等待用户输入
    scanf("%s", buf);
    log_write("%s", buf);

    // buf转为FTP_CMD
    // cmd = get_cmd(buf);
    cmd = FTP_CMD_LS;
    log_write("cmd %d\n", cmd);
    switch (cmd) {
        case FTP_CMD_LS:
            break;
        default:
            break;
    }

    // 初始化结构体struct Msg
    msg_send->cmd = cmd;
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
    log_write("recv \n");

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

    // 成功建立tcp连接
    log_write("connect server sucess");

    
    while(1) {

        // 1. 等待用户输入
        handle_user_input(msg_send);

        // 2. 发送
        ret = send(sock, msg_send, sizeof(struct Msg), 0);
        log_write("send ret %d\n", ret);

        // 3. 接收
        ret = recv(sock, msg_recv, sizeof(struct Msg), 0);
        log_write("recv ret %d\n", ret);
        log_write("data %s\n", msg_recv->data);
    }

    log_destroy();
    return 0;
}
