
#ifndef TCPSERVER_H
#define TCPSERVER_H

#include<string>
#include<vector>
#include<unistd.h>

#include"Callbacks.h"


namespace zxc_net{

class EventLoop;
class InetAddress;
class Accept;
class TcpConnection;


class TcpServer {
    public:
		  TcpServer(EventLoop* loop, InetAddress& local);
	      TcpServer ( EventLoop* loop, std::string& ip, uint16_t port);
	      ~TcpServer ();
	
	
	
	      void newConnection( int );
		  void removeConnection(const std::shared_ptr<TcpConnection>&);
              
          void setMessageCallback(MessageCallback cb) {
			   messageCallback_ =cb;
			   
		   }
          void setWriteCallback(WriteCallback cb ) {
			   writeCallback_ = cb;
		   }
          void setConnectionCallback(ConnectionCallback cb ) {
			   connectionCallback_ = cb;
		   }
		  void setWriteCompleteCallback(const WriteCompleteCallback &cb)
		  {
			  writeCompleteCallback_ = cb;
		  }
   
	private:
		MessageCallback  messageCallback_;
	    WriteCallback    writeCallback_;
	    ConnectionCallback    connectionCallback_;
		WriteCompleteCallback  writeCompleteCallback_;

	private:
	   typedef std::vector<std::shared_ptr<TcpConnection>>  CONNLISTS;
	
	   EventLoop* loop_;
	   InetAddress* address_;
	   Accept *  accept_;
	   //TcpConnection* conn_;
	   
	  // std::vector<std::shared_ptr<TcpConnection>> 
	   CONNLISTS  connLists_;
	
};
	

}

#endif





