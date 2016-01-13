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
 *	example.c
 *
 *	Copyright (C) 2016 YangLing(yl.tienon@gmail.com)
 *
 *	Description:
 *		xml2st 用法示例. 把一个 "person" XML 报文解析成 C 结构体.
 *
 *		注意: xml2st 的解析结果是 "指针槽位" 结构体 —— 每个字段
 *		占一个 void* 槽, 槽内存放库在内存池里分配的值指针:
 *		  int 字段 -> unsigned int *
 *		  str 字段 -> char *
 *		  dbl 字段 -> double *
 *
 *	Revision History:
 *
 *	2016-01-13 Created YangLing
 */

#include "xml2st.h"

#include <stdio.h>
#include <string.h>

/* 与 XML 报文对应的结构体: 每个字段一个指针槽 */
struct person {
	char			* name;	/* str  */
	unsigned int		* age;	/* int  */
	char			* email;/* str  */
	char			* phone;/* str, 可选 */
};

/* 字段<->XML节点 映射表 (节点名对应根元素 <person> 的子节点) */
static const struct xml2st_column person_cols[] = {
	XML2ST_DEF_MSTR("name",  (size_t)64, struct person, name),
	XML2ST_DEF_MINT("age",	            struct person, age),
	XML2ST_DEF_MSTR("email", (size_t)64, struct person, email),
	XML2ST_DEF_OSTR("phone", (size_t)32, struct person, phone),
};

static const struct xml2st_table person_tbl =
	XML2ST_DEF_MTBL("person", struct person, person_cols);

/* 待解析的 XML 报文 (ASCII, 避免 GB18030 转码在 UTF-8 终端乱码) */
static const char sample_xml[] =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<person>\n"
"    <name>ZhangSan</name>\n"
"    <age>28</age>\n"
"    <email>zs@example.com</email>\n"
"    <phone>13800000000</phone>\n"
"</person>\n";

int main(void)
{
	xmlDocPtr		doc;
	xml2st_hndl		h;
	struct person	*	p;
	int			ret = 1;

	doc = xmlReadMemory(sample_xml, (int)strlen(sample_xml),
			NULL, "UTF-8", 0);
	if (doc == NULL) {
		fprintf(stderr, "xmlReadMemory() failed\n");
		return 1;
	}

	h = xml2st_easy_init(&person_tbl);
	if (h == NULL) {
		xmlFreeDoc(doc);
		return 1;
	}

	p = xml2st_easy_parse(h, doc);
	if (p != NULL) {
		printf("name  : %s\n", p->name  ? p->name  : "(null)");
		printf("age   : %u\n", p->age   ? *p->age  : 0U);
		printf("email : %s\n", p->email ? p->email : "(null)");
		printf("phone : %s\n", p->phone ? p->phone : "(null)");
		ret = 0;
	} else {
		fprintf(stderr, "xml2st_easy_parse() failed\n");
	}

	xml2st_easy_free(h);
	xmlFreeDoc(doc);
	xmlCleanupParser();

	return ret;
}
