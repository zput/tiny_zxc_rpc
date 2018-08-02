// copied from muduo/net/tests/TimerQueue_unittest.cc

#include "EventLoop.h"
#include "Timestamp.h"
#include <functional>

#include <stdio.h>
#include <ctime>

int cnt = 0;
zxc_net::EventLoop* g_loop;

void printTid()
{
 // printf("pid = %d, tid = %d\n", getpid(), muduo::CurrentThread::tid());
    printf("pid = %d, tid =\n", getpid());
	
  // printf("now %s\n", muduo::Timestamp::now().toString().c_str());
  time_t t=std::chrono::system_clock::to_time_t(zxc_net::clock::now());
   printf("now %s\n", ctime(&t));
}

void print(const char* msg)
{
  time_t t=std::chrono::system_clock::to_time_t(zxc_net::clock::now());
	
  printf("msg %s %s\n", ctime(&t), msg);
  if (++cnt == 20)
  {
    g_loop->quit();
  }
}

int main()
{
  printTid();
  zxc_net::EventLoop loop;
  g_loop = &loop;

  print("main");
  /*
  for(int i=0;i<20; i++) {
    loop.runAt( zxc_net::clock::now() , std::bind(print,"ok--now"));
	sleep(1);
  }
*/
  loop.runAfter(zxc_net::Second(1), std::bind(print, "once1"));
 // loop.runAfter(1.5, boost::bind(print, "once1.5"));
 // loop.runAfter(2.5, boost::bind(print, "once2.5"));
 // loop.runAfter(3.5, boost::bind(print, "once3.5"));
  // loop.runEvery(2, boost::bind(print, "every2"));
  loop.runEvery(zxc_net::Second(3), std::bind(print, "every3"));
 
 
  loop.loop();
  print("main loop exits");
  sleep(1);
}
