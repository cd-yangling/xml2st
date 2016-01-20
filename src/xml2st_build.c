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
#include "rbtree.h"
#include "list.h"
#include "xml2st_internal.h"
#include "xml2st.h"
#include "xml2st_log.h"

static
unsigned int BKDR(
	const char					*	v,
	unsigned int					c)
{
	size_t			i;
	size_t			n = strlen(v);

	for(i = 0; i < n; i++)
	{
		c = c * 131 + v[i];
	}
	
	return c;	
}

static
struct rb_root * _xml2st_hash(
	struct xml2st_table_in			*	itbl,
	const char					*	_xml)
{
	unsigned int	hash = 0;

	hash = BKDR(_xml, hash);

	return itbl->hash + (hash % NR_X2S_HASH_BUCKET);
}

static struct xml2st_column_in * _select(
	struct rb_root				*	root,
	const char					*	_xml)
{
	int							result;
	struct rb_node			*	node = root->rb_node;
	struct xml2st_column_in	*	mcol;

	while(node)
	{
		mcol = rb_entry(node, struct xml2st_column_in, node);

		result = strcmp(_xml, mcol->rcol->col_xml);
		if(result < 0)
			node = node->rb_left;
		else if(result > 0)
			node = node->rb_right;
		else
		{
			return mcol;
		}
	}

	return NULL;
}

struct xml2st_column_in *
xml2st_icolumn_lookup(
	struct xml2st_table_in			*	itbl,
	const char					*	_xml)
{
	return _select(_xml2st_hash(itbl, _xml), _xml);
}

static struct xml2st_column_in * _insert(
	struct rb_root				*	root,
	struct xml2st_column_in		*	icol)
{
	int						result;
	struct rb_node			**link, *parent=NULL;
	struct xml2st_column_in	*mcol;

	link = &(root->rb_node);
	while(*link)
	{
		parent = *link;
		mcol = rb_entry(parent, struct xml2st_column_in, node);
		result = strcmp(icol->rcol->col_xml,
							mcol->rcol->col_xml);
		if(result < 0)
			link = &((*link)->rb_left);
		else if (result > 0)
			link = &((*link)->rb_right);
		else
		{
			return NULL;
		}
	}
		
	rb_link_node(&(icol->node), parent, link);
	rb_insert_color(&(icol->node), root);
	
	return icol;
}

static struct xml2st_column_in *
xml2st_icolumn_init(
	xml2st_memory_t				*	sysm,
	const struct xml2st_column	*	rcol,
	struct xml2st_table_in			*	stbl)
{
	struct xml2st_column_in		*	icol;
	size_t							size;

	size = sizeof(struct xml2st_column_in);
	icol = (struct xml2st_column_in*)xml2st_std_alloc(sysm, size);
	if(__builtin_expect((NULL == icol), 0))
	{
		xml2st_log(XML2ST_LOG_ERR, "xml2st_std_alloc(%lu) failed", size);
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
	struct xml2st_table_in			*	itbl,
	const struct xml2st_column	*	rcol)
{
	struct xml2st_column_in		*	icol;
	struct xml2st_table_in			*	stbl = NULL;

	if(__builtin_expect(
		(xml2st_rcolumn_check(rcol)), 0))
	{
		return NULL;
	}

	if(xml2st_ptr == rcol->col_typ)
	{
		stbl =
			xml2st_itable_build(sysm, rcol->sub_tbl, itbl->encoding);
		if(__builtin_expect((NULL == stbl), 0))
		{
			return NULL;
		}
	}

	icol = xml2st_icolumn_init(sysm, rcol, stbl);
	if(__builtin_expect((NULL == icol), 0))
	{
		return NULL;
	}

	if(__builtin_expect(
		(icol != _insert(
			_xml2st_hash(
				itbl, rcol->col_xml), icol)), 0))
	{
		xml2st_log(XML2ST_LOG_ERR, 
			"%s->%s field duplicate definition",
			itbl->rtbl->tblname, rcol->col_xml);
		return NULL;
	}

	return icol;
}

static struct xml2st_table_in *
xml2st_itable_init(
	xml2st_memory_t				*	sysm,
	const struct xml2st_table	*	rtbl)
{
	struct xml2st_table_in			*	itbl;
	size_t							size;

	size = sizeof(struct xml2st_table_in);
	itbl = (struct xml2st_table_in *)xml2st_std_alloc(sysm, size);
	if(__builtin_expect((NULL == itbl), 0))
	{
		xml2st_log(XML2ST_LOG_ERR, "xml2st_std_alloc(%lu) failed", size);
		return NULL;
	}

	size = sizeof(struct rb_root) * NR_X2S_HASH_BUCKET;
	itbl->hash = (struct rb_root*)xml2st_std_alloc(sysm, size);
	if(__builtin_expect((NULL == itbl->hash), 0))
	{
		xml2st_log(XML2ST_LOG_ERR, "xml2st_std_alloc(%lu) failed", size);
		return NULL;
	}

	itbl->icol = (struct xml2st_column_in**)
						xml2st_ptr_alloc(sysm, rtbl->nr_cols, 0);
	if(__builtin_expect((NULL == itbl->icol), 0))
	{
		xml2st_log(XML2ST_LOG_ERR, 
			"xml2st_ptr_alloc(%lu) failed", rtbl->nr_cols);
		return NULL;
	}

	itbl->rtbl = rtbl;

	return itbl;
}

struct xml2st_table_in *
xml2st_itable_build(
	xml2st_memory_t				*	sysm,
	const struct xml2st_table	*	rtbl,
	const char					*	encoding)
{
	size_t							i;
	size_t							calc = 0;
	struct xml2st_table_in			*	itbl;
	const struct xml2st_column	*	rcol;

	if(__builtin_expect((NULL == rtbl->tblname), 0))
	{
		xml2st_log(XML2ST_LOG_ERR, "missing tblname defined");
		return NULL;
	}

	itbl = xml2st_itable_init(sysm, rtbl);
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
			xml2st_icolumn_build(sysm, itbl, rcol);
		if(__builtin_expect(
			(NULL == itbl->icol[i]), 0))
		{
			return NULL;
		}
	}

	if(__builtin_expect(
		(xml2st_rtable_check(rtbl, calc)), 0))
	{
		xml2st_log(XML2ST_LOG_ERR, 
			"xml2st_rtable_check(%s:%lu:%lu) failed",
			rtbl->tblname, calc, rtbl->tbl_len);
		return NULL;
	}

	return itbl;
}

