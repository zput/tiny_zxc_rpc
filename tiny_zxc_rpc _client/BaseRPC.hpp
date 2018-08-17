
#ifndef BASERPC_HPP
#define BASERPC_HPP

#include <string>
#include <map>
#include <string>
#include <sstream>
#include <functional>

#include "SerializationAntiserialization.hpp"



namespace zxc_rpc {

	class SerializationAntiserialization;

	template<typename T>
	struct type_xx { typedef T type; };

	template<>
	struct type_xx<void> { typedef int8_t type; };


	// 打包帮助模板
	template<typename Tuple, std::size_t... Is>
	void package_params_impl(SerializationAntiserialization& ds, const Tuple& t, std::index_sequence<Is...>)
	{
		std::initializer_list<int>{((ds << std::get<Is>(t)), 0)...};
	}

	template<typename... Args>
	void package_params(SerializationAntiserialization& ds, const std::tuple<Args...>& t)
	{
		package_params_impl(ds, t, std::index_sequence_for<Args...>{});
	}

	// 用tuple做参数调用函数模板类
	template<typename Function, typename Tuple, std::size_t... Index>
	decltype(auto) invoke_impl(Function&& func, Tuple&& t, std::index_sequence<Index...>)
	{
		return func(std::get<Index>(   std::forward<Tuple>(t)   )...);
	}

	template<typename Function, typename Tuple>
	decltype(auto) invoke(Function&& func, Tuple&& t)
	{
		constexpr auto size = std::tuple_size<typename std::decay<Tuple>::type>::value;
		return invoke_impl(std::forward<Function>(func), std::forward<Tuple>(t), std::make_index_sequence<size>{});
	}

	// 调用帮助类，主要用于返回是否void的情况
	template<typename R, typename F, typename ArgsTuple>
	typename std::enable_if<std::is_same<R, void>::value, typename type_xx<R>::type >::type
		call_helper(F f, ArgsTuple args) {
		invoke(f, args);
		return 0;
	}

	template<typename R, typename F, typename ArgsTuple>
	typename std::enable_if<!std::is_same<R, void>::value, typename type_xx<R>::type >::type
		call_helper(F f, ArgsTuple args) {
		return invoke(f, args);
	}







	// rpc 类定义
	class BaseRPC
	{
	public:
		BaseRPC(){ m_error_code = RPC_ERR_SUCCESS;};
		~BaseRPC(){};
		enum rpc_role {
			RPC_CLIENT,
			RPC_SERVER
		};
		enum rpc_err_code {
			RPC_ERR_SUCCESS = 0,
			RPC_ERR_FUNCTIION_NOT_BIND,
			RPC_ERR_RECV_TIMEOUT
		};

		// wrap return value
		template<typename T>
		class value_t {
		public:
			typedef typename type_xx<T>::type type;
			typedef std::string msg_type;
			typedef uint16_t code_type;

			value_t() { code_ = 0; msg_.clear(); }
			bool valid() { return (code_ == 0 ? true : false); }
			int error_code() { return code_; }
			std::string error_msg() { return msg_; }
			type val() { return val_; }

			void set_val(const type& val) { val_ = val; }
			void set_code(code_type code) { code_ = code; }
			void set_msg(msg_type msg) { msg_ = msg; }

			friend SerializationAntiserialization& operator >> (SerializationAntiserialization& in, value_t<T>& d) {
				in >> d.code_ >> d.msg_;
				if (d.code_ == 0) {
					in >> d.val_;
				}
				return in;
			}
			friend SerializationAntiserialization& operator << (SerializationAntiserialization& out, value_t<T> d) {
				out << d.code_ << d.msg_ << d.val_;
				return out;
			}
		private:
			code_type code_;
			msg_type msg_;
			type val_;
		};

		// network
		void as_client()
		{
			m_role = RPC_CLIENT;
		}
		
	    void as_server()
		{
			m_role = RPC_SERVER;
			/*
				m_socket = new zmq::socket_t(m_context, ZMQ_REP);
				ostringstream os;
				os << "tcp://*:" << port;
				m_socket->bind (os.str());
			*/
		}
	   
    
		// network
	//	void as_client(std::string ip, int port);
	//    void as_server(int port);
	//	void send(zmq::message_t& data);
	//	void recv(zmq::message_t& data);
	//	void set_timeout(uint32_t ms);
		//void run();

	 
		void handleMessage(std::string& message)
		{
			// only server can call
			if (m_role != RPC_SERVER) {
				return;
			}
			//////////////////////////////////////////////////////////////////
						StreamBuffer iodev(message.c_str(), message.size());
						SerializationAntiserialization ds(iodev);

						std::string funname;
						ds >> funname;
						
						SerializationAntiserialization* r = call_(funname, ds.current(), ds.size()- funname.size());
						message.clear() ; 
						char* temp_p = new char[r->size()+1]();
						memcpy (temp_p, r->data(), r->size());
						temp_p[r->size()] = '\0';
						message = temp_p;
			  /////////////////////////////////////////////////////////////////
				
				delete r;		
		}
		
	public:
	  // server   -------------
		template<typename F>
		void bind(std::string name, F func);

		template<typename F, typename S>
		void bind(std::string name, F func, S* s);

		
	   // client   -------------
					template<typename R, typename... Params>
					SerializationAntiserialization  call(std::string name, Params... ps) {
						using args_type = std::tuple<typename std::decay<Params>::type...>;
						constexpr auto N = std::tuple_size<typename std::decay<args_type>::type>::value;
						args_type args = std::make_tuple(ps...);

						SerializationAntiserialization ds;
						ds << name;
						package_params(ds, args);
						return ds ;
						           //return net_call<R>(ds);
					}

