


#include<functional>

#include"InetAddress.h"
#include"EventLoopThread.h"
#include"EventLoop.h"
#include"TcpServerMulti.h"
#include"TcpConnection.h"
#include"Buffer.h"
#include "Logger.h"

#include"SerializationAntiserialization.hpp"
#include"BaseRPC.hpp"                          

using namespace  zxc_net;


char buff[200];

int foo_3(int arg1) {
	//  buttont_assert(arg1 == 10);
	return arg1 * arg1;
}




void onConnection(const std::shared_ptr<zxc_net::TcpConnection> & conn)
{
	if (conn->connected())
	{
		printf("onConnection(): new connection [xxx] from threadId:%d\n", syscall(SYS_gettid));
	}
	else
	{
		printf("onConnection(): connection [s] is down---threadId:%d\n", syscall(SYS_gettid));
	}
}



void messageFunction(zxc_rpc::BaseRPC& baseRPC, const std::shared_ptr<zxc_net::TcpConnection> & conn, zxc_net::Buffer& buf) {

	int p = buf.readableBytes();
	std::string ps = buf.retrieveAsString();
	printf("\n---------\nonMessage(): received %zd bytes from connection [%zd] at %s\n",
		buf.readableBytes(),
		p,
		ps.c_str());

	std::string message = ps;

	baseRPC.handleMessage(message);

	conn->send(message);

	printf("this thread pid is : %d\n", syscall(SYS_gettid));

}



void writeFunction(char* insideBuff) {
	char* i = buff;
	while (*i != '\0') {
		(*i) -= 32;
		i++;
	}
	// printf("\nwriteFunction\n");
	int j = 0;
	while (buff[j] != '\0'&&j<199) {
		insideBuff[j] = buff[j];
		j++;
	}
	insideBuff[199] = '\0';
}

void runInThread()
{
	printf("\nrunInThread(): pid = %d, tid \n",
		getpid());


}

int main() {                     // server  // 

	using namespace zxc_rpc;

	BaseRPC server;
	server.as_server();
	server.bind("foo_3", std::function<int(int)>(foo_3));

	//std::cout << "run rpc server on: " << 5555 << std::endl;
	//server.run();




	INFO("I'm enter");

	using namespace zxc_net;

	EventLoop loopF;
	EventLoop* loop = &loopF;

	std::string ip = "192.168.5.129";
	// unsigned short int port =9009;
	uint16_t port = 30005;
	InetAddress local(ip, port);

	TcpServerMulti  serverAll(loop, local);

	serverAll.setMessageCallback(std::bind(messageFunction, server, std::placeholders::_1, std::placeholders::_2));
	//	serverAll.setWriteCallback(writeFunction);

	serverAll.setConnectionCallback(onConnection);
	serverAll.setNumThread(5);
	serverAll.start();

	loopF.runAfter(Second(2), runInThread);
	loopF.loop();

	sleep(120);   //sleep two minites;

}









//
//// 创建一个TcpEcho Function
//
//#include"InetAddress.h"
//#include"EventLoopThread.h"
//#include"EventLoop.h"
//#include"TcpServerMulti.h"
//#include"TcpConnection.h"
//#include"Buffer.h"
//#include "Logger.h"
//
//char buff[200];
//
//void onConnection(const std::shared_ptr<zxc_net::TcpConnection> & conn)
//{
//	if (conn->connected())
//	{
//		printf("onConnection(): new connection [xxx] from threadId:%d\n", syscall(SYS_gettid));
//	}
//	else
//	{
//		printf("onConnection(): connection [s] is down---threadId:%d\n", syscall(SYS_gettid));
//	}
//}
//
//
//
//void messageFunction(const std::shared_ptr<zxc_net::TcpConnection> & conn, zxc_net::Buffer& buf) {
//
//	int p = buf.readableBytes();
//	std::string ps = buf.retrieveAsString();
//	printf("onMessage(): received %zd bytes from connection [%zd] at %s\n",
//		buf.readableBytes(),
//		p,
//		ps.c_str());
//
//	conn->send(ps + ps);
//
//	printf("this thread pid is : %d\n", syscall(SYS_gettid));
//
//}
//
//
//
//void writeFunction(char* insideBuff) {
//	char* i = buff;
//	while (*i != '\0') {
//		(*i) -= 32;
//		i++;
//	}
//	// printf("\nwriteFunction\n");
//	int j = 0;
//	while (buff[j] != '\0'&&j<199) {
//		insideBuff[j] = buff[j];
//		j++;
//	}
//	insideBuff[199] = '\0';
//}
//
//void runInThread()
//{
//	printf("\nrunInThread(): pid = %d, tid \n",
//		getpid());
//
//
//}
//
//int main() {
//	INFO("I'm enter");
//	
//	using namespace zxc_net;
//
//	EventLoop loopF;
//	EventLoop* loop = &loopF;
//
//	std::string ip = "192.168.5.129";
//	// unsigned short int port =9009;
//	uint16_t port = 30005;
//	InetAddress local(ip,port);
//
//	TcpServerMulti  serverAll(loop, local);
//
//	serverAll.setMessageCallback(messageFunction);
////	serverAll.setWriteCallback(writeFunction);
//
//	serverAll.setConnectionCallback(onConnection);
//	serverAll.setNumThread(5);
//	serverAll.start();
//
//	loopF.runAfter(Second(2), runInThread);
//	loopF.loop();
//
//	sleep(120);   //sleep two minites;
//
//}



//////////////////////////////////single_thread////////////////////////////////////////



/*
// 创建一个TcpEcho Function

#include"EventLoopThread.h"
#include"EventLoop.h"
#include"TcpServer.h"
#include"TcpConnection.h"
#include"Buffer.h"

char buff[200];

void onConnection(const std::shared_ptr<zxc_net::TcpConnection> & conn)
{
if (conn->connected())
{
printf("onConnection(): new connection [s] from s\n");
}
else
{
printf("onConnection(): connection [s] is down\n");
}
}



void messageFunction(const std::shared_ptr<zxc_net::TcpConnection> & conn, zxc_net::Buffer& buf){

int p=buf.readableBytes();
std::string ps=buf.retrieveAsString();
printf("onMessage(): received %zd bytes from connection [%zd] at %s\n",
buf.readableBytes(),
p,
ps.c_str());

conn->send(ps+ps);

}



void writeFunction(char* insideBuff ) {
char* i=buff;
while(*i!='\0') {
(*i)-=32;
i++;
}
// printf("\nwriteFunction\n");
int j=0;
while(buff[j]!='\0'&&j<199) {
insideBuff[j]=buff[j];
j++;
}
insideBuff[199]='\0';
}

void runInThread()
{
printf("\nrunInThread(): pid = %d, tid \n",
getpid());


}

int main() {
using namespace zxc_net;

std::string ip="192.168.5.199";
// unsigned short int port =9009;
uint16_t port=30005;
// EventLoopThread loopThread;
// EventLoop* loop = loopThread.startLoopInThread();

EventLoop loopF;
EventLoop* loop=&loopF;


TcpServer server(loop, ip, port );
server.setMessageCallback(messageFunction);
server.setWriteCallback(writeFunction);
server.setConnectionCallback(onConnection);

loopF.runAfter(Second(2),runInThread);
loopF.loop();

sleep(120);   //sleep two minites;

}
*/
