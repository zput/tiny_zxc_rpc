


#include<assert.h>
#include<functional>

#include "Logger.h"

#include"InetAddress.h"
#include"EventLoopThread.h"
#include"EventLoop.h"
#include"TcpConnection.h"
#include"TcpClient.h"

#include"BaseRPC.hpp"                            
#include"SerializationAntiserialization.hpp"


int foo_3(int arg1) {
	                   //buttont_assert(arg1 == 10);
	return arg1 * arg1;
}




void runInThread()
{
	printf("\nrunInThread(): pid = %d, tid \n",getpid());


}



void sendFunction(zxc_net::TcpClient&client, zxc_rpc::SerializationAntiserialization& ds) 
	{

	//	std::string request = "???";

		std::string request;
		char* temp=new char[ds.size()+1]();
		temp[ds.size()]='\0';
		memcpy(temp, ds.data(), ds.size());
		for(int i=0;i<ds.size();i++){
			printf("%d===%d===%x\n",i,temp[i],temp[i]);
			request+=temp[i];
		}
	//	request += "???";

		//for (int i = 0; i<request.size(); i++) {
		//	printf("%d===%d===%x\n", i, request[i], request[i]);
		//}

		delete[] temp;
  //      INFO("sendFunction ds.size():%d---%s",ds.size(),request.c_str());
		//assert(request.size()==6+ds.size());
		client.send(request);

	}

	void readFunction( zxc_net::Buffer& buf)
	{
		std::string reply = buf.retrieveAsString();

		for (int i = 0; i<reply.size(); i++) {
			printf("%d===%d===%x\n", i, reply[i], reply[i]);
		}

		INFO("---%s", reply.c_str());
		zxc_rpc::SerializationAntiserialization ds;
		zxc_rpc::BaseRPC::value_t<int> val;
		if (reply.size() == 0) {
			// timeout
			//	m_error_code = RPC_ERR_RECV_TIMEOUT;
			val.set_code(zxc_rpc::BaseRPC::RPC_ERR_RECV_TIMEOUT);
			val.set_msg("recv timeout");
			INFO("%d", val.val());
			return;
		}
		// m_error_code = RPC_ERR_SUCCESS;
		ds.clear();
		ds.write_raw_data((char*)reply.data(), reply.size());
		ds.reset();

		ds >> val;
		INFO("---%d", val.val());
		return;

	}



	int main() {                        //  client  // 

		using namespace zxc_rpc;

		BaseRPC    baseServer;
		SerializationAntiserialization  ds = baseServer.call<int>("foo_3", 10);

		INFO("I'm enter");
		using namespace zxc_net;
		EventLoop loopF;
		EventLoop* loop = &loopF;

		std::string ip = "192.168.5.129";
		uint16_t port = 30005;
		InetAddress peer(ip, port);

		TcpClient client(loop, &peer);
		client.setMessageCallback(readFunction);

		std::function<void(void)>  f=std::bind( sendFunction, std::ref(client), std::ref(ds) );   
		loop->runAfter(Second(5),f );

		loopF.runAfter(Second(2), runInThread);
		loopF.loop();


		sleep(120);   //sleep two minites;

	}







/*

// 创建一个TcpEcho Function

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

	std::string message=ps;	
	baseRPC.handleMessage( message);
	
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

int main() {
	
	using namespace zxc_rpc;
	
	BaseRPC server;
			  //server.as_server(5555);
	server.bind("foo_3", std::function<int(int)>(foo_3));

	          //std::cout << "run rpc server on: " << 5555 << std::endl;
	          //server.run();
	
	
	
	
	INFO("I'm enter");
	
	using namespace zxc_net;

	EventLoop loopF;
	EventLoop* loop = &loopF;

	std::string ip = "192.168.5.199";
	// unsigned short int port =9009;
	uint16_t port = 30005;
	InetAddress local(ip,port);

	TcpServerMulti  serverAll(loop, local);

	serverAll.setMessageCallback(std::bind(messageFunction,server, std::placeholders::_1, std::placeholders::_2) );
//	serverAll.setWriteCallback(writeFunction);

	serverAll.setConnectionCallback(onConnection);
	serverAll.setNumThread(5);
	serverAll.start();

	loopF.runAfter(Second(2), runInThread);
	loopF.loop();

	sleep(120);   //sleep two minites;

}
*/



/////////////////////////////////////////////


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







