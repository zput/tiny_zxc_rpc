# tiny_zxc_rpc
A lightweight RPC

## 简介

轻量级的RPC: 
-- 底层使用自己的支持多线程网络库zxc_net.
-- 中间层是序列化,反序列化, 其中反序列化是C++模板进行调用函数的参数列表,作为依据,
对客户端的的数据进行反序列化, 取得参数,调用函数, 把结果返回给客户端. 

