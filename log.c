#include "log.h"
#include <stdio.h>
#include <stdarg.h>

FILE *g_log = NULL;

//创建,前缀
void log_create(const char *filename)
{
    g_log = fopen(filename, "a+");
    if (NULL == g_log) {
        printf("fopen %s failed\n", filename);
    }
}

//销毁
void log_destroy()
{
    fclose(g_log);
    // 不想变为野指针
    g_log = NULL;
}

//写入
void log_write(const char *format, ...)
{
    // 1 定义va_list变量
    va_list args;

    // 2 创建
    va_start(args, format);

    vfprintf(g_log, format, args);

    // 3 销毁
    va_end(args);

    // 强制写入文件
    fflush(g_log);
}
