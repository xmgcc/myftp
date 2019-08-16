#include "linklist.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void linklist_insert(struct LinkList **head, char *cmd)
{
    // 申请新节点，并且初始化
    struct LinkList *node = (struct LinkList *)malloc(sizeof(struct LinkList));
    node->next = NULL;
    strcpy(node->cmd, cmd);

    // 新节点指向头结点
    node->next = *head;

    // 头结点指针指向新节点
    *head = node;
}


void linklist_printf(struct LinkList *head)
{
    struct LinkList *p = head;

    while (p != NULL) {
        printf("%s\n", p->cmd);
        p = p->next;
    }
}

void linklist_get_cmd(struct LinkList *head, char *out_buf)
{
    struct LinkList *p = head;

    char buf[32];

    while (p != NULL) {
        // 在命令后加\n
        sprintf(buf, "%s\n", p->cmd);

        // 拷贝命令
        strcat(out_buf, buf);

        // 指向下一个节点
        p = p->next;
    }
}
