#include<iostream>
#include<boost/asio.hpp>
#include<boost/bind.hpp>
#include<boost/format.hpp>
using boost::format;
namespace bsys = boost::system;
namespace basio = boost::asio;
namespace baip = basio::ip;



#define Log(fmt) std::clog << format("[" __FILE__ "] " fmt "\n")

size_t read_complete(char* buf, bsys::error_code const& err, size_t bytes)
{
	if ( err ) return 0;
	bool found = std::find(buf, buf + bytes, '\n') < buf + bytes;
	return found ? 0:1;
}

void handle_connections(basio::io_service& io_service)
{
	baip::tcp::acceptor acceptor(io_service, baip::tcp::endpoint(baip::tcp::v4(), 8001));
	char buf[1024];
	boost::system::error_code err;
	for(;;)
	{
		baip::tcp::socket sock(io_service);
		acceptor.accept(sock);
		do
		{
			int bytes = basio::read(sock, basio::buffer(buf), boost::bind(read_complete, buf, _1, _2), err);
			if(err)
			{
				Log("read error:%s") % err;
				break;
			}
			std::string msg(buf, bytes);
			basio::write(sock, basio::buffer(msg),err);
			if(err)
			{
				Log("write error:%s") % err;
				break;
			}
		}while(0);
		sock.close();
	}
}

int main()
{
	basio::io_service io_service;
	handle_connections(io_service);
	return 0;
}
