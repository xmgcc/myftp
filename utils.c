#include "utils.h"
#include <string.h>

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
