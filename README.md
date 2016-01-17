# xml2st

xml2st 是一个把 XML 报文与 C 结构体直接对应的轻量解析库。

## 为什么做这个库

这个库诞生于一个大量使用 XML 报文进行通信的项目。当时的痛点是：报文种类繁多，每一种都要照着协议文档手写解析与构造代码——字段一个一个对，工作量大，还特别容易出错（漏字段、错类型、错顺序）；而且文档一旦改动，代码就得手工跟着改，两边没有强约束，协议文档本身往往就是一份 XML 样例。

xml2st 就是冲着这个来的：把"协议文档里的 XML"和"代码里的 C 结构体"用一张字段映射表直接对应起来。写协议文档时按约定给出 XML，就能机械地得到对应的 C 结构体定义与映射表，省掉重复的手写解析，也让代码和文档天然一致——文档即定义，定义即代码。

## 特性

- 一张映射表描述 XML 节点与 C 结构体字段的对应关系
- 支持 int / str / dbl / ptr(子表嵌套) 四种字段类型
- 字段可声明为必选或可选
- 解析结果由库内部内存池分配，统一释放
- 日志默认走 stderr，可注入用户回调并携带上下文
- 32/64 位兼容（字段以指针槽位组织，随 `sizeof(void*)` 自适应）
- 仅依赖 libxml2

## 工作原理

解析结果是一个"指针槽位"结构体：每个 XML 字段对应一个 `void*` 大小的槽，槽内存放库在内存池里分配的值指针。因为字段以 `sizeof(void*)` 为粒度，结构体布局在 32 位（4 字节槽）和 64 位（8 字节槽）下都能正确工作。各类型字段的槽里存放：

| 字段类型 | 槽内存放 | C 中读取 |
|---|---|---|
| int | `unsigned int *` | `*p->field` |
| str | `char *` | `p->field`（直接用） |
| dbl | `double *` | `*p->field` |
| ptr（子表） | 子结构体指针 | `p->field`（嵌套） |

以 `example.c` 解析 `<person>` 报文为例，三步走：

**1. 定义与报文对应的结构体（字段是指针类型）**

```c
struct person {
    char         * name;    /* str  字段 */
    unsigned int * age;     /* int  字段 */
    char         * email;
    char         * phone;   /* 可选字段 */
};
```

**2. 用宏声明"字段 <-> XML 节点"映射表**（节点名对应根元素 `<person>` 的子节点）

```c
static const struct xml2st_column person_cols[] = {
    XML2ST_DEF_MSTR("name",  64, struct person, name),    /* 必选 str */
    XML2ST_DEF_MINT("age",       struct person, age),     /* 必选 int */
    XML2ST_DEF_OSTR("email", 64, struct person, email),   /* 可选 str */
    XML2ST_DEF_OSTR("phone", 16, struct person, phone),
};

static const struct xml2st_table person_tbl =
    XML2ST_DEF_MTBL("person", struct person, person_cols);
```

宏命名约定：`M` = 必选（mandatory）/ `O` = 可选（optional），后缀 `INT/STR/DBL/PTR` 表示类型。`MSTR`/`OSTR` 的第二个参数是该字段允许的最大字符长度。`PTR` 类型可挂子表，从而表达报文嵌套。

**3. 初始化 → 解析 → 释放**

```c
xml2st_hndl h = xml2st_easy_init(&person_tbl);   /* 1. 用映射表初始化句柄 */
struct person * p = xml2st_easy_parse(h, doc);    /* 2. 解析, 返回填好的结构体指针 */
if (p != NULL) {
    printf("name=%s age=%u\n", p->name, *p->age); /* str 直接用, int 解引用 */
}
xml2st_easy_free(h);                              /* 3. 释放, p 同时失效 */
```

所有解析结果的内存都在句柄的内存池里，`xml2st_easy_free` 时统一释放——外部无需、也切勿对单个字段单独 `free`。完整可运行示例见 `example.c`，回归测试见 `tests/test_xml2st.c`。

## 编译与运行

依赖：

- **libxml2** 开发包
  - Debian/Ubuntu：`apt install libxml2-dev`
  - RHEL/Fedora/CentOS：`dnf install libxml2-devel`（CentOS 7 等旧版用 `yum`）
  - Arch Linux：`pacman -S libxml2`
- **iconv**（字符编码转换，str 字段解析时用于 UTF-8 与本地编码之间的转换）
  - Linux：glibc 自带，无需额外安装
  - Windows：需 libiconv，例如 MSYS2 下安装 `mingw-w64-x86_64-libiconv`

```
make            # 编译 example 与 test
make example    # 仅示例
make test       # 仅测试
make clean      # 清理产物
make BITS=32    # 编译 32 位 (需 gcc-multilib 与 32 位 libxml2)
```

## 目录结构

```
xml2st/
├── include/   对外公开头 (xml2st.h)
├── src/       内部实现与内部头
├── lib/       改编自 Linux 内核的基础设施 (rbtree / list)
├── tests/     回归测试
├── example.c  用法示例
└── Makefile
```

## 许可证

GPL-2.0。其中 `lib/rbtree.*` 与 `lib/list.h` 改编自 Linux 内核（GPL-2.0-only），随内核原版许可。
