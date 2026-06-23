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
 *	xml2st_internal.h
 *
 *	Copyright (C) 2016 YangLing(yl.tienon@gmail.com)
 *
 *	Description:
 *
 *	Revision History:
 *
 *	2016-01-13 Created YangLing
 */


#ifndef	XML2ST_INTERNAL_H
#define	XML2ST_INTERNAL_H

#include "xml2st_port.h"
#include <libxml/tree.h>
#include <libxml/parser.h>
#include "list.h"
#include "xml2st.h"

#ifdef _WIN32
#define __builtin_expect(EXP, C)  (EXP)
#endif // _WIN32


typedef struct xml2st_memory_s
{
	struct list_head					head;
	char							*	curr;
	size_t								left;
} xml2st_memory_t;

typedef struct xml2st_error_s
{
	int									errcode;
	char								errmsg[128];
} xml2st_error_t;

struct xml2st_column_in
{
	const struct xml2st_column		*	rcol;
	struct xml2st_table_in				*	stbl;
	size_t								scnt;
	struct list_head					head;
};

#define	NR_X2S_HASH_BUCKET				64
struct xml2st_table_in
{
	xml2st_memory_t					*	sysm;
	const struct xml2st_table		*	rtbl;
	struct xml2st_column_in			**	icol;
	const char						*	encoding;
};

struct xml2st_handle
{
	xml2st_memory_t						sysm;
	xml2st_memory_t						datm;
	void							*	usrp;
	const struct xml2st_table		*	rtbl;
	unsigned int						done;
	const char						*	encoding;

	/* 错误状态 */
	xml2st_error_t						err;
};

typedef	char**						st_buffer_t;

#define	xml2st_column_index(_icol_)			\
	(										\
		(_icol_)->rcol->col_off /			\
			sizeof(void*)					\
	)

#ifdef	__cplusplus
extern	"C" {
#endif

struct xml2st_column_in *
xml2st_icolumn_lookup(
	struct xml2st_table_in			*	itbl,
	const char					*	_xml);

struct xml2st_table_in *
xml2st_itable_build(
	xml2st_memory_t				*	sysm,
	xml2st_error_t				*	err,
	const struct xml2st_table	*	rtbl,
	const char					*	encoding);

void *
xml2st_itable_parse(
	xml2st_memory_t				*	datm,
	xml2st_error_t				*	err,
	struct xml2st_table_in			*	itbl,
	xmlNodePtr						lead);

/*	memory module*/
void
xml2st_memory_init(xml2st_memory_t * mem);

void
xml2st_memory_free(xml2st_memory_t * mem);

void *
xml2st_std_alloc(xml2st_memory_t * mem, size_t size);

void *
xml2st_ptr_alloc(xml2st_memory_t * mem, size_t nmemb, size_t expand);

void *
xml2st_write_int(
	xml2st_memory_t				*	datm,
	xml2st_error_t				*	err,
	struct xml2st_column_in		*	icol,
	const char					*	valp);

void *
xml2st_write_str(
	xml2st_memory_t				*	datm,
	xml2st_error_t				*	err,
	struct xml2st_column_in		*	icol,
	const char					*	valp,
	const char					*	encoding);

void *
xml2st_write_dbl(
	xml2st_memory_t				*	datm,
	xml2st_error_t				*	err,
	struct xml2st_column_in		*	icol,
	const char					*	valp);

/*	check module*/
int
xml2st_icolumn_check(
	xml2st_error_t				*	err,
	struct xml2st_column_in		*	icol,
	void						*	vptr);

int
xml2st_rcolumn_check(
	xml2st_error_t				*	err,
	const struct xml2st_column	*	rcol);

int
xml2st_rtable_check(
	xml2st_error_t				*	err,
	const struct xml2st_table	*	rtbl,
	size_t							calc);

/*
 * xml2st_set_error - 设置错误状态（内部使用）
 *
 * @err: 错误结构体指针
 * @errcode: 错误码
 * @fmt: 格式化字符串
 * @...: 可变参数
 */
void
xml2st_set_error(
	xml2st_error_t				*	err,
	int								errcode,
	const char					*	fmt,
	...);

/**
 * myiconv_convert - 通用字符编码转换
 *
 *	@ienc:		输入编码 (iconv 能识别的名字, 如 "UTF-8")
 *	@oenc:		输出编码 (iconv 能识别的名字, 如 "GB18030")
 *	@ibuf:		输入缓冲
 *	@ilen:		输入字节数
 *	@obuf:		输出缓冲
 *	@olen:		输入为输出缓冲容量, 输出为实际写入字节数
 *
 *	return
 *		==	0		成功
 *		!=	0		出错
 */
int myiconv_convert(
	const char * ienc, const char * oenc,
	const char * ibuf, unsigned int ilen,
	char * obuf, unsigned int * olen);

#ifdef __cplusplus
}
#endif
#endif	//	XML2ST_INTERNAL_H
