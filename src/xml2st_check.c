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
 *	xml2st_check.c
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
#include "xml2st.h"

int xml2st_icolumn_check(
	xml2st_error_t				*	err,
	struct xml2st_column_in		*	icol,
	void						*	vptr)
{
	if(__builtin_expect(
		((icol->rcol->col_opt) && (NULL == vptr)), 0))
	{
		xml2st_set_error(err, XML2ST_E_MISSING,
			"missing required field '%s'",
			icol->rcol->col_xml);
		return -1;
	}

	return 0;
}

int xml2st_rcolumn_check(
	xml2st_error_t				*	err,
	const struct xml2st_column	*	rcol)
{
	if(__builtin_expect(
		(NULL == rcol->col_xml), 0))
	{
		xml2st_set_error(err, XML2ST_E_MISUSE,
			"field XML tag is NULL");
		return -1;
	}

	if(__builtin_expect(
		(sizeof(void*) != rcol->col_len), 0))
	{
		xml2st_set_error(err, XML2ST_E_MISUSE,
			"unexpected field length %zu (expected %zu) for field '%s'",
			rcol->col_len, sizeof(void*), rcol->col_xml);
		return -1;
	}

	if(__builtin_expect(
		(rcol->col_off % sizeof(void*)), 0))
	{
		xml2st_set_error(err, XML2ST_E_MISUSE,
			"unexpected field offset %zu for field '%s'",
			rcol->col_off, rcol->col_xml);
		return -1;
	}

	switch(rcol->col_typ)
	{
	case xml2st_int:
	case xml2st_str:
	case xml2st_dbl:
		{
			if(__builtin_expect(
				(NULL != rcol->sub_tbl), 0))
			{
				xml2st_set_error(err, XML2ST_E_MISUSE,
					"unexpected sub-table for non-pointer field '%s'",
					rcol->col_xml);
				return -1;
			}
		}
		break;
	case xml2st_ptr:
		{
			if(__builtin_expect(
				(NULL == rcol->sub_tbl), 0))
			{
				xml2st_set_error(err, XML2ST_E_MISUSE,
					"missing sub-table for pointer field '%s'",
					rcol->col_xml);
				return -1;
			}
		}
		break;
	default:
		xml2st_set_error(err, XML2ST_E_INTERNAL,
			"unexpected field type: %d", rcol->col_typ);
		return -1;
	}

	return 0;
}

int xml2st_rtable_check(
	xml2st_error_t				*	err,
	const struct xml2st_table	*	rtbl,
	size_t							calc)
{
	size_t							size;
	int								result;

	size = rtbl->nr_cols * sizeof(void*);

	result = ((size == calc) ? 0 : -1);

	if(__builtin_expect(result, 0))
	{
		xml2st_set_error(err, XML2ST_E_MISUSE,
			"table '%s' field size mismatch: %zu != %zu",
			rtbl->tblname, calc, size);
		return result;
	}

	if(rtbl->tbl_len != size)
	{
		xml2st_set_error(err, XML2ST_E_MISUSE,
			"table '%s' structure size mismatch: %zu != %zu",
			rtbl->tblname, rtbl->tbl_len, size);
		return -1;
	}

	return 0;
}