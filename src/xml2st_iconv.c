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

int myiconv_convert(
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

