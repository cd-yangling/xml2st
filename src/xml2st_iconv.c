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
 *	xml2st_iconv.c
 *
 *	Copyright (C) 2014 YangLing(yl.tienon@gmail.com)
 *
 *	Description:
 *
 *	Revision History:
 *
 *	2016-01-13 Created YangLing
 */


#include "iconv.h"
#include "xml2st_log.h"
#include "xml2st_internal.h"

static
int myiconv_iconv(
	const char * ienc, const char   * oenc,
	const char * ibuf, unsigned int   ilen,
	      char * obuf, unsigned int * olen)
{
	int rc = 0;
    iconv_t cd;
    size_t ec;
    size_t lf = *olen;
    char *inbuf = (char *)ibuf;  // 移除const限定
    size_t inbytesleft = ilen;
    size_t outbytesleft = lf;

    cd = iconv_open(oenc, ienc);
    if (cd == (iconv_t)-1) {
        xml2st_log(XML2ST_LOG_ERR, "iconv open %s => %s", ienc, oenc);
        return -1;
    }

    ec = iconv(cd, &inbuf, &inbytesleft, &obuf, &outbytesleft);
    if (ec == (size_t)-1) {
        xml2st_log(XML2ST_LOG_ERR, "iconv char %s => %s", ienc, oenc);
        rc = -1;
    }

    *olen = *olen - outbytesleft;
    iconv_close(cd);
    return rc;
}

/**
 *	myiconv_utf8_2_gb18030 - UTF8编码转换成GB18030
 *
 *	@utf8_buf:		UTF8编码字符串
 *	@utf8_len:		UTF8编码字符串长度
 *	@gb_buf:		GB18030编码字符串
 *	@gb_len:		GB18030编码字符串长度
 *
 *	return
 *		==	0		成功
 *		!=	0		出错
 */
int myiconv_utf8_2_gb18030(
	const char * utf8_buf, unsigned int utf8_len,
	char * gb_buf, unsigned int * gb_len)
{
	return myiconv_iconv("UTF-8", "GB18030",
		utf8_buf, utf8_len, gb_buf, gb_len);
}

/**
 *	myiconv_gb18030_2_utf8 - GB18030编码转换成UTF8
 *
 *	@gb_buf:		GB18030编码字符串
 *	@gb_len:		GB18030编码字符串长度
 *	@utf8_buf:		UTF8编码字符串
 *	@utf8_len:		UTF8编码字符串长度
 *
 *	return
 *		==	0		成功
 *		!=	0		出错
 */
int myiconv_gb18030_2_utf8(
	const char * gb_buf, unsigned int gb_len,
	char * utf8_buf, unsigned int * utf8_len)
{
	return myiconv_iconv("GB18030", "UTF-8",
		gb_buf, gb_len, utf8_buf, utf8_len);
}