
#ifndef RPCSERVER_H
#define RPCSERVER_H

#include<functional>
#include"BaseRPC.hpp"


using namespace zxc_net;
namespace zxc_net {
	class EventLoop;
	class InetAddress;
	class TcpServerMulti;
	class Buffer;
}

using namespace zxc_rpc;
namespace zxc_rpc {
	class BaseRPC;
}

class  RpcServer {
	public:                  
        RpcServer(EventLoop* loop, InetAddress* address, int threadNum, zxc_rpc::BaseRPC * baseRpc);
		~RpcServer(){};
		
		void messageFunction (zxc_rpc::BaseRPC& baseRPC, const std::shared_ptr<zxc_net::TcpConnection> & conn, zxc_net::Buffer& buf);
		void onConnectionFunction(const std::shared_ptr<zxc_net::TcpConnection> & conn);
		
		void start();
	
	private:
	  
	   EventLoop* loop_;
	   int threadNum_;
	   InetAddress* inetAddress_;
	   TcpServerMulti* tcpServerMulti_;
	   zxc_rpc::BaseRPC* baseRpc_;
	   Buffer* buff_;
	   
};

#endif // RPCSERVER_H

