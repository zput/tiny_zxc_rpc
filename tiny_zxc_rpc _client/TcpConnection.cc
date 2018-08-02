
#include<unistd.h>
#include <sys/ioctl.h>

#include"Channel.h"
#include"EventLoop.h"
#include"InetAddress.h"
#include"TcpConnection.h"
#include "Logger.h"



namespace zxc_net
{


	void defaultThreadInitCallback(size_t index)
	{
		TRACE("EventLoop thread #%lu started", index);
	}

	void defaultConnectionCallback(const TcpConnectionPtr& conn)
	{
		//INFO("connection %s -> %s %s",
		//	conn->peer().toIpPort().c_str(),
		//	conn->local().toIpPort().c_str(),
		//	conn->connected() ? "up" : "down");

		printf("\ndefaultConnectionCallback\n");
	}

	void defaultMessageCallback(const TcpConnectionPtr& conn, Buffer& buffer)
	{
		//TRACE("connection %s -> %s recv %lu bytes",
		//	conn->peer().toIpPort().c_str(),
		//	conn->local().toIpPort().c_str(),
		//	buffer.readableBytes());
		//buffer.retrieveAll();
		printf("\ndefaultMessageCallback\n");
	}



	
}

namespace {

	int creatSocket() {
		int temp_socket = ::socket(AF_INET, SOCK_STREAM , 0);
		if (temp_socket>0) {
			return temp_socket;

		}
		else {
			INFO("creatSocket error\n");
			return -1;
		}

	}


}



using namespace zxc_net;

TcpConnection::TcpConnection (EventLoop* loop,InetAddress* inetAddress) 
                  :loop_(loop),
				   peer_(inetAddress),
				   connfd_(creatSocket()),
                   connChannel_(new Channel(loop_, connfd_) ),
                   state_(kConnecting)		{
	 				   
	 connChannel_->setReadCallback( std::bind(&TcpConnection::handleRead,this) );
	 
	// connChannel_->setWriteCallback( [this](){handleWrite();} );
	// connChannel_->enableReading();
	int temp_return = ::connect(connfd_,peer_->getSockaddr() , peer_->getSocklen());
	if(temp_return == 0) {
		INFO("connection success");
		unsigned long ul = 1;
        ioctl(connfd_, FIONBIO, &ul );
	} else {
		SYSERR("connection failure");
	}

}


TcpConnection::~TcpConnection ( ){
	
	
}




void TcpConnection::send(const std::string& message)
{
  if (state_ == kConnected) {
    if (loop_->isInLoopThread()) {
      sendInLoop(message);
    } else {
      loop_->runInLoop(
          std::bind(&TcpConnection::sendInLoop, this, message));
    }
  }
}

void TcpConnection::sendInLoop(const std::string& message)
{
  INFO("the length of send %d",message.size() );
  loop_->assertInLoopThread();
  ssize_t nwrote = 0;
  // if no thing in output queue, try writing directly
  if (!connChannel_->isWriting() && outputBuffer_.readableBytes() == 0) {
    nwrote = ::write(connChannel_->fd(), message.data(), message.size());
    if (nwrote >= 0) {
      if ((size_t)nwrote < message.size()) {
        INFO("I am going to write more data");
	   printf("\nI am going to write more data\n");
	  } else if (writeCompleteCallback_) {
			  // user may send data in writeCompleteCallback_
			  // queueInLoop can break the chain
			  loop_->queueInLoop(std::bind(
				  writeCompleteCallback_, shared_from_this()));
		 }
    } else {
      nwrote = 0;
      if (errno != EWOULDBLOCK) {
        SYSERR("TcpConnection::sendInLoop");
		printf("\nTcpConnection::sendInLoop\n");
      }
    }
  }

  assert(nwrote >= 0);
  if ((size_t)nwrote < message.size()) {
    outputBuffer_.append(message.data()+nwrote, message.size()-nwrote);
    if (!connChannel_->isWriting()) {
		connChannel_->enableWriting();
    }
  }
}




void TcpConnection::handleRead(){  
	  int savedErrno, n;
	  n = inputBuffer_.readFd(connfd_, &savedErrno);
	  
	 if(n>0){ 
		//	std::string temp; 
		//    temp=inputBuffer_.retrieveAsString();
		    INFO("handleRead");
			if(messageCallback_){
			   INFO("handleRead") ;
			   messageCallback_( inputBuffer_);
			}
		//	connChannel_->enableWriting(); 
	} 
	else if (n==0) {
		// 有触发,但是读到的是zero, 说明对端关闭了
          handleClose();
		  printf("\nTcpConnection::handleRead()---handlClose\n");
	}
	else {
		// 其它有错误应该怎么处理呢? 
	}
}
              




			  
void TcpConnection::handleWrite()
{
  loop_->assertInLoopThread();
  if (connChannel_->isWriting()) {
    ssize_t n = ::write(connChannel_->fd(),
                        outputBuffer_.peek(),
                        outputBuffer_.readableBytes());
    if (n > 0) {
      outputBuffer_.retrieve(n);
      if (outputBuffer_.readableBytes() == 0) {
		  connChannel_->disableWriting();
		  if (writeCompleteCallback_) {
			  // user may send data in writeCompleteCallback_
			  // queueInLoop can break the chain
			  loop_->queueInLoop(std::bind(
				  writeCompleteCallback_, shared_from_this()));
		  }

        if (state_ == kDisconnecting) {
          // shutdownInLoop();
        }
      } else {
        //LOG_TRACE << "I am going to write more data";
		printf("\nI am going to write more data\n");
      }
    } else {
      //LOG_SYSERR << "TcpConnection::handleWrite";
	  printf("\nTcpConnection::handleWrite\n");
    }
  } else {
    //LOG_TRACE << "Connection is down, no more writing";
	printf("\nConnection is down, no more writing\n");
  }
}







void TcpConnection::handleClose() {
     connChannel_->disableAll();    
	 
   //  std::shared_ptr<TcpConnection> temp( shared_from_this() );       
 	 printf("\nTcpConnection::handleRead()---handlClose\n");
     if(removeConnection_) {
	  	 removeConnection_();
	 }
     // else 没有removeConnection_ 那么会怎样, 只是 LOG<< xxx ??? 
}





void TcpConnection::connectEstablished()
{
  loop_->assertInLoopThread();
  assert(state_ == kConnecting);
  
  setState(kConnected);
  connChannel_->enableReading();
  // connectionCallback_(shared_from_this());  //client not needs 
}



void TcpConnection::destoryConn(){
	 // 上方的channel HandleEvent已经结束了, 所以要删除poll 的struct pollfd; 
	 printf("\nTcpConnection::destoryConn()---destoryConn\n");
	 setState(kDisconnected);
   	 connChannel_->deleteChanel();
	 
}


