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
 *	xml2st.h
 *
 *	Copyright (C) 2016 YangLing(yl.tienon@gmail.com)
 *
 *	Description:
 *
 *	Revision History:
 *
 *	2016-01-13 Created YangLing
 */

#ifndef	XML2ST_H
#define	XML2ST_H

#include <stddef.h>
#include <libxml/tree.h>
#include <libxml/parser.h>

#define xml2st_int	0
#define	xml2st_str	1
#define	xml2st_dbl	2
#define	xml2st_ptr	3

#define	stsizeof(t,m)	sizeof(((t*)0)->m)

/*
 * xml2st 默认目标编码 (未调用 xml2st_set_encoding 时生效):
 * Windows 下用 GB18030(CP936), 其他平台(如 Linux)用 UTF-8.
 */
#ifdef _WIN32
#define	XML2ST_DEFAULT_ENCODING	"GB18030"
#else
#define	XML2ST_DEFAULT_ENCODING	"UTF-8"
#endif

#ifdef	WIN32
//int
#define	XML2ST_DEF_MINT(				\
	col_xml,t, m)					\
	{								\
		1,							\
		xml2st_int,					\
		(col_xml),(0),				\
		stsizeof(t,m),				\
		offsetof(t,m), NULL,		\
		NULL, NULL, NULL			\
	}

#define	XML2ST_DEF_OINT(				\
	col_xml,t, m)					\
	{								\
		0,							\
		xml2st_int,					\
		(col_xml),(0),				\
		stsizeof(t,m),				\
		offsetof(t,m), NULL,		\
		NULL, NULL, NULL			\
	}
//str
#define	XML2ST_DEF_MSTR(				\
	col_xml,l,t, m)					\
	{								\
		1,							\
		xml2st_str,					\
		(col_xml),(l),				\
		stsizeof(t,m),				\
		offsetof(t,m), NULL,		\
		NULL, NULL, NULL			\
	}

#define	XML2ST_DEF_OSTR(				\
	col_xml,l,t, m)					\
	{								\
		0,							\
		xml2st_str,					\
		(col_xml),(l),				\
		stsizeof(t,m),				\
		offsetof(t,m), NULL,		\
		NULL, NULL, NULL			\
	}
//dbl
#define	XML2ST_DEF_MDBL(				\
	col_xml,t, m)					\
	{								\
		1,							\
		xml2st_dbl,					\
		(col_xml),(0),				\
		stsizeof(t,m),				\
		offsetof(t,m), NULL,		\
		NULL, NULL, NULL			\
	}

#define	XML2ST_DEF_ODBL(				\
	col_xml,t, m)					\
	{								\
		0,							\
		xml2st_dbl,					\
		(col_xml),(0),				\
		stsizeof(t,m),				\
		offsetof(t,m), NULL,		\
		NULL, NULL, NULL			\
	}
//ptr
#define	XML2ST_DEF_MPTR(				\
	col_xml, t, m, sub)				\
	{								\
		1, xml2st_ptr, (col_xml),0,	\
		stsizeof(t,m),				\
		offsetof(t,m),				\
		NULL, NULL, NULL,			\
		(&(sub)),					\
	}

#define	XML2ST_DEF_OPTR(				\
	col_xml, t, m, sub)				\
	{								\
		0, xml2st_ptr, (col_xml),0,	\
		stsizeof(t,m),				\
		offsetof(t,m),				\
		NULL, NULL, NULL,			\
		(&(sub)),					\
	}

#define	XML2ST_DEF_MTBL(n,t,c)			\
	{								\
		(n),						\
		sizeof(t),					\
		sizeof(c)/sizeof(c[0]),		\
		(c)							\
	}
#else
//int
#define	XML2ST_DEF_MINT(				\
	xml,t, m)						\
	{								\
		col_opt : 1,				\
		col_typ : xml2st_int,		\
		col_xml : (xml),			\
		col_ltd : (0),				\
		col_len : stsizeof(t,m),	\
		col_off : offsetof(t,m),	\
		sub_tbl : NULL,				\
	}

#define	XML2ST_DEF_OINT(				\
	xml,t, m)						\
	{								\
		col_opt : 0,				\
		col_typ : xml2st_int,		\
		col_xml : (xml),			\
		col_ltd : (0),				\
		col_len : stsizeof(t,m),	\
		col_off : offsetof(t,m), 	\
		sub_tbl : NULL,				\
	}
//str
#define	XML2ST_DEF_MSTR(				\
	xml,l,t, m)						\
	{								\
		col_opt : 1,				\
		col_typ : xml2st_str,		\
		col_xml : (xml),			\
		col_ltd : (l),				\
		col_len : stsizeof(t,m),	\
		col_off : offsetof(t,m),	\
		sub_tbl : NULL,				\
	}

#define	XML2ST_DEF_OSTR(				\
	xml,l,t, m)						\
	{								\
		col_opt : 0,				\
		col_typ : xml2st_str,		\
		col_xml : (xml),			\
		col_ltd : (l),				\
		col_len : stsizeof(t,m),	\
		col_off : offsetof(t,m),	\
		sub_tbl : NULL,				\
	}

//dbl
#define	XML2ST_DEF_MDBL(				\
	xml,t, m)						\
	{								\
		col_opt : 1,				\
		col_typ : xml2st_dbl,		\
		col_xml : (xml),			\
		col_ltd : (0),				\
		col_len : stsizeof(t,m),	\
		col_off : offsetof(t,m),	\
		sub_tbl : NULL,				\
	}