					template<typename R>
					SerializationAntiserialization  call(std::string name) {
						SerializationAntiserialization ds;
						ds << name;
						return ds;
						          //return net_call<R>(ds);
					}

	private:
		
				 SerializationAntiserialization*  call_(std::string name, const char* data, int len)
				{
					SerializationAntiserialization* ds = new SerializationAntiserialization();
					if (m_handlers.find(name) == m_handlers.end()) {
						(*ds) << value_t<int>::code_type(RPC_ERR_FUNCTIION_NOT_BIND);
						(*ds) << value_t<int>::msg_type("function not bind: " + name);
						return ds;
					}
					auto fun = m_handlers[name];
					fun(ds, data, len);
					ds->reset();
					return ds;
				}
		
		
		
		
		
	//	template<typename R>
	//	value_t<R> net_call(SerializationAntiserialization& ds);
   //////////////////////////////////////////////////////////////////////////////////////////////////
		template<typename F>
		void callproxy(F fun, SerializationAntiserialization* pr, const char* data, int len);

		template<typename F, typename S>
		void callproxy(F fun, S* s, SerializationAntiserialization* pr, const char* data, int len);
   /////////////////////////////////////////////////////////////////////////////////////////////////


		// 函数指针
		template<typename R, typename... Params>
		void callproxy_(R(*func)(Params...), SerializationAntiserialization* pr, const char* data, int len);

		// 类成员函数指针
		template<typename R, typename C, typename S, typename... Params>
		void callproxy_(R(C::* func)(Params...), S* s, SerializationAntiserialization* pr, const char* data, int len) {

			using args_type = std::tuple<typename std::decay<Params>::type...>;

			SerializationAntiserialization ds(StreamBuffer(data, len));
			constexpr auto N = std::tuple_size<typename std::decay<args_type>::type>::value;
			args_type args = ds.get_tuple < args_type >(std::make_index_sequence<N>{});

			auto ff = [=](Params... ps)->R {
				return (s->*func)(ps...);
			};
			typename type_xx<R>::type r = call_helper<R>(ff, args);

			value_t<R> val;
			val.set_code(RPC_ERR_SUCCESS);
			val.set_val(r);
			(*pr) << val;
		}

		// functional
		template<typename R, typename... Params>
		void callproxy_(std::function<R(Params... ps)> func, SerializationAntiserialization* pr, const char* data, int len) {

			using args_type = std::tuple<typename std::decay<Params>::type...>;
                                                   
			SerializationAntiserialization ds(StreamBuffer(data, len));
			constexpr auto N = std::tuple_size<    typename std::decay<args_type>::type  >::value;
			
			args_type args = ds.get_tuple < args_type >(std::make_index_sequence<N>{});

			typename type_xx<R>::type r = call_helper<R>(func, args);

			value_t<R> val;
			val.set_code(RPC_ERR_SUCCESS);
			val.set_val(r);
			(*pr) << val;      //----pr----//
		}

		
		
		
	private:
		std::map<std::string, std::function<void(SerializationAntiserialization*, const char*, int)>> m_handlers;

		// zmq::context_t m_context;
		// zmq::socket_t* m_socket;

		rpc_err_code m_error_code;
		int m_role;
	};
	
























// network



template<typename F>
 void BaseRPC::bind( std::string name, F func )
{
	 //std::map<std::string, std::function<void(SerializationAntiserialization*, const char*, int)>>
	 //m_handlers;

	 m_handlers[name] = std::bind(&BaseRPC::callproxy<F>, this, func, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

template<typename F, typename S>
 void BaseRPC::bind(std::string name, F func, S* s)
{
	m_handlers[name] = std::bind(&BaseRPC::callproxy<F, S>, this, func, s, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

template<typename F>
 void BaseRPC::callproxy( F fun, SerializationAntiserialization* pr, const char* data, int len )
{
	/////////////////////////////////////////////
	/////////////////////////////////////////////
	callproxy_(fun, pr, data, len);
}

template<typename F, typename S>
 void BaseRPC::callproxy(F fun, S * s, SerializationAntiserialization * pr, const char * data, int len)
{
	callproxy_(fun, s, pr, data, len);
}




 // 函数指针
 template<typename R, typename... Params>
 void BaseRPC::callproxy_(R(*func)(Params...), SerializationAntiserialization* pr, const char* data, int len) {
	 callproxy_(std::function<R(Params...)>(func), pr, data, len);
 }


//
//
//template<typename R>
// BaseRPC::value_t<R> BaseRPC::net_call(SerializationAntiserialization& ds)
//{
//	zmq::message_t request(ds.size() + 1);
//	memcpy(request.data(), ds.data(), ds.size());
//	if (m_error_code != RPC_ERR_RECV_TIMEOUT) {
//		send(request);
//	}
//	zmq::message_t reply;
//	recv(reply);
//	value_t<R> val;
//	if (reply.size() == 0) {
//		// timeout
//		m_error_code = RPC_ERR_RECV_TIMEOUT;
//		val.set_code(RPC_ERR_RECV_TIMEOUT);
//		val.set_msg("recv timeout");
//		return val;
//	}
//	m_error_code = RPC_ERR_SUCCESS;
//	ds.clear();
//	ds.write_raw_data((char*)reply.data(), reply.size());
//	ds.reset();
//
//	ds >> val;
//	return val;
//}
//
//







 //////////////////////////////////////////////////////////////////////////////////////
	
}

#endif   //  BASERPC_HPP


