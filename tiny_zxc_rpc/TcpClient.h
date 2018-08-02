
#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include"Callbacks.h"

namespace zxc_net {

class EventLoop;
class InetAddress;
class TcpConnection;

class TcpClient {
	public:
	   TcpClient(EventLoop* loop, InetAddress* inetAddress);
	   ~TcpClient();

	   void setMessageCallback(MessageCallback cb ) {
			messageCallback_ = cb; 
			conn_->setMessageCallback(messageCallback_);
       }
 
        void removeConnection();
        void send(const std::string& message);
		
	private:
	   MessageCallback  messageCallback_;
	
	private:
	   EventLoop* loop_;
	   InetAddress*  peer_; 
	   TcpConnection* conn_;
  };
	
}

#endif  //TCPCLIENT_H