#define	XML2ST_DEF_ODBL(				\
	xml,t, m)						\
	{								\
		col_opt : 0,				\
		col_typ : xml2st_dbl,		\
		col_xml : (xml),			\
		col_ltd : (0),				\
		col_len : stsizeof(t,m),	\
		col_off : offsetof(t,m),	\
		sub_tbl : NULL,				\
	}
	
//ptr
#define	XML2ST_DEF_MPTR(				\
	xml, t, m, sub)					\
	{								\
		col_opt : 1,				\
		col_typ : xml2st_ptr,		\
		col_xml : (xml),			\
		col_ltd : (0),				\
		col_len : stsizeof(t,m),	\
		col_off : offsetof(t,m),	\
		sub_tbl : (&(sub))			\
	}

#define	XML2ST_DEF_OPTR(				\
	xml, t, m, sub)					\
	{								\
		col_opt : 0,				\
		col_typ : xml2st_ptr,		\
		col_xml : (xml),			\
		col_ltd : (0),				\
		col_len : stsizeof(t,m),	\
		col_off : offsetof(t,m),	\
		sub_tbl : (&(sub))			\
	}

#define	XML2ST_DEF_MTBL(n,t,c)			\
	{								\
		tblname : (n),				\
		tbl_len : sizeof(t),		\
		nr_cols : sizeof(c)/		\
			sizeof(c[0]),			\
		tbl_col : (c)				\
	}

#endif

struct xml2st_table
{
	const char					*	tblname;
	size_t							tbl_len;
	size_t							nr_cols;
	const struct xml2st_column	*	tbl_col;
};

struct xml2st_column
{
	unsigned int					col_opt;
	unsigned int					col_typ;
	const char					*	col_xml;

	size_t							col_ltd;
	size_t							col_len;
	size_t							col_off;

	const struct xml2st_table	*	sub_tbl;
};

/*
 * xml2st 错误码定义
 *
 * 成功: XML2ST_E_OK (0)
 * 错误: 正整数，按类别分组
 */
typedef enum {
	XML2ST_E_OK = 0,		/* 操作成功 */

	/* 通用错误 (1-10) */
	XML2ST_E_ERROR = 1,	/* 通用错误 */
	XML2ST_E_NOMEM = 2,	/* 内存分配失败 */
	XML2ST_E_INTERNAL = 3,/* 内部逻辑错误 */

	/* 参数/状态错误 (11-20) */
	XML2ST_E_MISUSE = 11,	/* 库使用不当 */
	XML2ST_E_RANGE = 12,	/* 参数超出范围 */

	/* XML 结构错误 (21-30) */
	XML2ST_E_EMPTY = 21,	/* XML 文档为空 */
	XML2ST_E_CORRUPT = 22,/* XML 结构损坏 */

	/* 数据解析错误 (31-50) */
	XML2ST_E_MISSING = 31,/* 缺少必需字段 */
	XML2ST_E_DUPLICATE = 32,/* 字段重复出现 */
	XML2ST_E_OVERFLOW = 33,/* 数值溢出 */
	XML2ST_E_INVALID = 34,/* 值包含无效字符 */
	XML2ST_E_TOOBIG = 35,/* 字符串超长 */

	/* 编码转换错误 (51-60) */
	XML2ST_E_ICONV = 51,	/* 编码转换失败 */

} xml2st_errcode_t;

typedef struct xml2st_handle		*	xml2st_hndl;

#ifdef	__cplusplus
extern	"C" {
#endif

xml2st_hndl
xml2st_easy_init(
	const struct xml2st_table	*	tbl);

void
xml2st_easy_free(
	xml2st_hndl						hndl);

void *
xml2st_easy_parse(
	xml2st_hndl						hndl,
	xmlDocPtr						_doc);

void *
xml2st_easy_refdp(
	xml2st_hndl						hndl);

/*
 * xml2st_set_encoding - 设置 str 字段目标编码
 *
 * @hndl: 句柄指针
 * @enc: 目标编码（如 "GB18030", "UTF-8"），NULL 表示使用默认编码
 *
 * 注意：
 * - 仅在第一次 xml2st_easy_parse 前调用生效
 * - parse 完成后再次调用将被忽略（不会重新解析）
 * - 编码名称不区分大小写（如 "utf-8" 与 "UTF-8" 等效）
 * - 默认编码：Windows 为 GB18030，其他平台为 UTF-8
 */
void
xml2st_set_encoding(
	xml2st_hndl						hndl,
	const char					*	enc);

/*
 * xml2st_errcode - 获取最后错误码
 *
 * @hndl: 句柄指针
 *
 * Return: 错误码，XML2ST_E_OK 表示无错误
 *
 * 注意：后续操作会覆盖错误码，需立即读取
 */
int
xml2st_errcode(
	xml2st_hndl						hndl);

/*
 * xml2st_errmsg - 获取错误描述
 *
 * @hndl: 句柄指针
 *
 * Return: 错误描述字符串（无需释放）
 *
 * 注意：
 * - 返回句柄内部 buffer，无需 free
 * - 后续操作会覆盖，需立即读取
 * - 无错误时返回 "not an error"
 */
const char *
xml2st_errmsg(
	xml2st_hndl						hndl);

#ifdef __cplusplus
}
#endif
#endif	//	XML2ST_H
