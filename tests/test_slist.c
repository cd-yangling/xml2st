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
 */

/**
 *  test_slist.c - 单向链表测试套件
 *
 *  Copyright (C) 2026 YangLing(yl.tienon@gmail.com)
 *  Created: 2026-06-24
 *
 *  测试单向链表的各项功能和边界情况。
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>
#include "xml2st_slist.h"

/* 测试计数器 */
static int tests_passed = 0;
static int tests_total = 0;

/* 测试宏 */
#define TEST_START(name) \
    do { \
        printf("Test %d: %s\n", tests_total + 1, name); \
        tests_total++; \
    } while (0)

#define TEST_PASS() \
    do { \
        printf("  [PASSED]\n"); \
        tests_passed++; \
    } while (0)

#define TEST_FAIL(msg) \
    do { \
        printf("  [FAILED]: %s\n", msg); \
    } while (0)

#define ASSERT_EQ(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            TEST_FAIL("expected != actual"); \
            return; \
        } \
    } while (0)

#define ASSERT_NE(expected, actual) \
    do { \
        if ((expected) == (actual)) { \
            TEST_FAIL("expected == actual (should be different)"); \
            return; \
        } \
    } while (0)

#define ASSERT_NULL(ptr) \
    do { \
        if ((ptr) != NULL) { \
            TEST_FAIL("pointer is not NULL"); \
            return; \
        } \
    } while (0)

#define ASSERT_NOT_NULL(ptr) \
    do { \
        if ((ptr) == NULL) { \
            TEST_FAIL("pointer is NULL"); \
            return; \
        } \
    } while (0)

/* ============================================================
 * Test 1: 初始化测试
 * ============================================================ */
static void test_init(void)
{
    TEST_START("初始化测试");

    struct slist_head list;
    slist_init(&list);

    ASSERT_NULL(list.head);
    ASSERT_NULL(list.tail);
    ASSERT_EQ(1, slist_empty(&list));

    TEST_PASS();
}

/* ============================================================
 * Test 2: 单节点测试
 * ============================================================ */
static void test_single_node(void)
{
    TEST_START("单节点测试");

    struct slist_head list;
    struct slist_node node;

    slist_init(&list);

    /* 插入一个节点 */
    slist_add_tail(&node, &list);

    ASSERT_NOT_NULL(list.head);
    ASSERT_NOT_NULL(list.tail);
    ASSERT_EQ(list.head, list.tail);  /* head 和 tail 指向同一个节点 */
    ASSERT_EQ(&node, list.head);
    ASSERT_NULL(node.next);
    ASSERT_EQ(0, slist_empty(&list));

    /* 弹出一个节点 */
    struct slist_node *pop_node = slist_pop_head(&list);

    ASSERT_EQ(&node, pop_node);
    ASSERT_NULL(pop_node->next);  /* 弹出后 next 应该是 NULL */
    ASSERT_NULL(list.head);
    ASSERT_NULL(list.tail);
    ASSERT_EQ(1, slist_empty(&list));

    /* 再次弹出应该返回 NULL */
    struct slist_node *pop_null = slist_pop_head(&list);
    ASSERT_NULL(pop_null);

    TEST_PASS();
}

/* ============================================================
 * Test 3: 多节点测试（顺序验证）
 * ============================================================ */
