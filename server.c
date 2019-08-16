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

int g_running;

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
    // 局部静态变量，他的值一直存在
    static struct LinkList *hist = NULL;

    // in_cmd 从网络读取度数据，全部写入日志，用于调试
    log_write("cmd %d, args %s\n", in_cmd->cmd, in_cmd->args);

    // 返回的命令
    out_cmd->cmd = in_cmd->cmd;

    // 保存命令
    linklist_insert(&hist, in_cmd->args);

    // 判断in_cmd的命令类型
    if (FTP_CMD_LS == in_cmd->cmd) {
        FILE *fp = popen(in_cmd->args, "r");
        if (fp != NULL) {
            int ret = fread(out_cmd->data, 1, sizeof(out_cmd->data), fp);
            log_write("fread ret %d, %s", ret, out_cmd->data);
            pclose(fp);
        }
    } else if (FTP_CMD_HIST == in_cmd->cmd) {
        linklist_get_cmd(hist, out_cmd->data);
    } else if (FTP_CMD_CD == in_cmd->cmd) {
        char dirname[32];

        // 获取文件夹名字
        if (split_string2(in_cmd->args, dirname) < 0) {
            return;
        }

        // 切换目录
        int ret = chdir(dirname);
        log_write("chdir ret %d", ret);
    } else if (FTP_CMD_QUIT == in_cmd->cmd) {
        log_write("quit\n");
        g_running = 0;
    } else if (FTP_CMD_GET == in_cmd->cmd) {
        char filename[32];
        // 分割字符
        if (split_string2(in_cmd->args, filename) < 0) {
            out_cmd->cmd = FTP_CMD_ERROR;
            log_write("filename not find\n");
            return;
        }

        long length = get_length(filename);
        if (length < 0 || length > sizeof(out_cmd->data)) {
            out_cmd->cmd = FTP_CMD_ERROR;
            log_write("get_length failed, filename %s\n", filename);
            return;
        }

        get_md5(filename, out_cmd->md5);

        FILE *fp = fopen(filename, "r");
        if (fp != NULL) {
            int ret = fread(out_cmd->data, 1, length, fp);
            out_cmd->data_length = ret;
            log_write("fread ret %d, eof %d\n", ret, feof(fp));
            fclose(fp);
        } else {
            out_cmd->cmd = FTP_CMD_ERROR;
            log_write("filename not find %s\n", filename);
        }
    } else if (FTP_CMD_PUT == in_cmd->cmd) {
        // 获取文件名,以+开头
        char filename[32];
        filename[0] = '+';
        split_string2(in_cmd->args, &filename[1]);

        // 把文件内容写入文件
        FILE *fp = fopen(filename, "w");
        if (fp != NULL) {
            int ret = fwrite(in_cmd->data, 1, in_cmd->data_length, fp);
            log_write("fwrite ret %d, filename %s, data_length %d\n",
                      ret, filename, in_cmd->data_length);
            fclose(fp);
        }

        // 对比客户端的md5
        char md5[64];
        get_md5(filename, md5);
        if (memcmp(md5, in_cmd->md5, 32) != 0) {
            // md5不一样，删掉服务端的文件
            remove(filename);
            log_write("client %s, server %s\n", in_cmd->md5, md5);
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

    // 读取用户名密码
    struct Auth auth;
    ret = recv(sock, &auth, sizeof(struct Auth), 0);
    log_write("%s %s\n", auth.username, auth.password);

    // 获取本地用户名密码
    struct Auth server;
    FILE *fp = fopen("passwd", "r");
    if (fp != NULL) {
        fscanf(fp, "%s %s", server.username, server.password);
        log_write("server %s %s\n", server.username, server.password);
        fclose(fp);
    }
 
    // 校验用户名密码
    if (0 != memcmp(auth.username, server.username, strlen(server.username)) ||
        0 != memcmp(auth.password, server.password, strlen(server.password))) {
        // 不一样
        auth.cmd = FTP_CMD_ERROR;
        log_write("auth failed\n");
    }

    // 发送检验结果
    ret = send(sock, &auth, sizeof(struct Auth), 0);
    log_write("send %d\n", ret);

    if (FTP_CMD_ERROR == auth.cmd) {
        return -1;
    }

    // 服务端运行
    g_running = 1;

    while (g_running) {
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
