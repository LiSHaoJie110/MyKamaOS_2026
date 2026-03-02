// xargs.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// 带参数列表，执行某个程序
void run(char *program, char **args) {
	if(fork() == 0) { // child exec  
		// 为什么一定要 fork()？ 因为 exec 会摧毁当前进程。如果 xargs 主进程直接调用 exec，那它自己就死了，只能处理一行输入。所以必须派出一个“敢死队”（子进程）去被夺舍。
		exec(program, args);
		exit(0);
	}
	return; // parent return
}

int main(int argc, char *argv[]){
	char buf[2048]; // 读入时使用的内存池
	char *p = buf, *last_p = buf; // 当前参数的结束、开始指针
	char *argsbuf[128]; // 全部参数列表，字符串指针数组，包含 argv 传进来的参数和 stdin 读入的参数
	char **args = argsbuf; // 指向 argsbuf 中第一个从 stdin 读入的参数
	for(int i=1;i<argc;i++) {
		// 将 argv 提供的参数加入到最终的参数列表中
		*args = argv[i];
		args++;
	}
	char **pa = args; // 开始读入参数
	//管道符分开了
// 	当你按下回车键的那一瞬间，Shell 发现了中间的管道符 |。于是它做了以下操作：
// 它创建了一根真实的、看不见的水管（Pipe）。
// 它把水管的进水口，接到了左边 echo 程序的“嘴巴”（标准输出 Standard Output，即屏幕）。
// 它把水管的出水口，接到了右边 xargs 程序的“耳朵”（标准输入 Standard Input，即键盘，也就是文件描述符 0）。

	while(read(0, p, 1) != 0) {//从文件描述符 0 读取数据（0 就是标准输入 stdin）
		if(*p == ' ' || *p == '\n') {
			// 读入一个参数完成（以空格分隔，如 `echo hello world`，则 hello 和 world 各为一个参数）
			*p = '\0';	// 将空格替换为 \0 分割开各个参数，这样可以直接使用内存池中的字符串作为参数字符串
						// 而不用额外开辟空间
			*(pa++) = last_p;//// 把这个切出来的单词的首地址，登记到 argsbuf 参数列表里，然后 pa 往后挪
			last_p = p+1;

			if(*p == '\n') {
				// 读入一行完成
				*pa = 0; // 参数列表末尾用 null 标识列表结束
				//“空白纸条”（也就是数字 0，在 C 语言指针的语境下，它等同于 NULL）。
				run(argv[1], argsbuf); // 执行最后一行指令
				pa = args; // 重置读入参数指针，准备读入下一行
			}
		}
		p++;
	}
	if(pa != args) { // 如果最后一行不是空行 猝死收尾没有换行符号\n
		//防止漏掉最后一行没有敲回车
		// 收尾最后一个参数
		*p = '\0';
		*(pa++) = last_p;
		// 收尾最后一行
		*pa = 0; // 参数列表末尾用 null 标识列表结束
		// 执行最后一行指令
		run(argv[1], argsbuf);
	}
	while(wait(0) != -1) {}; // 循环等待所有子进程完成，每一次 wait(0) 等待一个
	//这短短一行代码，是保证所有多进程程序能在终端里干净利落输出的终极法宝！
	exit(0);
}