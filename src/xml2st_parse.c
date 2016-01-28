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
 *	xml2st_parse.c
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
#include <libxml/tree.h>
#include <libxml/parser.h>
#include "list.h"
#include "xml2st_internal.h"
#include "xml2st.h"

static int backtrace_icolumn(
	xml2st_memory_t				*	datm,
	xml2st_error_t				*	err,
	struct xml2st_column_in		*	icol,
	st_buffer_t						sbuf)
{
	size_t							i;
	size_t							_idx;
	st_buffer_t						slot;
	struct list_head			*	list;
	struct list_head			*	next;

	_idx = xml2st_column_index(icol);
	switch(icol->rcol->col_typ)
	{
	case xml2st_int:
	case xml2st_str:
	case xml2st_dbl:
		return xml2st_icolumn_check(err, icol, sbuf[_idx]);
	case xml2st_ptr:
		break;
	default:
		xml2st_set_error(err, XML2ST_INTERNAL,
			"unexpected field type: %d", icol->rcol->col_typ);
		return -1;
	}

	if(__builtin_expect(
		(icol->rcol->col_opt && (0 == icol->scnt)), 0))
	{
		xml2st_set_error(err, XML2ST_MISSING,
			"missing required field '%s'",
			icol->rcol->col_xml);
		return -1;
	}

	if(0 == icol->scnt)
		return 0;

	sbuf[_idx] =
			xml2st_ptr_alloc(datm, icol->scnt, 1);
	slot = (st_buffer_t)sbuf[_idx];

	if(__builtin_expect((NULL == slot), 0))
	{
		xml2st_set_error(err, XML2ST_NOMEM,
			"failed to allocate %zu elements for field '%s'",
			icol->scnt, icol->rcol->col_xml);
		return -1;
	}

	i = 0;
	list_for_each_safe(list, next, &(icol->head))
	{
		slot[i] = (char*)(list + 1);++i;
		list_del_init(list);
	}

	icol->scnt = 0;
	INIT_LIST_HEAD(&(icol->head));

	return 0;
}

static int backtrace_itable(
	xml2st_memory_t				*	datm,
	xml2st_error_t				*	err,
	struct xml2st_table_in			*	itbl,
	st_buffer_t						sbuf)
{
	size_t							i;

	for(i = 0; i < itbl->rtbl->nr_cols; i++)
	{
		if(__builtin_expect(
			backtrace_icolumn(
				datm, err, itbl->icol[i], sbuf), 0))
		{
			return -1;
		}
	}

	return 0;
}

static st_buffer_t create_st_buffer(
	xml2st_memory_t				*	datm,
	xml2st_error_t				*	err,
	struct xml2st_table_in			*	itbl)
{
	size_t							size;
	struct list_head			*	list;

	size = itbl->rtbl->tbl_len +
			sizeof(struct list_head);

	list = xml2st_std_alloc(datm, size);
	if(__builtin_expect((NULL == list), 0))
	{
		xml2st_set_error(err, XML2ST_NOMEM,
			"failed to allocate structure buffer for table '%s'",
			itbl->rtbl->tblname);
		return NULL;
	}
	return (char**)(list+1);/*	very important*/
}

static int do_leaves_node(
	xml2st_memory_t				*	datm,
	xml2st_error_t				*	err,
	struct xml2st_column_in		*	icol,
	xmlNodePtr						node,
	st_buffer_t						sbuf,
	const char					*	encoding)
{
	xmlChar					*	text;
	char					*	vptr;

	text = xmlNodeGetContent(node);
	if(NULL == text)
		return 0;

	if(0 == xmlStrlen(text))
	{
		xmlFree(text);
		return 0;
	}

	switch(icol->rcol->col_typ)
	{
	case xml2st_int:
			vptr = xml2st_write_int(datm, err, icol, (const char*)text);
			break;
	case xml2st_str:
			vptr = xml2st_write_str(datm, err, icol, (const char*)text, encoding);
			break;
	case xml2st_dbl:
			vptr = xml2st_write_dbl(datm, err, icol, (const char*)text);
			break;
	default:
		{
			xml2st_set_error(err, XML2ST_INTERNAL,
				"unexpected field type: %d", icol->rcol->col_typ);
			vptr = NULL;
		}
		break;
	}

	if(__builtin_expect((NULL == vptr), 0))
	{
		xmlFree(text);
		return -1;
	}

	if(__builtin_expect(
		(NULL != sbuf[xml2st_column_index(icol)]), 0))
	{
		xml2st_set_error(err, XML2ST_DUPLICATE,
			"duplicate field '%s' with value '%s'",
			icol->rcol->col_xml, (const char*)text);
		xmlFree(text);
		return -1;
	}

	xmlFree(text);
	sbuf[xml2st_column_index(icol)] = vptr;

	return 0;
}

static int do_branch_node(
	xml2st_memory_t				*	datm,
	xml2st_error_t				*	err,
	struct xml2st_column_in		*	icol,
	xmlNodePtr						node)
{
	struct list_head			*	list;
	struct list_head			*	head = &(icol->head);

	list = xml2st_itable_parse(
		datm, err, icol->stbl, node->children);
	if(__builtin_expect((NULL == list), 0))
	{
		return -1;
	}

	--list;	/*	very important*/

	list_add_tail(list, head);
	++icol->scnt;

	return 0;
}

static
int xml2st_icolumn_parse(
	xml2st_memory_t				*	datm,
	xml2st_error_t				*	err,
	struct xml2st_column_in		*	icol,
	xmlNodePtr						node,
	st_buffer_t						sbuf,
	const char					*	encoding)
{
	switch(icol->rcol->col_typ)
	{
	case xml2st_int:
	case xml2st_str:
	case xml2st_dbl:
		return do_leaves_node(datm, err, icol, node, sbuf, encoding);
	case xml2st_ptr:
		return do_branch_node(datm, err, icol, node);
	default:
		xml2st_set_error(err, XML2ST_INTERNAL,
			"unexpected field type: %d", icol->rcol->col_typ);
		return -1;
	}
}

void*
xml2st_itable_parse(
	xml2st_memory_t				*	datm,
	xml2st_error_t				*	err,
	struct xml2st_table_in			*	itbl,
	xmlNodePtr						lead)
{
	xmlNodePtr						node;
	struct xml2st_column_in		*	icol;
	st_buffer_t						sbuf;

	sbuf = create_st_buffer(datm, err, itbl);
	if(__builtin_expect((NULL == sbuf), 0))
	{
		return NULL;
	}

	for(node = lead; NULL != node; node = node->next)
	{
		if(XML_ELEMENT_NODE != node->type)
			continue;

		icol = xml2st_icolumn_lookup(itbl, (const char*)node->name);
		if(NULL == icol)
			continue;

		if(__builtin_expect(
			xml2st_icolumn_parse(datm, err, icol, node, sbuf, itbl->encoding), 0))
		{
			return NULL;
		}
	}

	if(__builtin_expect(
		backtrace_itable(datm, err, itbl, sbuf), 0))
	{
		return NULL;
	}

	return sbuf;
}