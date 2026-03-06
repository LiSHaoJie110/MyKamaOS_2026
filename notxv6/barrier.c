#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>

static int nthread = 1;
static int round = 0;

struct barrier {//全局状态被封装在一个名为 bstate 的结构体里。它就是那个“签到本”
  pthread_mutex_t barrier_mutex;//保护共享状态
  pthread_cond_t barrier_cond;//阻塞/唤醒线程
  int nthread;      // 当前这一轮已经到达屏障的线程计数
  int round;     // 当前轮次号（最关键）
} bstate;

static void
barrier_init(void)
{
  assert(pthread_mutex_init(&bstate.barrier_mutex, NULL) == 0);
  assert(pthread_cond_init(&bstate.barrier_cond, NULL) == 0);
  bstate.nthread = 0;
}

static void 
barrier()
{
  // YOUR CODE HERE
  //
  // Block until all threads have called barrier() and
  // then increment bstate.round.
  //
  pthread_mutex_lock(&bstate.barrier_mutex);//加锁
  if(++bstate.nthread < nthread) { //记录线程数
    // 3. 如果我不是最后一个，那就乖乖睡觉
    // 这个函数会自动放开锁，睡醒后再自动拿回锁
    pthread_cond_wait(&bstate.barrier_cond, &bstate.barrier_mutex);//非最后一个线程
  } else {//最后一个线程
    bstate.nthread = 0;
    bstate.round++;
    pthread_cond_broadcast(&bstate.barrier_cond);//叫醒所有人
  }
  pthread_mutex_unlock(&bstate.barrier_mutex);//解锁返回
  
}

static void *
thread(void *xa)
{
  long n = (long) xa;
  long delay;
  int i;

  for (i = 0; i < 20000; i++) {
    int t = bstate.round;
    assert (i == t); //assert(...) 的意思是：断言这个条件必须为真
    barrier();//等待
    usleep(random() % 100);//模拟操作速度不同
  }

  return 0;
}

int
main(int argc, char *argv[])
{
  pthread_t *tha;// tha 代表 Thread Array（线程数组）。这是一个指针，稍后会用来存所有线程的 ID。
  void *value;
  long i;
  double t1, t0;

  if (argc < 2) {
    fprintf(stderr, "%s: %s nthread\n", argv[0], argv[0]);
    exit(-1);
  }
  nthread = atoi(argv[1]);
  tha = malloc(sizeof(pthread_t) * nthread);
  srandom(0);

  barrier_init();

  for(i = 0; i < nthread; i++) {
    assert(pthread_create(&tha[i], NULL, thread, (void *) i) == 0);//使用一个循环，调用 pthread_create 创建出指定数量的线程。
  }
  for(i = 0; i < nthread; i++) {
    // pthread_join 的意思是：“我就坐在这，等 i 号线程干完活儿死掉为止”。
    // 如果 i 号线程还在跑，主函数就会在这里卡住（阻塞）
    assert(pthread_join(tha[i], &value) == 0);//使用 pthread_join 等待所有线程把 20000 轮跑完
  }
  printf("OK; passed\n");
}
