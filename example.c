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
 *		xml2st 用法示例.
 *		例1: 基本类型字段 (person).
 *		例2: 结构体嵌套 (order 订单: 外层基本信息 + 内层明细列表),
 *		     演示 ptr 子表字段.
 *
 *	Revision History:
 *
 *	2016-01-13 Created YangLing
 */

#include "xml2st.h"

#include <stdio.h>
#include <string.h>

/* ============================================================
 * 例1: 基本类型字段
 * ============================================================ */

struct person {
	char			* name;
	unsigned int		* age;
	char			* email;
	char			* phone;
};

static const struct xml2st_column person_cols[] = {
	XML2ST_DEF_MSTR("name",  (size_t)16, struct person, name),
	XML2ST_DEF_MINT("age",	            struct person, age),
	XML2ST_DEF_OSTR("email", (size_t)32, struct person, email),
	XML2ST_DEF_OSTR("phone", (size_t)16, struct person, phone),
};

static const struct xml2st_table person_tbl =
	XML2ST_DEF_MTBL("person", struct person, person_cols);

static const char person_xml[] =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<person>\n"
"    <name>ZhangSan</name>\n"
"    <age>28</age>\n"
"    <email>zs@example.com</email>\n"
"    <phone>13800000000</phone>\n"
"</person>\n";

/* ============================================================
 * 例2: 结构体嵌套 —— 订单(基本信息) + 明细列表(重复 item)
 *
 * ptr 字段 (items) 在结构体里是一个 "子表指针数组":
 *   struct order_item ** items;
 * 数组元素个数由同级的 item_count 字段给出 (库本身不记录个数,
 * 需由报文或业务侧提供), 遍历 items[0 .. item_count-1].
 * ============================================================ */

/* 明细项 */
struct order_item {
	char			* product;
	unsigned int		* qty;
	char			* price;
};

/* 订单: 外层基本信息 + 内层明细列表 */
struct order {
	char			* order_id;
	char			* customer;
	unsigned int		* item_count;	/* 明细数量 */
	struct order_item	** items;	/* 明细列表 (ptr 子表) */
};

static const struct xml2st_column order_item_cols[] = {
	XML2ST_DEF_MSTR("product", (size_t)32, struct order_item, product),
	XML2ST_DEF_MINT("qty",	              struct order_item, qty),
	XML2ST_DEF_MSTR("price",  (size_t)16, struct order_item, price),
};

static const struct xml2st_table order_item_tbl =
	XML2ST_DEF_MTBL("item", struct order_item, order_item_cols);

static const struct xml2st_column order_cols[] = {
	XML2ST_DEF_MSTR("order_id",  (size_t)32, struct order, order_id),
	XML2ST_DEF_MSTR("customer",  (size_t)32, struct order, customer),
	XML2ST_DEF_MINT("item_count",           struct order, item_count),
	XML2ST_DEF_MPTR("items", struct order, items, order_item_tbl),
};

static const struct xml2st_table order_tbl =
	XML2ST_DEF_MTBL("order", struct order, order_cols);

static const char order_xml[] =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<order>\n"
"    <order_id>SO20160113001</order_id>\n"
"    <customer>Acme Corp</customer>\n"
"    <item_count>2</item_count>\n"
"    <items>\n"
"        <product>Widget</product>\n"
"        <qty>100</qty>\n"
"        <price>9.50</price>\n"
"    </items>\n"
"    <items>\n"
"        <product>Gadget</product>\n"
"        <qty>50</qty>\n"
"        <price>19.99</price>\n"
"    </items>\n"
"</order>\n";

int main(void)
{
	/* 例1: person (基本类型) */
	{
		xmlDocPtr		doc;
		xml2st_hndl		h;
		struct person	*	p;

		doc = xmlReadMemory(person_xml, (int)strlen(person_xml),
				NULL, "UTF-8", 0);
		h = xml2st_easy_init(&person_tbl);
		p = (doc != NULL && h != NULL) ?
			(struct person *)xml2st_easy_parse(h, doc) : NULL;

		printf("== person ==\n");
		if (p != NULL) {
			printf("name  : %s\n", p->name);
			printf("age   : %u\n", p->age   ? *p->age   : 0U);
			printf("email : %s\n", p->email ? p->email : "(null)");
			printf("phone : %s\n", p->phone ? p->phone : "(null)");
		} else {
			fprintf(stderr, "person parse failed\n");
		}

		if (h != NULL)
			xml2st_easy_free(h);
		if (doc != NULL)
			xmlFreeDoc(doc);
	}

	/* 例2: order (结构体嵌套) */
	{
		xmlDocPtr		doc;
		xml2st_hndl		h;
		struct order	*	o;
		unsigned int		i, n;

		doc = xmlReadMemory(order_xml, (int)strlen(order_xml),
				NULL, "UTF-8", 0);
		h = xml2st_easy_init(&order_tbl);
		o = (doc != NULL && h != NULL) ?
			(struct order *)xml2st_easy_parse(h, doc) : NULL;

		printf("\n== order ==\n");
		if (o != NULL) {
			n = o->item_count ? *o->item_count : 0U;
			printf("order_id : %s\n", o->order_id);
			printf("customer : %s\n", o->customer);
			printf("items    : %u\n", n);
			for (i = 0U; i < n; i++) {
				printf("  [%u] product=%s qty=%u price=%s\n", i,
					o->items[i]->product,
					*o->items[i]->qty,
					o->items[i]->price);
			}
		} else {
			fprintf(stderr, "order parse failed\n");
		}

		if (h != NULL)
			xml2st_easy_free(h);
		if (doc != NULL)
			xmlFreeDoc(doc);
	}

	xmlCleanupParser();
	return 0;
}
