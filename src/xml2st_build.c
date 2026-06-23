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
 *	xml2st_build.c
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
#include "xml2st.h"
#include <string.h>

/*
 * bsearch/qsort 辅助函数
 * 用于排序数组查找（替代 rbtree）
 */

/* qsort 比较函数：比较两个 icol 指针 */
static int
_compare_for_sort(
	const void					*	a,
	const void					*	b)
{
	const struct xml2st_column_in *	icol_a;
	const struct xml2st_column_in *	icol_b;

	icol_a = *(const struct xml2st_column_in **)a;
	icol_b = *(const struct xml2st_column_in **)b;

	return strcmp(icol_a->rcol->col_xml, icol_b->rcol->col_xml);
}

/* bsearch 比较函数：比较字符串 key 和 icol 指针 */
static int
_compare_for_search(
	const void					*	key,
	const void					*	element)
{
	const char					*	xml_name;
	const struct xml2st_column_in *	icol;

	xml_name = (const char *)key;
	icol = *(const struct xml2st_column_in **)element;

	return strcmp(xml_name, icol->rcol->col_xml);
}

/*
 * bsearch 查找函数（替代 hash+rbtree）
 * 直接在排序数组中查找
 */
static struct xml2st_column_in *
_lookup_bsearch(
	struct xml2st_table_in			*	itbl,
	const char					*	_xml)
{
	struct xml2st_column_in		**	result;

	/* 使用 bsearch 查找 */
	result = (struct xml2st_column_in **)bsearch(
		_xml,
		itbl->icol,
		itbl->rtbl->nr_cols,
		sizeof(struct xml2st_column_in *),
		_compare_for_search);

	return (result != NULL) ? *result : NULL;
}

struct xml2st_column_in *
xml2st_icolumn_lookup(
	struct xml2st_table_in			*	itbl,
	const char					*	_xml)
{
	/* 使用 bsearch 查找（替代 hash+rbtree） */
	return _lookup_bsearch(itbl, _xml);
}

static struct xml2st_column_in *
xml2st_icolumn_init(
	xml2st_memory_t				*	sysm,
	xml2st_error_t				*	err,
	const struct xml2st_column	*	rcol,
	struct xml2st_table_in			*	stbl)
{
	struct xml2st_column_in		*	icol;
	size_t							size;

	size = sizeof(struct xml2st_column_in);
	icol = (struct xml2st_column_in*)xml2st_std_alloc(sysm, size);
	if(__builtin_expect((NULL == icol), 0))
	{
		xml2st_set_error(err, XML2ST_E_NOMEM,
			"failed to allocate column structure for field '%s'",
			rcol->col_xml);
		return NULL;
	}

	icol->rcol = rcol;
	icol->scnt = 0;
	icol->stbl = stbl;
	INIT_LIST_HEAD(&(icol->head));

	return icol;
}

static struct xml2st_column_in *
xml2st_icolumn_build(
	xml2st_memory_t				*	sysm,
	xml2st_error_t				*	err,
	struct xml2st_table_in			*	itbl,
	const struct xml2st_column	*	rcol)
{
	struct xml2st_column_in		*	icol;
	struct xml2st_table_in			*	stbl = NULL;

	if(__builtin_expect(
		(xml2st_rcolumn_check(err, rcol)), 0))
	{
		return NULL;
	}

	if(xml2st_ptr == rcol->col_typ)
	{
		stbl =
			xml2st_itable_build(sysm, err, rcol->sub_tbl, itbl->encoding);
		if(__builtin_expect((NULL == stbl), 0))
		{
			return NULL;
		}
	}

	icol = xml2st_icolumn_init(sysm, err, rcol, stbl);
	if(__builtin_expect((NULL == icol), 0))
	{
		return NULL;
	}

	return icol;
}

static struct xml2st_table_in *
xml2st_itable_init(
	xml2st_memory_t				*	sysm,
	xml2st_error_t				*	err,
	const struct xml2st_table	*	rtbl)
{
	struct xml2st_table_in			*	itbl;
	size_t							size;

	size = sizeof(struct xml2st_table_in);
	itbl = (struct xml2st_table_in *)xml2st_std_alloc(sysm, size);
	if(__builtin_expect((NULL == itbl), 0))
	{
		xml2st_set_error(err, XML2ST_E_NOMEM,
			"failed to allocate table structure for '%s'",
			rtbl->tblname);
		return NULL;
	}

	itbl->icol = (struct xml2st_column_in**)
						xml2st_ptr_alloc(sysm, rtbl->nr_cols, 0);
	if(__builtin_expect((NULL == itbl->icol), 0))
	{
		xml2st_set_error(err, XML2ST_E_NOMEM,
			"failed to allocate column array for '%s'",
			rtbl->tblname);
		return NULL;
	}

	itbl->rtbl = rtbl;

	return itbl;
}

struct xml2st_table_in *
xml2st_itable_build(
	xml2st_memory_t				*	sysm,
	xml2st_error_t				*	err,
	const struct xml2st_table	*	rtbl,
	const char					*	encoding)
{
	size_t							i;
	size_t							calc = 0;
	struct xml2st_table_in			*	itbl;
	const struct xml2st_column	*	rcol;

	if(__builtin_expect((NULL == rtbl->tblname), 0))
	{
		xml2st_set_error(err, XML2ST_E_MISUSE,
			"table name is not defined");
		return NULL;
	}

	itbl = xml2st_itable_init(sysm, err, rtbl);
	if(__builtin_expect((NULL == itbl), 0))
	{
		return NULL;
	}

	itbl->encoding = (encoding != NULL) ? encoding : XML2ST_DEFAULT_ENCODING;

	for(i = 0; i < rtbl->nr_cols; i++)
	{
		rcol = rtbl->tbl_col + i;

		calc += rcol->col_len;

		itbl->icol[i] =
			xml2st_icolumn_build(sysm, err, itbl, rcol);
		if(__builtin_expect(
			(NULL == itbl->icol[i]), 0))
		{
			return NULL;
		}
	}

	if(__builtin_expect(
		(xml2st_rtable_check(err, rtbl, calc)), 0))
	{
		return NULL;
	}

	/*
	 * 对 icol 数组排序（用于 bsearch 查找）
	 * 替代 hash+rbtree 方案
	 */
	qsort(itbl->icol, rtbl->nr_cols,
		sizeof(struct xml2st_column_in *),
		_compare_for_sort);

	/*
	 * 检查重复元素（排序后更容易检测）
	 * 遍历一次，检查相邻元素是否相同
	 */
	for(i = 1; i < rtbl->nr_cols; i++)
	{
		if(strcmp(
			itbl->icol[i-1]->rcol->col_xml,
			itbl->icol[i]->rcol->col_xml) == 0)
		{
			xml2st_set_error(err, XML2ST_E_DUPLICATE,
				"duplicate definition of field '%s' in table '%s'",
				itbl->icol[i]->rcol->col_xml, itbl->rtbl->tblname);
			return NULL;
		}
	}

	return itbl;
}