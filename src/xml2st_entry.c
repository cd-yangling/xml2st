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
 *	xml2st_entry.c
 *
 *	Copyright (C) 2014 YangLing(yl.tienon@gmail.com)
 *
 *	Description:
 *
 *	Revision History:
 *
 *	2016-01-13 Created YangLing
 */


#include <libxml/tree.h>
#include <libxml/parser.h>
#include "xml2st_internal.h"
#include "xml2st.h"
#include <stdarg.h>

void
xml2st_set_error(
	xml2st_error_t				*	err,
	int								errcode,
	const char					*	fmt,
	...)
{
	va_list							ap;

	if(NULL == err)
		return;

	err->errcode = errcode;

	va_start(ap, fmt);
	vsnprintf(err->errmsg, sizeof(err->errmsg), fmt, ap);
	va_end(ap);
}

int
xml2st_errcode(
	xml2st_hndl						hndl)
{
	if(NULL == hndl)
		return XML2ST_E_MISUSE;

	return hndl->err.errcode;
}

const char *
xml2st_errmsg(
	xml2st_hndl						hndl)
{
	if(NULL == hndl)
		return "invalid handle";

	return hndl->err.errmsg;
}

xml2st_hndl
xml2st_easy_init(
	const struct xml2st_table	*	rtbl)
{
	struct xml2st_handle		*	hndl;

	hndl = (struct xml2st_handle*)malloc(
			sizeof(struct xml2st_handle));
	if(__builtin_expect((NULL == hndl), 0))
	{
		return NULL;
	}

	xml2st_memory_init(&(hndl->datm));
	xml2st_memory_init(&(hndl->sysm));

	hndl->rtbl	=	rtbl;
	hndl->usrp	=	NULL;
	hndl->done	=	0;
	hndl->encoding	=	NULL;

	/* 初始化错误状态 */
	hndl->err.errcode = XML2ST_E_OK;
	hndl->err.errmsg[0] = '\0';

	return hndl;
}

void
xml2st_easy_free(
	xml2st_hndl						hndl)
{
	xml2st_memory_free(&(hndl->datm));
	xml2st_memory_free(&(hndl->sysm));

	free(hndl);
}

void *
xml2st_easy_refdp(
	xml2st_hndl						hndl)
{
	return hndl->usrp;
}

void
xml2st_set_encoding(
	xml2st_hndl						hndl,
	const char					*	enc)
{
	if(NULL == hndl)
		return;

	if(hndl->done)
		return;	/* parse 已完成，忽略设置 */

	hndl->encoding	=	enc;
}

void *
xml2st_easy_parse(
	xml2st_hndl						hndl,
	xmlDocPtr						_doc)
{
	xmlNodePtr						root;
	struct xml2st_table_in		*	itbl;
	void						*	usrp;

	if(hndl->done)
		return xml2st_easy_refdp(hndl);

	hndl->done = 1;

	root = xmlDocGetRootElement(_doc);
	if(__builtin_expect((NULL == root), 0))
	{
		xml2st_set_error(&(hndl->err), XML2ST_E_EMPTY,
			"XML document has no root element");
		return NULL;
	}

	itbl = xml2st_itable_build(
				&(hndl->sysm), &(hndl->err), hndl->rtbl, hndl->encoding);
	if(__builtin_expect((NULL == itbl), 0))
	{
		return NULL;
	}

	usrp = xml2st_itable_parse(
		&(hndl->datm), &(hndl->err), itbl, root->children);

	hndl->usrp = usrp;

	return hndl->usrp;
}
