#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linklist.h"

int main(int argc, char **argv)
{
    // 定义一个头结点
    struct LinkList *h = NULL;

    // 插入命令
    linklist_insert(&h, "ls");
    linklist_insert(&h, "put 1.c");
    linklist_insert(&h, "ls");
    /* linklist_insert(&h, "cd dir"); */
    /* linklist_insert(&h, "put 1.c"); */
    /* linklist_insert(&h, "get 1.c"); */

    // 打印所有命令
    linklist_printf(h);


    printf("----------\n");
    // 获取所有命令，并打印
    char buf[1024] = {0};
    linklist_get_cmd(h, buf);
    printf("%s\n", buf);
    return 0;
}
