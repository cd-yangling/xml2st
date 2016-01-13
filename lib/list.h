/*
 * 本文件源自 Linux 内核 include/linux/list.h, 经改编用于 xml2st.
 * 主要改动: 头文件守卫改为 XML2ST_LIST_H; INLINE_FN 改用 _MSC_VER 判断;
 *           变量名用 new1 兼容 C++. 链表逻辑保持内核原版.
 * 修改日期: 2016-01-13
 */

#ifndef XML2ST_LIST_H
#define XML2ST_LIST_H

#ifdef  __cplusplus
extern  "C" {
#endif

#ifndef INLINE_FN
#ifdef _MSC_VER
	#define		INLINE_FN		__forceinline
#else
	#define		INLINE_FN		__inline__
#endif
#endif

/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */

struct list_head
{
	struct list_head *next;
	struct list_head *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define INIT_LIST_HEAD(ptr) do { \
	(ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)

/*
 * Insert a new1 entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 *
 * 注: 变量名使用 new1 而非内核原版 new, 以兼容 C++ (new 为关键字).
 */
static INLINE_FN void __list_add(struct list_head * new1,
	struct list_head * prev,
	struct list_head * next)
{
	next->prev = new1;
	new1->next = next;
	new1->prev = prev;
	prev->next = new1;
}

/**
 * list_add - add a new entry
 * @new1: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new1 entry after the specified head.
 * This is good for implementing stacks.
 */
static INLINE_FN void list_add(struct list_head *new1, struct list_head *head)
{
	__list_add(new1, head, head->next);
}

/**
 * list_add_tail - add a new entry
 * @new1: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new1 entry before the specified head.
 * This is useful for implementing queues.
 */
static INLINE_FN void list_add_tail(struct list_head *new1, struct list_head *head)
{
	__list_add(new1, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static INLINE_FN void __list_del(struct list_head * prev,
				  struct list_head * next)
{
	next->prev = prev;
	prev->next = next;
}

/**
 * list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
static INLINE_FN void list_del_init(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	INIT_LIST_HEAD(entry);
}

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static INLINE_FN int list_empty(struct list_head *head)
{
	return head->next == head;
}

static INLINE_FN void __list_splice(const struct list_head *list,
				 struct list_head *prev,
				 struct list_head *next)
{
	struct list_head *first = list->next;
	struct list_head *last = list->prev;

	first->prev = prev;
	prev->next = first;

	last->next = next;
	next->prev = last;
}

/**
 * list_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialised
 */
static INLINE_FN void list_splice_init(struct list_head *list,
				struct list_head *head)
{
	if (!list_empty(list)) {
		__list_splice(list, head, head->next);
		INIT_LIST_HEAD(list);
	}
}

/**
 * list_splice_tail_init - join two lists and reinitialise the emptied list
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * Each of the lists is a queue.
 * The list at @list is reinitialised
 */
static INLINE_FN void list_splice_tail_init(struct list_head *list,
					 struct list_head *head)
{
	if (!list_empty(list)) {
		__list_splice(list, head->prev, head);
		INIT_LIST_HEAD(list);
	}
}

/**
 * list_entry - get the struct for this entry
 * @ptr:	the &struct list_head pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 */
#define list_entry(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

/**
 * list_for_each - iterate over a list
 * @pos:	the &struct list_head to use as a loop counter.
 * @head:	the head for your list.
 */
#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); \
		pos = pos->next)

/**
 * list_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop counter.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = n->next)

#ifdef __cplusplus
}
#endif
#endif	/* XML2ST_LIST_H */
