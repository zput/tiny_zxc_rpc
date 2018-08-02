#include "EventLoop.h"
#include "EventLoopThread.h"
#include <stdio.h>

void runInThread()
{
  printf("\nrunInThread(): pid = %d, tid \n",
         getpid());
		 
		 
}

int main()
{
  printf("main(): pid = %d, tid =\n",
         getpid());

  zxc_net::EventLoopThread loopThread;
  zxc_net::EventLoop* loop = loopThread.startLoopInThread();
  loop->runInLoop(runInThread);
  sleep(1);
  printf("\n------------------------\n");
  loop->runAfter(zxc_net::Second(2), runInThread);
  sleep(3);
  loop->quit();

  printf("\n exit main(). \n");
}
