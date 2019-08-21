#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h> /* See NOTES */
#include "log.h"
#include "msg.h"
#include "utils.h"

/**
 * @brief 把用户输入的字符串转为FTP_CMD
 *
 * @param buf
 *
 * @return 
 */
enum FTP_CMD get_cmd(char *buf, struct Msg *msg)
{
    char *ptr_args;

    if (0 == memcmp(buf, "ls", 2)) {
        return FTP_CMD_LS;
    }

    return FTP_CMD_ERROR;
}

/**
 * @brief 等待用户输入，并处理
 */
int handle_user_input(struct Msg *msg_send)
{
    char buf[32];
    enum FTP_CMD cmd;

    // 等待用户输入
    printf("input cmd:\n");
    fgets(buf, 32, stdin);
    log_write("%s\n", buf);

    // buf转为FTP_CMD
    cmd = get_cmd(buf, msg_send);
    log_write("cmd %d\n", cmd);
    if (cmd == FTP_CMD_ERROR) {
        return -1;
    }

    // 初始化结构体struct Msg
    msg_send->cmd = cmd;
    strcpy(msg_send->args, buf);

    return 0;
}

/**
 * @brief 读取用户输入，初始化msg_send
 *
 * @param msg_send
 *
 * @return  0成功，-1失败
 */
int handle_user_input2(struct Msg *msg_send)
{
    char buf[32];
    enum FTP_CMD cmd;

    // 读取命令
    fgets(buf, 32, stdin);

    // 从键盘读取的数据，全部写入日志
    // 打印调试信息
    log_write("%s", buf);


    // 检测这是什么命令？
    // 不支持其他命令，只支持ls命令
    // 识别到ls命令
    if (0 == memcmp(buf, "ls", 2)) {
        cmd = FTP_CMD_LS;
    } else if (0 == memcmp(buf, "hist", 4)) {
        cmd = FTP_CMD_HIST;
    } else if (0 == memcmp(buf, "cd", 2)) {
        cmd = FTP_CMD_CD;
    } else if (0 == memcmp(buf, "get", 3)) {
        cmd = FTP_CMD_GET;
    } else if (0 == memcmp(buf, "quit", 4)) {
        cmd = FTP_CMD_QUIT;
    } else if (0 == memcmp(buf, "put", 3)) {
        cmd = FTP_CMD_PUT;
        // 解析命令，获取文件名
        char filename[32];
        if (split_string2(buf, filename) < 0) {
            log_write("filename not find");
            return -1;
        }

        long length = get_length(filename);
        // 文件长度为-1或者超过最大值
        if (length < 0 || length > sizeof(msg_send->data)) {
            log_write("get_length failed, length %d\n", length);
            return -1;
        }

        // 获取文件md5
        get_md5(filename, msg_send->md5);

        // 把文件内容写入data
        // #define NULL 0
        FILE *fp = fopen(filename, "r");
        if (NULL != fp) {
            // 设置data_length
            msg_send->data_length = fread(msg_send->data, 1, length, fp);
            log_write("fread %d", msg_send->data_length);
            fclose(fp);
        } else {
            log_write("filename not find, %s", filename);
            return -1;
        }
    } else {
        cmd = FTP_CMD_ERROR;
    }

    // 命令不支持，返回失败
    if (cmd == FTP_CMD_ERROR) {
        return -1;
    }

    // 初始化msg_send
    msg_send->cmd = cmd;
    strcpy(msg_send->args, buf);

    // 返回成功
    return 0;
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

    // 成功建立tcp连接
    log_write("connect server sucess\n");

    struct Auth auth;

    // 输入用户名密码
    printf("username:");
    scanf("%s", auth.username);
    printf("password:");
    scanf("%s", auth.password);
    auth.cmd = FTP_CMD_AUTH;

    // 发送给服务端验证
    ret = send(sock, &auth, sizeof(struct Auth), 0);
    log_write("send ret %d", ret);

    // 判断用户名密码是否正确
    ret = recv(sock, &auth, sizeof(struct Auth), 0);
    if (FTP_CMD_ERROR == auth.cmd) {
        printf("username or password error\n");
        return -1;
    }
    printf("well done\n");

    while(1) {

        // 1. 等待用户输入, 初始化msg_send
        if (handle_user_input2(msg_send) < 0) {
            continue;
        }

        // 2. 发送
        ret = send(sock, msg_send, sizeof(struct Msg), 0);
        log_write("send ret %d\n", ret);

        // 3. 接收
        memset(msg_recv, 0, sizeof(struct Msg));
        ret = recv(sock, msg_recv, sizeof(struct Msg), 0);
        log_write("recv ret %d\n", ret);
        log_write("cmd %d\n", msg_recv->cmd);
        log_write("data %s\n", msg_recv->data);

        // 如果是历史记录或LS
        if (FTP_CMD_HIST == msg_recv->cmd || FTP_CMD_LS == msg_recv->cmd) {
            printf("%s", msg_recv->data);
        } else if (FTP_CMD_GET == msg_recv->cmd) {
            // get file.txt
            // file.txt
            // _file.txt
            char filename[32];
            filename[0] = '_';
            split_string2(msg_send->args, &filename[1]);
            FILE *fp = fopen(filename, "w");
            if (fp != NULL) {
                ret = fwrite(msg_recv->data, 1, msg_recv->data_length, fp);
                log_write("fwrite ret %d", ret);
                fclose(fp);
            }

            char md5[64];
            get_md5(filename, md5);
            if (memcmp(md5, msg_recv->md5, 32) != 0) {
                // md5不一样，删掉服务端的文件
                remove(filename);
                log_write("client %s, server %s\n", msg_recv->md5, md5);
            }
        } else if (FTP_CMD_QUIT == msg_recv->cmd) {
            printf("byebye\n");
            break;
        }

    }

    log_destroy();
    return 0;
}
