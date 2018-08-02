#include"EventLoop.h"
#include"InetAddress.h"

#include"Accept.h"

#include"TcpConnection.h"
#include"TcpServer.h"

using namespace zxc_net;

TcpServer::TcpServer(EventLoop* loop,InetAddress& local)
	:loop_(loop),
	address_( &local),
	accept_(new Accept(loop_, address_)) {
	accept_->setMessageCallback(messageCallback_);
	// accept_->setNewconnectionCallback([this](){newConnection( );});
	accept_->setNewconnectionCallback(std::bind(&TcpServer::newConnection, this, std::placeholders::_1));
}

 TcpServer::TcpServer (EventLoop* loop, std::string& ip, uint16_t port)
          :loop_(loop),
		   address_( new InetAddress(ip, port)),
		   accept_(new Accept(loop_,address_))  {
	  accept_->setMessageCallback(messageCallback_);
	 // accept_->setNewconnectionCallback([this](){newConnection( );});
	  accept_->setNewconnectionCallback(std::bind(&TcpServer::newConnection, this, std::placeholders::_1));
}
TcpServer::~TcpServer () {
		
}



  void TcpServer::newConnection( int connfd ) {
	  
	    std::shared_ptr<TcpConnection > conn ( new TcpConnection(loop_,connfd));
	    conn->setWriteCallback(writeCallback_);
		conn->setMessageCallback(messageCallback_);
		conn->setConnectionCallback(connectionCallback_);
		conn->setWriteCompleteCallback(writeCompleteCallback_);


	    conn->setremoveConnectionCallback(bind(&TcpServer::removeConnection,this,std::placeholders::_1));
    	conn->connectEstablished();
		
	  connLists_.push_back(conn);
   
  }
   
void TcpServer::removeConnection( const std::shared_ptr<TcpConnection>& conn ) {
	  for(CONNLISTS::iterator i=connLists_.begin(); i!= connLists_.end(); i++) {
		  if(*i==conn){
			  printf("\ntest\n");
			  connLists_.erase(i);
			  printf("\ntest2\n");
			  break;
		  }		  
	  }
      printf("\ntest3\n");
	  loop_->queueInLoop(std::bind(&TcpConnection::destoryConn,conn) ); 
      printf("\nTcpServer::removeConnection ---close \n");
	  
  }
  
  
  
  
  
  
  
  
  
  
  
  
  
  