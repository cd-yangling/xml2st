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
 *	test_xml2st.c
 *
 *	Copyright (C) 2016 YangLing(yl.tienon@gmail.com)
 *
 *	Description:
 *		xml2st 基本功能回归测试.
 *
 *	Revision History:
 *
 *	2016-01-13 Created YangLing
 */

#include "xml2st.h"

#include <stdio.h>
#include <string.h>

/* 测试用结构体: 指针字段 (每个占一个 sizeof(void*) 槽) */
struct person {
	char			* name;	/* 必选 str */
	unsigned int		* age;	/* 必选 int */
	char			* email;/* 可选 str */
	char			* phone;/* 可选 str */
};

static const struct xml2st_column person_cols[] = {
	XML2ST_DEF_MSTR("name",  (size_t)16, struct person, name),
	XML2ST_DEF_MINT("age",	            struct person, age),
	XML2ST_DEF_OSTR("email", (size_t)32, struct person, email),
	XML2ST_DEF_OSTR("phone", (size_t)16, struct person, phone),
};

static const struct xml2st_table person_tbl =
	XML2ST_DEF_MTBL("person", struct person, person_cols);

/* 简单断言计数 */
static int g_pass = 0;
static int g_fail = 0;

#define CHECK(cond, msg) do {					\
	if (cond) {						\
		g_pass++;					\
		printf("  PASS  %s\n", (msg));			\
	} else {						\
		g_fail++;					\
		printf("  FAIL  %s\n", (msg));			\
	}							\
} while (0)

/* 解析 XML 字符串; 成功返回 0 并输出 handle/结构体指针 (调用者负责 free) */
static int do_parse(const char * xml,
		xml2st_hndl * h_out, struct person ** p_out)
{
	xmlDocPtr	doc;

	*h_out = NULL;
	*p_out = NULL;

	doc = xmlReadMemory(xml, (int)strlen(xml), NULL, "UTF-8", 0);
	if (doc == NULL)
		return -1;

	*h_out = xml2st_easy_init(&person_tbl);
	if (*h_out != NULL)
		*p_out = xml2st_easy_parse(*h_out, doc);

	xmlFreeDoc(doc);
	return (*p_out != NULL) ? 0 : -1;
}

/* 1) 全字段正常解析 */
static void test_basic(void)
{
	static const char xml[] =
"<person><name>Tom</name><age>20</age>"
"<email>t@e.com</email><phone>123</phone></person>";

	xml2st_hndl	h = NULL;
	struct person	* p = NULL;

	printf("[test_basic]\n");
	if (do_parse(xml, &h, &p) != 0) {
		CHECK(0, "parse succeeds");
	} else {
		CHECK(p->name  != NULL && strcmp(p->name,  "Tom")     == 0,
			"name == Tom");
		CHECK(p->age   != NULL && *p->age == 20U,
			"age == 20");
		CHECK(p->email != NULL && strcmp(p->email, "t@e.com") == 0,
			"email == t@e.com");
		CHECK(p->phone != NULL && strcmp(p->phone, "123")     == 0,
			"phone == 123");
	}
	if (h != NULL)
		xml2st_easy_free(h);
}

/* 2) 缺可选字段 (email/phone) 应成功, 对应槽为 NULL */
static void test_optional_missing(void)
{
	static const char xml[] =
"<person><name>Tom</name><age>20</age></person>";

	xml2st_hndl	h = NULL;
	struct person	* p = NULL;

	printf("[test_optional_missing]\n");
	CHECK(do_parse(xml, &h, &p) == 0, "parse succeeds without optional");
	if (p != NULL) {
		CHECK(p->email == NULL, "email slot NULL");
		CHECK(p->phone == NULL, "phone slot NULL");
	}
	if (h != NULL)
		xml2st_easy_free(h);
}

/* 3) 缺必选字段 (name) 应失败 */
static void test_mandatory_missing(void)
{
	static const char xml[] =
"<person><age>20</age></person>";

	xml2st_hndl	h = NULL;
	struct person	* p = NULL;

	printf("[test_mandatory_missing]\n");
	CHECK(do_parse(xml, &h, &p) != 0, "parse fails without mandatory name");
	if (h != NULL)
		xml2st_easy_free(h);
}

/* 4) 字段值超过 col_ltd 应失败 */
static void test_str_too_long(void)
{
	static const char xml[] =
"<person><name>ThisNameIsWayTooLongXYZ</name><age>20</age></person>";

	xml2st_hndl	h = NULL;
	struct person	* p = NULL;

	printf("[test_str_too_long]\n");
	CHECK(do_parse(xml, &h, &p) != 0, "parse fails when name too long");
	if (h != NULL)
		xml2st_easy_free(h);
}

int main(void)
{
	printf("==== xml2st tests ====\n");
	test_basic();
	test_optional_missing();
	test_mandatory_missing();
	test_str_too_long();
	printf("\n==== %d passed, %d failed ====\n", g_pass, g_fail);
	xmlCleanupParser();
	return (g_fail != 0) ? 1 : 0;
}
