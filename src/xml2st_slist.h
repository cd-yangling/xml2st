/*
                            _ooOoo_
                           o8888888o
                           88" . "88
                           (| -_- |)
                            O\ = /O
                        ____/`---'\____
                      .   ' \\| |// `.
                       / \\||| : |||// \
                     / _||||| -:- |||||- \
                       | | \\\ - /// | |
                     | \_| ''\---/'' | |
                      \ .-\__ `-` ___/-. /
                   ___`. .' /--.--\ `. . __
                ."" '< `.___\_<|>_/___.' >'"".
               | | : `- \`.;`\ _ /`;.`/ - ` : | |
                 \ \ `-. \_ __\ /__ _/ .-` / /
         ======`-.____`-.___\_____/___.-`____.-'======
                            `=---='

         .............................................
                  佛祖保佑             永无BUG
          佛曰:
                  写字楼里写字间，写字间里程序员;
                  程序人员写程序，又拿程序换酒钱。
                  酒醒只在网上坐，酒醉还来网下眠;
                  醉酒醒日复日，网上网下年复年。
                  但愿老死电脑间，不愿鞠身老板前;
                  奔驰宝马贵者趣，公交自行程序员。
                  别人笑我忒疯癫，我笑自己命太贱;
                  不见满街漂亮妹，哪个归得程序员?
 */

/**
 *  xml2st_slist.h - 单向链表实现
 *
 *  Copyright (C) 2026 YangLing(yl.tienon@gmail.com)
 *  Created: 2026-06-24
 *
 *  背景：考虑到 xml2st 对链表的简单使用场景（只需尾插和头删），
 *        为消除对 Linux 内核 list.h 的 GPL 依赖而自行实现。
 *
 *  本实现基于经典单向链表算法，采用节点嵌入设计模式（将链表
 *  节点嵌入用户结构体中），不受任何版权约束。
 *
 *  参考：算法源自《数据结构与算法分析》等公开教材。
 */

#ifndef XML2ST_SLIST_H
#define XML2ST_SLIST_H

#ifdef  __cplusplus
extern "C" {
#endif

/**
 * 单向链表节点结构
 *
 * 节点可以嵌入到用户数据结构中，或放在数据的前面/后面。
 * 只有 next 指针，没有 prev 指针（节省内存）。
 */
struct slist_node
{
    struct slist_node *next;
};

/**
 * 单向链表头结构
 *
 * 包含 head 和 tail 两个指针：
 * - head: 用于删除（从头部弹出）
 * - tail: 用于插入（尾部插入 O(1)）
 */
struct slist_head
{
    struct slist_node *head;
    struct slist_node *tail;
};

/**
 * 初始化单向链表头
 *
 * @param list 链表头指针
 */
static inline void slist_init(struct slist_head *list)
{
    list->head = NULL;
    list->tail = NULL;
}

/**
 * 判断单向链表是否为空
 *
 * @param list 链表头指针
 * @return 1 表示空，0 表示非空
 */
static inline int slist_empty(const struct slist_head *list)
{
    return list->head == NULL;
}

/**
 * 在链表尾部添加节点
 *
 * 时间复杂度：O(1)
 *
 * @param node 要添加的节点
 * @param list 链表头指针
 */
static inline void slist_add_tail(struct slist_node *node,
                                   struct slist_head *list)
{
    node->next = NULL;

    if (list->tail == NULL)
    {
        /* 链表为空，head 和 tail 都指向新节点 */
        list->head = node;
        list->tail = node;
    }
    else
    {
        /* 链表不为空，添加到尾部 */
        list->tail->next = node;
        list->tail = node;
    }
}

/**
 * 从链表头部弹出节点
 *
 * 时间复杂度：O(1)
 *
 * 弹出后，节点的 next 会被置为 NULL。
 *
 * 注意：如果节点是通过 malloc 直接分配的（节点在开头），
 * 可以直接 free。但如果节点是嵌入在结构体中的（节点不在开头），
 * 不能直接 free。
 *
 * 参数：
 *   list - 链表头指针
 *
 * 返回：
 *   弹出的节点指针，如果链表为空则返回 NULL
 */
static inline struct slist_node *slist_pop_head(struct slist_head *list)
{
    if (list->head == NULL)
    {
        return NULL;
    }

    struct slist_node *node = list->head;
    list->head = node->next;

    /* 清理节点的 next，防止野指针 */
    node->next = NULL;

    /* 如果链表删空了，tail 也置空 */
    if (list->head == NULL)
    {
        list->tail = NULL;
    }

    return node;
}

#ifdef __cplusplus
}
#endif

#endif /* XML2ST_SLIST_H */