static void test_multiple_nodes(void)
{
    TEST_START("多节点测试（顺序验证）");

    struct slist_head list;
    struct slist_node nodes[3];

    slist_init(&list);

    /* 插入 3 个节点：A, B, C */
    slist_add_tail(&nodes[0], &list);  /* A */
    slist_add_tail(&nodes[1], &list);  /* B */
    slist_add_tail(&nodes[2], &list);  /* C */

    ASSERT_EQ(&nodes[0], list.head);  /* head 指向 A */
    ASSERT_EQ(&nodes[2], list.tail);  /* tail 指向 C */
    ASSERT_EQ(&nodes[1], nodes[0].next);  /* A->next = B */
    ASSERT_EQ(&nodes[2], nodes[1].next);  /* B->next = C */
    ASSERT_NULL(nodes[2].next);  /* C->next = NULL */
    ASSERT_EQ(0, slist_empty(&list));

    /* 弹出并验证顺序：A, B, C */
    struct slist_node *pop1 = slist_pop_head(&list);
    ASSERT_EQ(&nodes[0], pop1);  /* 第1次弹出 A */
    ASSERT_NULL(pop1->next);  /* 弹出后 next=NULL */

    ASSERT_EQ(&nodes[1], list.head);  /* head 指向 B */
    ASSERT_EQ(&nodes[2], list.tail);  /* tail 指向 C */

    struct slist_node *pop2 = slist_pop_head(&list);
    ASSERT_EQ(&nodes[1], pop2);  /* 第2次弹出 B */
    ASSERT_NULL(pop2->next);  /* 弹出后 next=NULL */

    ASSERT_EQ(&nodes[2], list.head);  /* head 指向 C */
    ASSERT_EQ(&nodes[2], list.tail);  /* tail 指向 C（最后一个节点）*/

    struct slist_node *pop3 = slist_pop_head(&list);
    ASSERT_EQ(&nodes[2], pop3);  /* 第3次弹出 C */
    ASSERT_NULL(pop3->next);  /* 弹出后 next=NULL */

    ASSERT_NULL(list.head);
    ASSERT_NULL(list.tail);
    ASSERT_EQ(1, slist_empty(&list));

    TEST_PASS();
}

/* ============================================================
 * Test 4: 连续操作测试
 * ============================================================ */
static void test_continuous_operations(void)
{
    TEST_START("连续操作测试");

    struct slist_head list;
    struct slist_node nodes[10];

    slist_init(&list);

    /* 插入 10 个节点 */
    for (int i = 0; i < 10; i++)
    {
        slist_add_tail(&nodes[i], &list);
    }

    ASSERT_EQ(0, slist_empty(&list));
    ASSERT_EQ(&nodes[0], list.head);
    ASSERT_EQ(&nodes[9], list.tail);

    /* 弹出 5 个节点 */
    for (int i = 0; i < 5; i++)
    {
        struct slist_node *pop = slist_pop_head(&list);
        ASSERT_EQ(&nodes[i], pop);
        ASSERT_NULL(pop->next);
    }

    ASSERT_EQ(0, slist_empty(&list));
    ASSERT_EQ(&nodes[5], list.head);
    ASSERT_EQ(&nodes[9], list.tail);

    /* 再插入 3 个节点 */
    struct slist_node extra_nodes[3];
    for (int i = 0; i < 3; i++)
    {
        slist_add_tail(&extra_nodes[i], &list);
    }

    ASSERT_EQ(&nodes[5], list.head);
    ASSERT_EQ(&extra_nodes[2], list.tail);
    ASSERT_EQ(&nodes[6], nodes[5].next);  /* nodes[5]->next = nodes[6] */
    ASSERT_EQ(&nodes[7], nodes[6].next);  /* nodes[6]->next = nodes[7] */
    ASSERT_EQ(&nodes[8], nodes[7].next);  /* nodes[7]->next = nodes[8] */
    ASSERT_EQ(&nodes[9], nodes[8].next);  /* nodes[8]->next = nodes[9] */
    ASSERT_EQ(&extra_nodes[0], nodes[9].next);  /* nodes[9]->next = extra_nodes[0] */
    ASSERT_EQ(&extra_nodes[1], extra_nodes[0].next);  /* extra[0]->next = extra[1] */
    ASSERT_EQ(&extra_nodes[2], extra_nodes[1].next);  /* extra[1]->next = extra[2] */
    ASSERT_NULL(extra_nodes[2].next);  /* extra[2]->next = NULL */

    /* 弹出剩余节点并验证顺序 */
    int node_idx = 0;
    while (!slist_empty(&list))
    {
        struct slist_node *pop = slist_pop_head(&list);

        if (node_idx < 5)
        {
            ASSERT_EQ(&nodes[5 + node_idx], pop);
        }
        else
        {
            ASSERT_EQ(&extra_nodes[node_idx - 5], pop);
        }

        ASSERT_NULL(pop->next);
        node_idx++;
    }

    ASSERT_EQ(8, node_idx);  /* 总共弹出 8 个节点 */
    ASSERT_NULL(list.head);
    ASSERT_NULL(list.tail);

    TEST_PASS();
}

