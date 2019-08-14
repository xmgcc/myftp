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

    // 无效的命令
    FTP_CMD_ERROR,
};

struct Msg {
    // 命令
    enum FTP_CMD cmd;

    // 命令行
    char args[32];

    // md5校验值
    char md5[64];

    // data
    char data[5000];
};

#endif // MSG_H
