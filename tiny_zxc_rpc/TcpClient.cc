

#include"EventLoop.h"
#include"InetAddress.h"
#include"TcpConnection.h"

#include"TcpClient.h"

#include"Logger.h"
using namespace zxc_net;

TcpClient::TcpClient(EventLoop* loop, InetAddress* inetAddress) 
           :loop_(loop),
            peer_(inetAddress),
		    conn_(new TcpConnection(loop_,peer_) )  {
		
		conn_->setremoveConnectionCallback(std::bind(&TcpClient::removeConnection, this) );
		conn_->connectEstablished();
		
  }

  
  
  TcpClient::~TcpClient() {
	  INFO("~TcpClient");
	  delete conn_; 
	  	  
 }
 

 void TcpClient::removeConnection(){
	   
	  loop_->queueInLoop(   std::bind(&TcpConnection::destoryConn,conn_)   );
 }
 
 
 void TcpClient::send(const std::string& message){
	  conn_->send(message);
	 
 }
 