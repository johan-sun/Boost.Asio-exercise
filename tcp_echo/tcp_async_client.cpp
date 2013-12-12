#include<iostream>
#include<string>
#include<algorithm>
#include<boost/asio.hpp>
#include<boost/smart_ptr.hpp>
#include<boost/bind.hpp>
#include<boost/format.hpp>
#include<boost/thread.hpp>
namespace asio = boost::asio;
namespace ip = asio::ip;
using boost::format;


#define Log(fmt) std::clog << format("[" __FILE__ "] " fmt "\n")


struct until
{
	until(const char* buf, char delim)
	{
		_buf = buf;
		_delim = delim;
		_last = 0;
	}
	size_t operator()(boost::system::error_code const& err, size_t byte)
	{
		if(err) return 0;
		auto start = _buf + _last;
		auto end = _buf + byte;
		_last = byte;
		return std::find(start, end, _delim) == end? 1:0;
	}
private:
	const char* _buf;
	char _delim;
	size_t _last;
};

class talk_to_svr:public boost::enable_shared_from_this<talk_to_svr>,
	boost::noncopyable
{
	typedef talk_to_svr self_type;
	talk_to_svr(asio::io_service& servie, const std::string& message)
		:_sock(servie),_started(false),_message(message)
	{}
	
	void start(ip::tcp::endpoint const& ep)
	{
		_started = true;
		_sock.async_connect(ep, boost::bind(&self_type::connect_done, shared_from_this(),_1));
	}
public:
	typedef boost::system::error_code error_code;
	typedef boost::shared_ptr<talk_to_svr> ptr;
	static ptr start(asio::io_service& servie, ip::tcp::endpoint ep, std::string const& message)
	{
		ptr new_(new talk_to_svr(servie, message + "\n"));
		new_->start(ep);
		return new_;
	}
	void stop()
	{
		if( !_started ) return;
		_started = false;
		_sock.close();
	}

private:
	void connect_done(error_code const& err)
	{
		if(!err) 
		{
			do_write();
		}
		else Log("connect error:%") % err;
	}
	void do_write()
	{
		if(!_started) return;
		std::copy(_message.cbegin(),_message.cend(), _write_buffer);
		asio::async_write(_sock, asio::buffer(_write_buffer, _message.size()), asio::transfer_all(),
				boost::bind(&self_type::write_done, shared_from_this(),_1,_2));
	}
	void write_done(error_code const& err, size_t /*bytes*/)
	{
		if (!err)
		{
			do_read();
		}
		else 
		{
			Log("write error:%s") % err;
			stop();
		}
	}
	void do_read()
	{
		//asio::async_read(_sock, _read_buf, , boost::bind(&self_type::read_done, shared_from_this(),_1,_2));
		asio::async_read(_sock, asio::buffer(_read_buf), until(_read_buf, '\n'), 
				boost::bind(&self_type::read_done, shared_from_this(),_1,_2));
	}
	void read_done(error_code const& err, size_t bytes)
	{
		if(!err)
		{
			std::string str(_read_buf, _read_buf + bytes - 1);
			Log("recive from server:%s") % str;
		}	
		else
		{
			Log("read error:%s") % err;
			stop();
		}
	}
private:
	ip::tcp::socket _sock;
	enum { max_msg = 1024 };
	char _read_buf[max_msg];
	char _write_buffer[max_msg];
	bool _started;
	std::string _message;
};


int main()
{
	asio::io_service service;
	ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 8001);
	char const* messages[] = {
		"John say hi",
		"so does James",
		"Boost.Asio is fun"
	};
	for(auto& mesg:messages)
	{
		talk_to_svr::start(service, ep, mesg);
		boost::this_thread::sleep(boost::posix_time::millisec(100));
	}
	service.run();
	return 0;
}
