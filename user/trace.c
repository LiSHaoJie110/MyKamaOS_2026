#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

//用户态的trace函数
int
main(int argc, char *argv[])
{
  int i;
  char *nargv[MAXARG];

  if(argc < 3 || (argv[1][0] < '0' || argv[1][0] > '9')){
    fprintf(2, "Usage: %s mask command\n", argv[0]);
    exit(1);
  }

  if (trace(atoi(argv[1])) < 0) {
    fprintf(2, "%s: trace failed\n", argv[0]);
    exit(1);
  }
  
  for(i = 2; i < argc && i < MAXARG; i++){//移花接木：整理参数柜子
    nargv[i-2] = argv[i];
  }
  exec(nargv[0], nargv);
  exit(0);
// 我们前面讲过，exec 的作用是“夺舍”。它会把当前进程的内存代码全部清空，替换成 grep 的代码。
// 但是！exec 绝对不会改变这个进程的 PID，也不会换掉这个进程在内核里的“户口本”（struct proc）
}
