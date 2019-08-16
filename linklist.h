#ifndef LINKLIST_H
#define LINKLIST_H

struct LinkList {
    // 历史记录
    char cmd[64];

    // 下一个节点
    struct LinkList *next;
};

/**
 * @brief 头插法插入新节点
 *
 * @param head 头结点
 * @param cmd 命令
 */
void linklist_insert(struct LinkList **head, char *cmd);


/**
 * @brief 获取所有的命令
 *
 * @param head
 * @param buf
 */
void linklist_get_cmd(struct LinkList *head, char *out_buf);

void linklist_printf(struct LinkList *head);

#endif // LINKLIST_H