/* ============================================================
 * Test 5: 空链表操作
 * ============================================================ */
static void test_empty_list_operations(void)
{
    TEST_START("空链表操作");

    struct slist_head list;
    slist_init(&list);

    /* 对空链表弹出 */
    struct slist_node *pop = slist_pop_head(&list);
    ASSERT_NULL(pop);

    /* 对空链表判断空 */
    ASSERT_EQ(1, slist_empty(&list));

    /* 多次弹出空链表 */
    for (int i = 0; i < 10; i++)
    {
        pop = slist_pop_head(&list);
        ASSERT_NULL(pop);
    }

    ASSERT_NULL(list.head);
    ASSERT_NULL(list.tail);

    TEST_PASS();
}

/* ============================================================
 * Test 6: 连续插入边界（压力测试）
 * ============================================================ */
static void test_continuous_insert(void)
{
    TEST_START("连续插入边界（压力测试）");

    struct slist_head list;
    struct slist_node *nodes = malloc(100 * sizeof(struct slist_node));

    if (nodes == NULL)
    {
        TEST_FAIL("malloc failed");
        return;
    }

    slist_init(&list);

    /* 连续插入 100 个节点 */
    for (int i = 0; i < 100; i++)
    {
        slist_add_tail(&nodes[i], &list);

        /* 验证 tail 指针始终正确 */
        ASSERT_EQ(&nodes[i], list.tail);
    }

    ASSERT_EQ(&nodes[0], list.head);
    ASSERT_EQ(&nodes[99], list.tail);
    ASSERT_EQ(0, slist_empty(&list));

    /* 验证链表结构正确 */
    for (int i = 0; i < 99; i++)
    {
        ASSERT_EQ(&nodes[i+1], nodes[i].next);
    }
    ASSERT_NULL(nodes[99].next);

    free(nodes);

    TEST_PASS();
}

/* ============================================================
 * Test 7: 连续弹出边界
 * ============================================================ */
static void test_continuous_pop(void)
{
    TEST_START("连续弹出边界");

    struct slist_head list;
    struct slist_node *nodes = malloc(100 * sizeof(struct slist_node));

    if (nodes == NULL)
    {
        TEST_FAIL("malloc failed");
        return;
    }

    slist_init(&list);

    /* 插入 100 个节点 */
    for (int i = 0; i < 100; i++)
    {
        slist_add_tail(&nodes[i], &list);
    }

    /* 连续弹出 100 次 */
    for (int i = 0; i < 100; i++)
    {
        struct slist_node *pop = slist_pop_head(&list);
        ASSERT_EQ(&nodes[i], pop);
        ASSERT_NULL(pop->next);

        /* 验证指针状态 */
        if (i < 99)
        {
            ASSERT_EQ(&nodes[i+1], list.head);
        }
        else
        {
            ASSERT_NULL(list.head);
            ASSERT_NULL(list.tail);
        }
    }

    ASSERT_EQ(1, slist_empty(&list));

    free(nodes);

    TEST_PASS();
}

/* ============================================================
 * Test 8: 插入-弹出交替
 * ============================================================ */
