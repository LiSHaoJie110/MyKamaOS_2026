// 实现 XV6 的 UNIX 程序 sleep;你的睡眠应暂停用户指定的滴答次数。
// 滴答是由 xv6 核定义的时间概念，即定时器芯片中两次中断之间的时间。你的解决方案应该在 user/sleep.c 文件里。
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h" // 必须以这个顺序 include，由于三个头文件有依赖关系
// 包含必要的头文件，定义了数据类型、文件状态结构、以及用户空间函数声明
// 这些文件定义了 printf()、sleep()、atoi()、exit() 等函数

int main(int argc, char **argv) {
	if(argc < 2) {
		printf("usage: sleep <ticks>\n");
	}
	sleep(atoi(argv[1]));//将第一个参数从字符串转换为整数
	exit(0);//退出程序
}
