#define O_RDONLY  0x000
#define O_WRONLY  0x001
#define O_RDWR    0x002
#define O_CREATE  0x200
#define O_TRUNC   0x400

#ifdef LAB_MMAP // mmap 实验用的条件编译宏定义
#define PROT_NONE       0x0 //无权限
#define PROT_READ       0x1 //可读
#define PROT_WRITE      0x2 //可写
#define PROT_EXEC       0x4 //可执行

#define MAP_SHARED      0x01 //映射类型
#define MAP_PRIVATE     0x02
#endif
