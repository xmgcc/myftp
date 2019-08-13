#ifndef LOG_H
#define LOG_H

//创建,前缀
void log_create(const char *filename);
//销毁
void log_destroy();
//写入
void log_write(const char *format, ...);
// 参数可变
// printf("read error, ret %d, %s", ret, msg);
// printf("read error, ret %d", ret);
// printf("read error");

#endif // LOG_H
