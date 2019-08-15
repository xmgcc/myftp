#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h> /* See NOTES */
#include <errno.h>
#include "msg.h"
#include "log.h"
#include "utils.h"
/**
 * @brief 处理客户端的命令in_cmd，并返回处理结果out_cmd
 *
 * @param in_cmd
 * @param out_cmd
 */
void handle_cmd(struct Msg *in_cmd, struct Msg *out_cmd)
{
    FILE *fp = NULL;
    int ret;

    out_cmd->cmd = in_cmd->cmd;

    switch(in_cmd->cmd) {
        case FTP_CMD_LS:
            fp = popen(in_cmd->args, "r");
            if (NULL != fp) {
                ret = fread(out_cmd->data, 1, sizeof(out_cmd->data), fp);
                // 一次读取5000个字节，读取一次，因为实际大小小于5000，导致EOF
                //ret = fread(out_cmd->data, sizeof(out_cmd->data), 1, fp);
                log_write("fread ret %d, eof %d, data %s\n",
                        ret, feof(fp), out_cmd->data);
                pclose(fp);
            }
            break;
        default:
            break;
    }
}

/**
 * @brief 处理客户端的命令in_cmd，把结果写入out_cmd
 *
 * @param in_cmd
 * @param out_cmd
 */
void handle_cmd2(struct Msg *in_cmd, struct Msg *out_cmd)
{
    // in_cmd 从网络读取度数据，全部写入日志，用于调试
    log_write("cmd %d, args %s\n", in_cmd->cmd, in_cmd->args);

    // 返回的命令
    out_cmd->cmd = in_cmd->cmd;

    // 判断in_cmd的命令类型
    if (FTP_CMD_LS == in_cmd->cmd) {
        FILE *fp = popen(in_cmd->args, "r");
        if (fp != NULL) {
            int ret = fread(out_cmd->data, 1, sizeof(out_cmd->data), fp);
            log_write("fread ret %d, %s", ret, out_cmd->data);
            pclose(fp);
        }
    } else if (FTP_CMD_GET == in_cmd->cmd) {
        char filename[32];
        // 分割字符
        if (split_string2(in_cmd->args, filename) < 0) {
            out_cmd->cmd = FTP_CMD_ERROR;
            log_write("filename not find\n");
            return;
        }

        FILE *fp = fopen(filename, "r");
        if (fp != NULL) {
            // 一次性读取5000字节，如果文件小于5000字节，整个文件就读取结束
            int ret = fread(out_cmd->data, 1, sizeof(out_cmd->data), fp);
            out_cmd->data_length = ret;
            log_write("fread ret %d, eof %d\n", ret, feof(fp));
            fclose(fp);
        } else {
            out_cmd->cmd = FTP_CMD_ERROR;
            log_write("filename not find %s\n", filename);
        }
    }
}

int main(int argc, char **argv)
{
    struct sockaddr_in serveraddr;
    int listenfd;
    int sock;
    int ret;
    struct Msg *msg_recv = NULL;
    struct Msg *msg_send = NULL;

    msg_recv = (struct Msg *)malloc(sizeof(struct Msg));
    msg_send = (struct Msg *)malloc(sizeof(struct Msg));

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

    // 配置socket
    // 允许地址重用
    int on = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    ret = bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (ret < 0) {
        log_write("bind failed, ret %d, errno %d %s\n", ret, errno, strerror(errno));
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

    // 成功建立TCP连接
    log_write("client connect.\n");

    while (1) {
        // 1. 接收到客户端命令
        ret = recv(sock, msg_recv, sizeof(struct Msg), 0);
        log_write("recv %d\n", ret);

        // 2. handle cmd处理客户端命令
        memset(msg_send, 0, sizeof(struct Msg));
        handle_cmd2(msg_recv, msg_send);

        // 3. 发送处理结果给客户端
        ret = send(sock, msg_send, sizeof(struct Msg), 0);
        log_write("send %d\n", ret);
    }

    log_destroy();
    return 0;
}
