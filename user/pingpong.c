#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h" //用户态声明
//在绝大多数情况下，char ** 在 C 语言里代表的逻辑含义是：一个字符串数组（Array of Strings）。
int main(int argc,char ** argv){
    //管道练手题，使用 fork() 复制本进程创建子进程，创建两个管道，分别用于父子之间两个方向的数据传输。
    int pp2c[2], pc2p[2]; // 其中 0 为用于从管道读取数据的文件描述符，1 为用于向管道写入数据的文件描述符
	pipe(pp2c); // 创建用于 父进程 -> 子进程 的管道 pipe()创建管道
	pipe(pc2p); // 创建用于 子进程 -> 父进程 的管道
    int pid =fork();
    if(pid != 0){//父进程
        write(pp2c[1],".",1);
        close(pp2c[1]); 
        //使用两个管道进行父子进程通信，需要注意的是如果管道的写端没有close，那么管道中数据为空时对管道的读取将会阻塞。因此对于不需要的管道描述符，要尽可能早的关闭。
        char byte;
        read(pc2p[0],&byte,1);
        printf("%d: received pong\n",getpid());//getpid 得到进程号
        wait(0);//父进程阻塞，等待子进程退出
        //xv6 是一个独立的微型操作系统。它的 write 和 read 并不是普通的 C 语言库函数（像 printf 那样），而是系统调用（System Calls）。
    }
    else{//子进程
        char buf;
		read(pp2c[0], &buf, 1); // 3. 子进程读取管道，收到父进程发送的字节数据
		printf("%d: received ping\n", getpid());
		write(pc2p[1], &buf, 1); // 4. 子进程通过 子->父 管道，将字节送回父进程
    }
    exit(0);
}