static void test_alternating_operations(void)
{
    TEST_START("插入-弹出交替");

    struct slist_head list;
    struct slist_node nodes1[3];
    struct slist_node nodes2[5];

    slist_init(&list);

    /* 插入 3 个，弹出 3 个（清空）*/
    for (int i = 0; i < 3; i++)
    {
        slist_add_tail(&nodes1[i], &list);
    }

    for (int i = 0; i < 3; i++)
    {
        struct slist_node *pop = slist_pop_head(&list);
        ASSERT_EQ(&nodes1[i], pop);
        ASSERT_NULL(pop->next);
    }

    ASSERT_NULL(list.head);
    ASSERT_NULL(list.tail);
    ASSERT_EQ(1, slist_empty(&list));

    /* 再插入 5 个，弹出 5 个（清空）*/
    for (int i = 0; i < 5; i++)
    {
        slist_add_tail(&nodes2[i], &list);
    }

    for (int i = 0; i < 5; i++)
    {
        struct slist_node *pop = slist_pop_head(&list);
        ASSERT_EQ(&nodes2[i], pop);
        ASSERT_NULL(pop->next);
    }

    ASSERT_NULL(list.head);
    ASSERT_NULL(list.tail);
    ASSERT_EQ(1, slist_empty(&list));

    TEST_PASS();
}

/* ============================================================
 * Test 9: 弹出后节点状态验证
 * ============================================================ */
static void test_pop_node_state(void)
{
    TEST_START("弹出后节点状态验证");

    struct slist_head list;
    struct slist_node nodes[3];

    slist_init(&list);

    /* 插入 A, B, C */
    slist_add_tail(&nodes[0], &list);
    slist_add_tail(&nodes[1], &list);
    slist_add_tail(&nodes[2], &list);

    /* 弹出 A */
    struct slist_node *pop1 = slist_pop_head(&list);
    ASSERT_EQ(&nodes[0], pop1);
    ASSERT_NULL(pop1->next);  /* A 的 next 应该被清零 */

    /* 验证 B 和 C 还在链表中，结构正确 */
    ASSERT_EQ(&nodes[1], list.head);
    ASSERT_EQ(&nodes[2], list.tail);
    ASSERT_EQ(&nodes[2], nodes[1].next);  /* B->next 还指向 C */
    ASSERT_NULL(nodes[2].next);  /* C->next 还是 NULL */

    /* 弹出 B */
    struct slist_node *pop2 = slist_pop_head(&list);
    ASSERT_EQ(&nodes[1], pop2);
    ASSERT_NULL(pop2->next);  /* B 的 next 应该被清零 */

    /* 验证 C 还在链表中 */
    ASSERT_EQ(&nodes[2], list.head);
    ASSERT_EQ(&nodes[2], list.tail);
    ASSERT_NULL(nodes[2].next);

    /* 弹出 C */
    struct slist_node *pop3 = slist_pop_head(&list);
    ASSERT_EQ(&nodes[2], pop3);
    ASSERT_NULL(pop3->next);  /* C 的 next 应该被清零 */

    ASSERT_NULL(list.head);
    ASSERT_NULL(list.tail);

    TEST_PASS();
}

/* ============================================================
 * Test 10: 节点后面是数据（node+1 用法）
 * ============================================================ */
static void test_node_plus_one(void)
{
    TEST_START("节点后面是数据（node+1 用法）");

    struct slist_head list;

    /* 模拟内存块结构：节点在前面，数据在后面 */
    struct {
        struct slist_node node;
        int data;
    } blocks[3];

    slist_init(&list);

    /* 初始化数据 */
    blocks[0].data = 100;
    blocks[1].data = 200;
    blocks[2].data = 300;

    /* 插入节点 */
    slist_add_tail(&blocks[0].node, &list);
    slist_add_tail(&blocks[1].node, &list);
    slist_add_tail(&blocks[2].node, &list);

    /* 验证可以通过 node+1 访问数据 */
    int expected_data[] = {100, 200, 300};
    int idx = 0;

    while (!slist_empty(&list))
    {
        struct slist_node *pop = slist_pop_head(&list);

        /* 通过 node+1 访问数据 */
        int *data_ptr = (int *)(pop + 1);
        ASSERT_EQ(expected_data[idx], *data_ptr);

        idx++;
    }

    ASSERT_EQ(3, idx);

    TEST_PASS();
}

/* ============================================================
 * Test 11: 节点前面是数据（offsetof 推算）
 * ============================================================ */
