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
 *	xml2st_log.h
 *
 *	Copyright (C) 2016 YangLing(yl.tienon@gmail.com)
 *
 *	Description:
 *		解耦日志模块.
 *		默认实现输出到 stderr (printf 语义, 带级别前缀).
 *		可通过 xml2st_log_bind() 注入用户回调, 并携带上下文 ctx,
 *		便于接入应用自有日志系统.
 *
 *	Revision History:
 *
 *	2016-01-13 Created YangLing
 */

#ifndef XML2ST_LOG_H
#define XML2ST_LOG_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 日志级别, 由低到高. */
typedef enum {
	XML2ST_LOG_ERR	= 0,	/* 错误 */
	XML2ST_LOG_WARN	= 1,	/* 警告 */
	XML2ST_LOG_INFO	= 2,	/* 信息 */
	XML2ST_LOG_DEBUG= 3	/* 调试 */
} xml2st_log_lvl_t;

/*
 * xml2st_log_fn_t - 用户日志回调类型
 *
 * @ctx:	xml2st_log_bind() 注册时传入的上下文指针, 透传回用户.
 * @lvl:	日志级别.
 * @fmt:	格式串 (printf 语义).
 * @ap:		与 fmt 对应的变参列表.
 *
 * 用户实现中可用 vfprintf / vsnprintf 等处理 ap.
 */
typedef void (* xml2st_log_fn_t)(
	void			*	ctx,
	xml2st_log_lvl_t		lvl,
	const char		*	fmt,
	va_list				ap);

/*
 * xml2st_log_bind - 绑定用户日志回调
 *
 * @ctx:	用户上下文指针, 透传给回调; 传 NULL 表示无上下文.
 * @fn:		用户回调; 传 NULL 则恢复库默认实现 (vfprintf 到 stderr).
 *
 * 非线程安全, 应在库初始化阶段 (xml2st_easy_init 之前) 单次调用.
 */
void xml2st_log_bind(void * ctx, xml2st_log_fn_t fn);

/*
 * xml2st_log - 库内部统一日志入口
 *
 * @lvl:	日志级别.
 * @fmt:	格式串 (printf 语义).
 * @...:	变参.
 *
 * 库内所有日志调用均经由此函数. 外部一般无需直接调用.
 */
void xml2st_log(xml2st_log_lvl_t lvl, const char * fmt, ...);

#ifdef __cplusplus
}
#endif
#endif	/* XML2ST_LOG_H */
