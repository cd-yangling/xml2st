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
 *	xml2st_log.c
 *
 *	Copyright (C) 2016 YangLing(yl.tienon@gmail.com)
 *
 *	Description:
 *		解耦日志模块实现.
 *
 *	Revision History:
 *
 *	2016-01-13 Created YangLing
 */

#include "xml2st_log.h"

#include <stdio.h>

static void			*	s_log_ctx	= NULL;
static xml2st_log_fn_t		s_log_fn	= NULL;

/* 默认日志实现: 输出到 stderr, 带级别前缀与换行. */
static void xml2st_log_default(
	void			*	ctx,
	xml2st_log_lvl_t		lvl,
	const char		*	fmt,
	va_list				ap)
{
	static const char	* prefix[] = {
		"[xml2st][E] ",
		"[xml2st][W] ",
		"[xml2st][I] ",
		"[xml2st][D] ",
	};

	(void)ctx;

	if ((unsigned)lvl <= XML2ST_LOG_DEBUG)
		fputs(prefix[lvl], stderr);

	vfprintf(stderr, fmt, ap);
	fputc('\n', stderr);
}

void xml2st_log_bind(void * ctx, xml2st_log_fn_t fn)
{
	s_log_ctx	= ctx;
	s_log_fn	= fn;
}

void xml2st_log(xml2st_log_lvl_t lvl, const char * fmt, ...)
{
	va_list			ap;
	xml2st_log_fn_t	fn;

	fn = s_log_fn ? s_log_fn : xml2st_log_default;

	va_start(ap, fmt);
	fn(s_log_ctx, lvl, fmt, ap);
	va_end(ap);
}
