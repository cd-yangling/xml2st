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
 *	xml2st_port.h
 *
 *	Copyright (C) 2016 YangLing(yl.tienon@gmail.com)
 *
 *	Description:
 *		跨平台基础头文件, 统一包含标准 C 库头并处理平台差异.
 *		收编自 argo_os_libc.h.
 *
 *	Revision History:
 *
 *	2016-01-13 Created YangLing
 */

#ifndef XML2ST_PORT_H
#define XML2ST_PORT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stddef.h>
#include <assert.h>
#include <stdarg.h>

#ifdef _WIN32
#include <process.h>
#endif

#include <ctype.h>
#include <time.h>
#include <limits.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

/* 跨平台字符串比较（不区分大小写） */
#ifdef _WIN32
#include <string.h>
#define strcasecmp _stricmp
#else
#include <strings.h>
#endif

#endif	/* XML2ST_PORT_H */
