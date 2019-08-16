#ifndef MSG_H
#define MSG_H

#define SERVER_PORT 8888

enum FTP_CMD {
    // ls
    FTP_CMD_LS = 0,
    // get 下载
    FTP_CMD_GET = 1,
    // put 上传
    FTP_CMD_PUT = 2,
    // quit 断开连接byebye
    FTP_CMD_QUIT = 3,
    // cd 切换服务端目录
    FTP_CMD_CD = 4,

    // 验证用户名密码
    FTP_CMD_AUTH = 5,

    // 历史纪律
    FTP_CMD_HIST= 6,

    // 无效的命令
    FTP_CMD_ERROR,
};

struct Auth {
    enum FTP_CMD cmd;

    // 用户名
    char username[32];
    // 密码
    char password[32];
};

struct Msg {
    // 命令
    enum FTP_CMD cmd;

    // 命令行
    char args[32];

    // md5校验值
    char md5[64];

    // data的实际长度
    int data_length;

    // data
    char data[5000];
};

#endif // MSG_H
