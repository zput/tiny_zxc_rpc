#include"InetAddress.h"
#include"EventLoop.h"
#include"TcpConnection.h"
#include"TcpServerMulti.h"
#include"RpcServer.h"
#include"Buffer.h"         

#include "Logger.h"

using namespace zxc_net;

RpcServer::RpcServer(EventLoop* loop,InetAddress* address,int threadNum, zxc_rpc::BaseRPC * baseRpc)
               :loop_(loop),
			    inetAddress_(address),
				threadNum_(threadNum),
                tcpServerMulti_(new TcpServerMulti(loop_,*inetAddress_)),
                baseRpc_(baseRpc),
                buff_( new Buffer())				
{
	tcpServerMulti_->setNumThread(threadNum_);
	tcpServerMulti_->setMessageCallback(std::bind(&RpcServer::messageFunction,this, *baseRpc_, std::placeholders::_1, std::placeholders::_2) );
	tcpServerMulti_->setConnectionCallback( std::bind(&RpcServer::onConnectionFunction,this, std::placeholders::_1) );
	baseRpc_->as_server();
	
}



void RpcServer::start() {
	
	 tcpServerMulti_->start();
	
}

void RpcServer::messageFunction (zxc_rpc::BaseRPC& baseRPC, const std::shared_ptr<zxc_net::TcpConnection> & conn, zxc_net::Buffer& buf) {

	               //   int p = buf.readableBytes();
	               //   std::string ps = buf.retrieveAsString();
     
	
	const int temp=buf.readableBytes();
	std::string str(buf.peek(), temp);
	
	int i=0;
	for( ; i<temp; i++) {
		if(str[i]!='\0') break;
		else continue;
	}
	int j=i;
	for( ; i<temp; i++) {
		if(str[i]=='\0') break;
		else continue;
	}
	
    if(j<i && i<temp) {	
	
	   buf.retrieve(i);
	   std::string message(str.begin()+i,str.begin()+j);
	   INFO("client message: %s", message);
	   baseRPC.handleMessage( message );
	   conn->send(message);
	   
	} else {
		INFO("client message: Not handle");	
	}
	printf("this thread pid is : %d\n", syscall(SYS_gettid));

}




void RpcServer::onConnectionFunction(const std::shared_ptr<zxc_net::TcpConnection> & conn)
{
	if (conn->connected())
	{
		printf("onConnection(): new connection from threadId:%d\n", syscall(SYS_gettid));
	}
	else
	{
		printf("onConnection(): connection is down---threadId:%d\n", syscall(SYS_gettid));
	}
}