static void test_node_before_data(void)
{
    TEST_START("节点前面是数据（offsetof 推算）");

    struct slist_head list;

    /* 模拟数据在前面，节点在后面 */
    struct test_data_v2 {
        int value;               /* 开头 */
        struct slist_node node;  /* 后面 */
    } blocks[3];

    slist_init(&list);

    /* 初始化数据 */
    blocks[0].value = 100;
    blocks[1].value = 200;
    blocks[2].value = 300;

    /* 插入节点 */
    slist_add_tail(&blocks[0].node, &list);
    slist_add_tail(&blocks[1].node, &list);
    slist_add_tail(&blocks[2].node, &list);

    /* 验证可以通过 offsetof 推算访问数据 */
    int expected_values[] = {100, 200, 300};
    int idx = 0;

    while (!slist_empty(&list))
    {
        struct slist_node *pop = slist_pop_head(&list);

        /* 通过 offsetof 推算结构体地址 */
        struct test_data_v2 *data = (struct test_data_v2 *)((char *)pop - offsetof(struct test_data_v2, node));
        ASSERT_EQ(expected_values[idx], data->value);

        idx++;
    }

    ASSERT_EQ(3, idx);

    TEST_PASS();
}

/* ============================================================
 * Test 12: 不同偏移位置测试
 * ============================================================ */
static void test_different_offsets(void)
{
    TEST_START("不同偏移位置测试");

    struct slist_head list;

    /* 测试节点在不同位置 */
    struct test_v1 {
        struct slist_node node;  /* 开头 */
        int value;
    } v1 = {.value = 10};

    struct test_v2 {
        int value;               /* 开头 */
        struct slist_node node;  /* 中间 */
    } v2 = {.value = 20};

    struct test_v3 {
        int value;
        char buffer[100];
        struct slist_node node;  /* 末尾 */
    } v3 = {.value = 30};

    slist_init(&list);

    slist_add_tail(&v1.node, &list);
    slist_add_tail(&v2.node, &list);
    slist_add_tail(&v3.node, &list);

    /* 弹出并验证可以通过 node 找到正确的 value */
    struct slist_node *pop1 = slist_pop_head(&list);
    struct test_v1 *p1 = (struct test_v1 *)((char *)pop1 - offsetof(struct test_v1, node));
    ASSERT_EQ(10, p1->value);

    struct slist_node *pop2 = slist_pop_head(&list);
    struct test_v2 *p2 = (struct test_v2 *)((char *)pop2 - offsetof(struct test_v2, node));
    ASSERT_EQ(20, p2->value);

    struct slist_node *pop3 = slist_pop_head(&list);
    struct test_v3 *p3 = (struct test_v3 *)((char *)pop3 - offsetof(struct test_v3, node));
    ASSERT_EQ(30, p3->value);

    ASSERT_NULL(list.head);
    ASSERT_NULL(list.tail);

    TEST_PASS();
}

/* ============================================================
 * 主函数：运行所有测试
 * ============================================================ */
int main(void)
{
    printf("\n");
    printf("====================================\n");
    printf("  单向链表测试套件\n");
    printf("====================================\n");
    printf("\n");

    /* 运行所有测试 */
    test_init();
    test_single_node();
    test_multiple_nodes();
    test_continuous_operations();
    test_empty_list_operations();
    test_continuous_insert();
    test_continuous_pop();
    test_alternating_operations();
    test_pop_node_state();
    test_node_plus_one();
    test_node_before_data();
    test_different_offsets();

    /* 输出测试结果 */
    printf("\n");
    printf("====================================\n");
    printf("  测试结果：\n");
    printf("  总计：%d 个测试\n", tests_total);
    printf("  通过：%d 个测试\n", tests_passed);
    printf("  失败：%d 个测试\n", tests_total - tests_passed);
    printf("====================================\n");

    if (tests_passed == tests_total)
    {
        printf("\n");
        printf("  All tests passed!\n");
        printf("\n");
        return 0;
    }
    else
    {
        printf("\n");
        printf("  Some tests failed!\n");
        printf("\n");
        return 1;
    }
}