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
                  写字楼里写字间，写字间里程序员； 
                  程序人员写程序，又拿程序换酒钱。 
                  酒醒只在网上坐，酒醉还来网下眠； 
                  酒醉酒醒日复日，网上网下年复年。 
                  但愿老死电脑间，不愿鞠躬老板前； 
                  奔驰宝马贵者趣，公交自行程序员。 
                  别人笑我忒疯癫，我笑自己命太贱； 
                  不见满街漂亮妹，哪个归得程序员？  
 */
/**
 *	xml2st_mmblk.c
 *
 *	Copyright (C) 2014 YangLing(yl.tienon@gmail.com)
 *
 *	Description:
 *
 *	Revision History:
 *
 *	2016-01-13 Created YangLing
 */


#include "xml2st_port.h"
#include "list.h"
#include "xml2st_internal.h"

void xml2st_memory_init(xml2st_memory_t * mem)
{
	mem->curr = NULL;
	mem->left = 0;
	INIT_LIST_HEAD(&(mem->head));
}

void xml2st_memory_free(xml2st_memory_t * mem)
{
	struct list_head * list;
	struct list_head * safe;
	struct list_head * head = &(mem->head);

	list_for_each_safe(list, safe, head)
	{
		list_del_init(list);
		free(list);
	}

	if(NULL != mem->curr)
		free(mem->curr);
}

#define	X2S_MEMORY_BLOCK_SIZE	((size_t)(64*1024))

static void xml2st_new_block(xml2st_memory_t * mem)
{
	struct list_head * list;

	if(NULL != mem->curr)
	{
		list = (struct list_head*)mem->curr;
		list_add_tail(list, &(mem->head));
		mem->curr = NULL;
	}

	mem->curr = (char*)malloc(X2S_MEMORY_BLOCK_SIZE);
	if(NULL != mem->curr)
	{
		memset(mem->curr, 0, X2S_MEMORY_BLOCK_SIZE);
		mem->left = X2S_MEMORY_BLOCK_SIZE -
							sizeof(struct list_head);
	}
}

static int xml2st_ll_prepare(xml2st_memory_t * mem, size_t size)
{
	if(NULL == mem->curr)
		xml2st_new_block(mem);
	
	if(NULL == mem->curr)
		return -1;

	if(size > mem->left)
		xml2st_new_block(mem);

	if(size > mem->left)
		return -1;

	/*	safe check never happy!!!*/
	if(X2S_MEMORY_BLOCK_SIZE < mem->left)
		return -1;

	return 0;
}

static void * xml2st_ll_malloc(xml2st_memory_t * mem, size_t size)
{
	size_t		pos;

	if(0 == xml2st_ll_prepare(mem, size))
	{
		pos			= 
			X2S_MEMORY_BLOCK_SIZE - mem->left;
		mem->left	= mem->left - size;
		
		return mem->curr + pos;
	}
	else
		return NULL;
}

void * xml2st_std_alloc(xml2st_memory_t * mem, size_t size)
{
	return xml2st_ll_malloc(mem, size);
}

void * xml2st_ptr_alloc(xml2st_memory_t * mem, size_t nmemb, size_t expand)
{
	size_t	size;

	nmemb += expand;

	size = nmemb * sizeof(void*);

	return xml2st_ll_malloc(mem, size);
}
