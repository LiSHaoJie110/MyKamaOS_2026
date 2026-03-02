// find.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"//内核的文件操作

void find(char *path, char *target) {
//当你看到 char *path 时：
// path 手里只握着这串字符里第一个字符的地址。
// 当 C 语言的函数去读取 path 时，它会从这个起始地址开始，一个字节一个字节地往后读，直到它撞见那个 '\0'，它才会停下来，并认为“哦，这个字符串到此结束了”。
// 在物理层面，它们不是字符串；但在逻辑层面上，我们把它们“当做”字符串来用。
	char buf[512], *p;
	int fd;
	struct dirent de;//目录项 (Directory Entry)。
	struct stat st;//保存文件信息的结构体。里面有一个 type 字段，用来区分这是一个普通文件 (T_FILE)、目录 (T_DIR) 还是设备 (T_DEVICE)。

    // 1. 打开当前路径
	if((fd = open(path, 0)) < 0){
		fprintf(2, "find: cannot open %s\n", path);//fprintf(2, ...) 意思是将错误信息输出到标准错误流（文件描述符 2）。
		return;
	}
    //fstat(int fd, struct stat *st): 通过文件描述符 (fd) 获取文件信息，存入 st 中
    //stat(char *path, struct stat *st): 功能和 fstat 一样，但它是通过文件路径 (path) 来获取信息的。
    // 2. 获取当前路径的文件状态信息
	if(fstat(fd, &st) < 0){
		fprintf(2, "find: cannot stat %s\n", path);
		close(fd);
		return;
	}

	switch(st.type){
	case T_FILE:
		// 如果文件名结尾匹配 `/target`，则视为匹配
		if(strcmp(path+strlen(path)-strlen(target), target) == 0) {
			printf("%s\n", path);
		}
		break;
	case T_DIR:
		if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){//缓冲区溢出保护
			printf("find: path too long\n");
			break;
		}
		strcpy(buf, path);
		p = buf+strlen(buf);
		*p++ = '/';
		while(read(fd, &de, sizeof(de)) == sizeof(de)){//如果目录里还有文件，操作系统会精准地切下 16 个字节的数据塞进 de 盒子里，然后返回数字 16。
			if(de.inum == 0)
				continue; //跳过空文件
			memmove(p, de.name, DIRSIZ);
            //memmove(p, de.name, DIRSIZ);：把读到的文件名（de.name），直接复制到指针 p 现在指着的地方（正好是 / 的后面）。
			p[DIRSIZ] = 0;
			if(stat(buf, &st) < 0){
				printf("find: cannot stat %s\n", buf);
				continue;
			}
			// 不要进入 `.` 和 `..`
			if(strcmp(buf+strlen(buf)-2, "/.") != 0 && strcmp(buf+strlen(buf)-3, "/..") != 0) {
				find(buf, target); // 递归查找
			}
		}
		break;
	}
	close(fd);
}

int main(int argc, char *argv[])
{
	if(argc < 3){
		exit(0);// 检查参数数量。命令格式应为：find <搜索路径> <目标文件名>
	}

	char target[512];
	target[0] = '/'; // 为查找的文件名添加 / 在开头
	strcpy(target+1, argv[2]);// 将用户输入的文件名接在 '/' 后面 防止误判文件
    // strcpy(目的地, 来源); target是地址，地址加1 因为放了一个/

	find(argv[1], target);
	exit(0);
}