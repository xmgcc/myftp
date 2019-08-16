#include "utils.h"
#include <string.h>
#include <stdio.h>

// get file
// get  file
int split_string2(char *in_str, char *out_str)
{
    char *first;
    char *_n;

    // 1. 从in_str查找第一个空格
    first = strstr(in_str, " ");

    // 找到\n
    _n = strstr(in_str, "\n");

    while (1) {
        // 没有找到空格
        if (NULL == first || *first == '\0') {
            return -1;
        }

        // 2. 移动到下一个字符
        first += 1;

        // 3. 判断当前字符是否空格
        if (*first != ' ') {
            // 找到
            break;
        }
    }

    // client.c\n, 不拷贝\n
    strncpy(out_str, first, _n - first);

    // 设置文件结尾'\0'
    out_str[_n-first] = '\0';

    return 0;
}

int split_string(char *in_str, char *out_str)
{
    char *first;
    char *_n;

    // 1. 从in_str查找第一个空格

    // 找到\n

    while (1) {
        // 没有找到空格

        // 2. 移动到下一个字符

        // 3. 判断当前字符是否空格
    }

    // client.c\n, 不拷贝\n

    // 设置文件结尾'\0'

    return 0;
}

long get_length(char *filename)
{
    // 打开文件
    FILE *fp = fopen(filename, "r");
    if (fp != NULL) {
        // 移动到文件末尾
        if (fseek(fp, 0, SEEK_END) < 0) {
            log_write("fseek failed");
            return -1;
        }
        // 获取文件长度
        long length = ftell(fp);

        // 关闭文件
        fclose(fp);

        return length;
    }

    return -1;
}

void get_md5(char *filename, char *md5sum)
{
    // 转换成命令 md5sum 1.txt
    char cmd[64];
    sprintf(cmd, "md5sum %s", filename);

    // popen调用md5sum获取文件的md5
    char md5[64];
    FILE * fp = popen(cmd, "r");
    if (fp != NULL) {
        int ret = fread(md5, 1, sizeof(md5), fp);
        log_write("fread ret %d, md5 %s\n", ret, md5);
        pclose(fp);
    }

    // 51f7b7b7c117f4bf69f911d4d9c87051  log.h
    // 分割md5值 51f7b7b7c117f4bf69f911d4d9c87051
    sscanf(md5, "%s", md5sum);
}
