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
 *	xml2st_field.c
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
#include "xml2st_internal.h"
#include "xml2st_log.h"

void * xml2st_write_int(
	xml2st_memory_t				*	datm,
	struct xml2st_column_in		*	icol,
	const char					*	valp)
{
	unsigned int * result;
	unsigned long	val;
	char	*		endptr = "?";

	result = (unsigned int *)xml2st_std_alloc(
				datm, sizeof(unsigned int *));
	if(__builtin_expect((NULL == result), 0))
	{
		xml2st_log(XML2ST_LOG_ERR, 
			"xml2st_std_alloc(%lu) failed", 
			sizeof(unsigned int *));
		return NULL;
	}

	errno = 0;
	val = strtoul(valp, &endptr, 0);
	if(__builtin_expect(
		(errno == ERANGE || val > (unsigned long)UINT_MAX), 0))
	{
		xml2st_log(XML2ST_LOG_ERR,
			"This field(%s) value(%s) "
			"overflow", icol->rcol->col_xml, valp);
		return NULL;	/*	overflow*/
	}
	*result = (unsigned int)val;

	if(*endptr != '\0')
	{
		xml2st_log(XML2ST_LOG_ERR, 
			"This field(%s) values(%s) "
			"include invalid characters",
			icol->rcol->col_xml, valp);
	}

	return result;
}

void * xml2st_write_dbl(
	xml2st_memory_t				*	datm,
	struct xml2st_column_in		*	icol,
	const char					*	valp)
{
	double	*	result;
	char	*	endptr = "?";

	result = (double *)xml2st_std_alloc(
				datm, sizeof(double *));
	if(__builtin_expect((NULL == result), 0))
	{
		xml2st_log(XML2ST_LOG_ERR, 
			"xml2st_std_alloc(%lu) failed", sizeof(double *));
		return NULL;
	}

	errno = 0;
	*result = strtod(valp, &endptr);
	if(__builtin_expect((ERANGE == errno), 0))
	{
		xml2st_log(XML2ST_LOG_ERR, 
			"This field(%s) value(%s) "
			"overflow", icol->rcol->col_xml, valp);
		return NULL;
	}

	if(*endptr != '\0')
	{
		xml2st_log(XML2ST_LOG_ERR, 
			"This field(%s) values(%s) "
			"include invalid characters",
			icol->rcol->col_xml, valp);
	}

	return result;
}

void * xml2st_write_str(
	xml2st_memory_t				*	datm,
	struct xml2st_column_in		*	icol,
	const char					*	valp,
	const char					*	encoding)
{
	char	*	result;
	char			cvt_buf[1024];
	unsigned int	cvt_len;
	size_t		length = strlen(valp);

	if(__builtin_expect(
		(icol->rcol->col_ltd &&
		length > icol->rcol->col_ltd), 0))
	{
		xml2st_log(XML2ST_LOG_ERR,
			"This field(%s) value(%s) "
			"too long, LTD length %lu",
			icol->rcol->col_xml, valp, icol->rcol->col_ltd);
		return NULL;
	}

	/* str 字段: XML 源为 UTF-8, 按目标编码转换; 目标为 UTF-8 时不转换 */
	if(encoding != NULL &&
	   encoding[0] != '\0' &&
	   0 != strcmp(encoding, "UTF-8") &&
	   0 != strcmp(encoding, "UTF8"))
	{
		cvt_len = sizeof(cvt_buf);
		memset(cvt_buf, 0, sizeof(cvt_buf));
		if(0 != myiconv_convert(
			"UTF-8", encoding, valp, length, cvt_buf, &cvt_len))
		{
			xml2st_log(XML2ST_LOG_ERR,
				"field(%s) 编码转换错误", icol->rcol->col_xml);
			return NULL;
		}

		length = strlen(cvt_buf); valp = cvt_buf;
	}

	/*	ensure including the terminating null character*/
	result = (char *)xml2st_std_alloc(datm, length + 1);
	if(__builtin_expect((NULL == result), 0))
	{
		xml2st_log(XML2ST_LOG_ERR,
			"xml2st_std_alloc(%lu) failed", length + 1);
		return NULL;
	}

	memcpy(result, valp, length);

	return result;
}
